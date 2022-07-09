//
// Created by Pierluigi Carlino on 24/04/22.
//

#ifndef ASS9_BASICGEOMETRY_H
#define ASS9_BASICGEOMETRY_H
#include "customTransf.hpp"
#include <fstream>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
};


class Object{
protected:
    virtual void createIndiciesList() = 0;
    virtual void createVertexList() = 0;

public:
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> norms;

    std::vector<uint32_t> indiciesList;
    std::string filename;

    Object(std::string filename){
        this->filename = filename;
    }


    template<typename T>
    T getVerticies(){
        if constexpr(std::same_as<T,float*>) {

            float *arrayRapp = (float*) malloc((sizeof(float)*points.size()*3));

#pragma omp for
            for (int i = 0; i < points.size()*3; i++)
                arrayRapp[i] = points.at(i/3)[i%3];


            return arrayRapp;
        }else if constexpr(std::same_as<T,std::vector<float>>) {

            std::vector<float> vectorRapp;

#pragma omp for
            for (int i = 0; i < points.size()*3; i++)
                vectorRapp.push_back(points.at(i/3)[i%3]);


            return vectorRapp;
        }else if constexpr(std::same_as<T,std::vector<Vertex>>) {
            auto toRet = std::vector<Vertex>(points.size());

            #pragma omp for
            for (int i = 0; i < points.size(); i++) {
                toRet[i].pos = points[i];
                toRet[i].norm = norms[i];
            }

            return toRet;
        }else if constexpr(std::same_as<T,std::vector<glm::vec3>>) {
            return points;
        }
    }

    template<typename T>
    T getIndiciesList(){
        if constexpr(std::same_as<T,int*>) {
            createIndiciesList();
            return indiciesList.data();
        }else if constexpr(std::same_as<T,std::vector<uint32_t>>) {
            return indiciesList;
        }
    }

    template<typename T>
    T getNormList(){
        if constexpr(std::same_as<T,int*>) {

            float *arrayRapp = (float*) malloc((sizeof(float)*norms.size()*3));

#pragma omp for
            for (int i = 0; i < norms.size()*3; i++)
                arrayRapp[i] = norms.at(i/3)[i%3];


            return arrayRapp;
        }else if constexpr(std::same_as<T,std::vector<float>>) {

            std::vector<float> vectorRapp;

#pragma omp for
            for (int i = 0; i < norms.size()*3; i++)
                vectorRapp.push_back(norms.at(i/3)[i%3]);


            return vectorRapp;
        }else if constexpr(std::same_as<T,std::vector<glm::vec3>>) {
            return norms;
        }
    }


    virtual void loadData() = 0;
};



class SliceGeometric:public Object{
protected:
    int slices;
    int cloves;

    void createVertexList() override {

    };
    void createIndiciesList() override{
#pragma omp for
        for(int j=0;j<this->slices-1;j++){
            int off = j*this->cloves;
#pragma omp for
            for(int i=0;i<this->cloves+1;i++){
                this->indiciesList.push_back(i+off);
                this->indiciesList.push_back(i+this->cloves+off);
                this->indiciesList.push_back(
                        (i+1)%this->cloves == 0 ? this->cloves+off: (i+1)%this->cloves+off
                );

                this->indiciesList.push_back(i+this->cloves+off);
                this->indiciesList.push_back(
                        (i+1)%this->cloves == 0 ? this->cloves+off : (i+1)%this->cloves+off
                );
                this->indiciesList.push_back(
                        (i+1)%this->cloves == 0 ? 2*this->cloves+off: (i+1)%this->cloves+off+this->cloves
                );
            }
        }
    };

public:
    SliceGeometric(float slices,float cloves):Object("SliceGeometric"){
        this->slices = slices;
        this->cloves = cloves;
    }

    void loadData() override{
        this->createIndiciesList();
        this->createVertexList();
    }
};


class SlicedSphere:public SliceGeometric{
protected:
    float rho;

public:

    void createIndiciesList(){
        SliceGeometric::createIndiciesList();
        int l = this->indiciesList.size();

#pragma omp for
        for(int j=0;j<l;j++){
            indiciesList.push_back(j-1);
        }
    };

    void createVertexList() override {
        float alpha_step = glm::pi<float>()/this->slices;
        float theta_step = glm::pi<float>()/this->cloves*2;

#pragma omp for
        for(int j=0;j<this->slices-1;j++){
            float alpha = alpha_step*j;
#pragma omp for
            for(int i=0;i<this->cloves+1;i++){
                float theta = theta_step*i;
                this->points[i+j*this->cloves] =
                        glm::vec3(
                                this->rho*glm::cos(theta)*glm::sin(alpha),
                                this->rho*glm::sin(theta)*glm::sin(alpha),
                                this->rho*glm::cos(alpha)
                        );

                //add the norms
                this->norms[i+j*this->cloves] = glm::normalize(this->points[i+j*this->cloves]);

            }
        }
    };

    SlicedSphere(float slices,float cloves,float rho):SliceGeometric(slices,cloves){
        this->slices = slices;
        this->cloves = cloves;
        this->rho = rho;

        this->points.resize(this->slices*this->cloves);
        this->norms.resize(this->slices*this->cloves);
    }
};

class SlicedCylinder:public SliceGeometric{
protected:
    float rho;
    float h;

public:

    void createIndiciesList(){
#pragma omp for
        for(int j=1;j<this->cloves+1;j++){
            indiciesList.push_back(j%this->cloves == 0? this->cloves: j);
            indiciesList.push_back(0);
            indiciesList.push_back((j)%this->cloves == 0? 1: (j+1)%this->cloves);

        }


        SliceGeometric::createIndiciesList();

#pragma omp for
        for(int j=1;j<this->cloves+1;j++){
            indiciesList.push_back(j%this->cloves == 0?
                                   2*this->cloves:
                                   j%this->cloves+this->cloves
            );
            indiciesList.push_back(this->cloves*2+1);
            indiciesList.push_back((j)%this->cloves == 0?
                                   this->cloves+ 1:
                                   (j+1)%this->cloves+this->cloves
            );

        }
    };

    void createVertexList() override {
        float theta_step = glm::pi<float>()/this->cloves*2;

        this->points[this->points.size()-1] =  glm::vec3(
                0,
                this->h/2,
                0);
#pragma omp for
        for(int j=0;j<this->slices;j++){
#pragma omp for
            for(int i=0;i<this->cloves+1;i++){
                float theta = theta_step*i;
                this->points[i+j*this->cloves+1] =
                        glm::vec3(
                                this->rho*glm::cos(theta),
                                (j == 0 ? -1 : 1) *this->h/2,
                                this->rho*glm::sin(theta)
                        );

                //norms
                this->norms[i+j*this->cloves+1] =
                        glm::vec3(
                                this->rho*glm::cos(theta),
                                0.0f,
                                this->rho*glm::sin(theta)
                        );
            }
        }

        this->points[0] =  glm::vec3(
                0,
                -this->h/2,
                0);
    };

    SlicedCylinder(float cloves,float rho,float h,float slices=2):SliceGeometric(slices,cloves){
        this->slices = slices;
        this->cloves = cloves;
        this->rho = rho;
        this->h =h;

        this->points.resize(this->cloves*this->slices+2);
        this->norms.resize(this->cloves*this->slices+2);
    }
};

class SlicedCube:public SlicedCylinder{
public:
    SlicedCube(float h):SlicedCylinder(4,glm::sqrt(h),h){
    }
};

class Spring:public SliceGeometric{
protected:
    int alpha;
    float h;
    float rho;
    float r;

public:
    Spring(int slices,
           int cloves,
           int alpha,
           float h,
           float rho):SliceGeometric(slices,cloves){
        this->alpha = alpha;
        this->h = h;
        this->rho = rho;
        this->r = this->h/(2*this->alpha);

    }

    void createVertexList() override {
        auto baseCirc = std::vector<float>();

        //crating a basic circunference to copy
        //for each iterations
        float clovesStep = 2 * glm::pi<float>() / this->cloves;

        float t = 0;
        while (t <2 * glm::pi<float>()) {
            baseCirc.push_back(0);
            baseCirc.push_back(this->r*glm::sin(t));
            baseCirc.push_back(this->r*glm::cos(t));

            t += clovesStep;
        }

        //starting generating the curve
        float rotBound = 2 * glm::pi<float>() * this->alpha;
        float rotBoundStep = rotBound / this->slices*2;


        t = -rotBound;
        while (t < rotBound) {
            float x_c = this->rho * glm::cos(t);
            float y_c = this->rho * glm::sin(t);
            float z_c = t / (this->alpha)*this->h;

            float z_thetha = t+glm::pi<float>()/2;



            for(int i=0;i<baseCirc.size();i+=3) {
                //xRot
                float a = baseCirc[i+0];
                float b = baseCirc[i+1];
                float c = baseCirc[i+2];


                //zRot
                float n_a = a * glm::cos(z_thetha) - b * glm::sin(z_thetha);
                float n_b = a * glm::sin(z_thetha) + b * glm::cos(z_thetha);

                a = n_a;
                b = n_b;

                //center translation
                a = a + x_c;
                b = b + y_c;
                c = c + z_c;

                this->points.push_back(glm::vec3 (a,c,b));
                this->norms.push_back(glm::normalize(glm::vec3 (a,c,b)-glm::vec3(x_c,y_c,z_c)));
            }

            //self.positions.append((x_c, y_c, z_c))
            t += rotBoundStep;
        }
    }

};


class CustomGeometric:public Object{
protected:
    void createIndiciesList() override {
        return;
    };

    void createVertexList() override {

    };

private:
    std::vector<std::string> splitString(std::string s,std::string del = ";")
    {
        int start = 0;
        int end = s.find(del);
        std::vector<std::string> toRet;

        while (end != -1) {
            toRet.push_back(s.substr(start, end - start));
            start = end + del.size();
            end = s.find(del, start);
        }
        toRet.push_back(s.substr(start, end - start));
        return toRet;
    }

public:
    CustomGeometric(std::string filename): Object(filename){}
    void loadData() override{
        //load vertices
        std::fstream vertFile;

        vertFile.open(this->filename+".vrt",std::ios::in);
        if (vertFile.is_open()){   //checking whether the file is open
            std::string tp;
            while(getline(vertFile, tp)){
                auto lines = this->splitString(tp);
                if(lines.size()>3)
                    throw ("Vert file for "+filename+"is not formatted well" );

#pragma omp for
                for(int i=0;i<lines.size();i++) {
                    glm::vec3 point;

                    point[0] = std::stof(lines[0]);
                    point[1] = std::stof(lines[1]);
                    point[2] = std::stof(lines[2]);

                    this->points.push_back(point);
                }
            }
            vertFile.close(); //close the file object.
        }else
            throw ("Cannot open file " +this->filename+".vrt");


        //load indicies
        std::fstream indFile;

        indFile.open(this->filename+".ind",std::ios::in);
        if (indFile.is_open()){   //checking whether the file is open
            std::string tp;
            while(getline(indFile, tp)){
                auto lines = this->splitString(tp);

#pragma omp for
                for(int i=0;i<lines.size();i++)
                    this->indiciesList.push_back(std::stoi(lines[i]));
            }
            indFile.close(); //close the file object.
        }else
            throw ("Cannot open file " +this->filename+".ind");
    }


};



#endif //ASS9_BASICGEOMETRY_H
