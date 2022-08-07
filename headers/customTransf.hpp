//
// Created by Pierluigi Carlino on 19/04/22.
//
#ifndef CUSTOM_TRANSFORMATION
#define CUSTOM_TRANSFORMATION

#include "commonVulkanIncludes.h"


enum Screen{
    top,
    bottom,
    left,
    right
};
class TransfBuff{
private:
    std::vector<glm::mat4x4> transformations;
    float arrayRapp[16];
    bool isVulkan = true;
    bool isDebug = false;

    TransfBuff(){

    };

    const glm::vec3
            xAxis = glm::vec3 (1.0f,0.0f,0.0f),
            yAxis = glm::vec3 (0.0f,1.0f,0.0f),
            zAxis = glm::vec3 (0.0f,0.0f,1.0f);


public:

    TransfBuff& addRotation(float anglex,float angley,float anglez, glm::vec3 axis){
        addTranslation(-axis);
        addYRotation(-angley);
        addZRotation(-anglez);

        addXRotation(anglex);

        addZRotation(anglez);
        addYRotation(angley);
        addTranslation(axis);
        return *this;
    };

    TransfBuff& addXRotation(float angle){
        transformations.push_back(glm::rotate(glm::mat4x4(1.0f),glm::radians(angle),xAxis));
        return *this;
    };


    TransfBuff& addYRotation(float angle){
        auto start = glm::mat4x4 (1.0f);

        transformations.push_back(glm::rotate(start,glm::radians(angle),yAxis));

        return *this;
    };

    TransfBuff& addZRotation(float angle){
        transformations.push_back(glm::rotate(glm::mat4x4(1.0f),glm::radians(angle),zAxis));
        return *this;
    };


    TransfBuff& flatOverZ(){
        auto proj = glm::mat4x4 (1.0f);
        proj[2][2] = 0.0f;
        transformations.push_back(proj);
        return *this;
    };


    TransfBuff& flatOverY(){
        auto proj = glm::mat4x4 (1.0f);
        proj[1][1] = 0.0f;
        transformations.push_back(proj);
        return *this;
    };

    TransfBuff& flatOverX(){
        auto proj = glm::mat4x4 (1.0f);
        proj[0][0] = 0.0f;
        transformations.push_back(proj);
        return *this;
    };


    TransfBuff& addCenteredScaling(float factor = 1,glm::vec3 center = glm::vec3(0.0f,0.0f,0.0f)){
        addTranslation(-center);
        addShrink(factor);
        addTranslation(center);
        return *this;

    }


    TransfBuff& addMirroring(float yzAngle = 1,glm::vec3 center = glm::vec3(0.0f,0.0f,0.0f)){
        addTranslation(-center);
        addXRotation(-yzAngle);
        addFlixY();
        addXRotation(yzAngle);
        addTranslation(center);
        return *this;

    }


    TransfBuff& addFlixX(){
        auto toFlip = glm::mat4x4(1.0f);
        toFlip[0][0] = -1.0f;
        transformations.push_back(toFlip);
        return *this;

    }

    TransfBuff& addFlixY(){
        auto toFlip = glm::mat4x4(1.0f);
        toFlip[1][1] = -1.0f;
        transformations.push_back(toFlip);
        return *this;

    }

    TransfBuff& addFlixZ(){
        auto toFlip = glm::mat4x4(1.0f);
        toFlip[2][2] = -1.0f;
        transformations.push_back(toFlip);
        return *this;

    }

    TransfBuff& addTranslation(glm::vec3 direction, float module = 1){
        glm::mat4x4  toPush(1.0f);
        toPush[3][0] = module*direction[0];
        toPush[3][1] = module*direction[1]*(isVulkan ? 1 : -1);
        toPush[3][2] = module*direction[2];

        transformations.push_back(toPush);
        return *this;
    };

    TransfBuff& addShrink(float factor = 1.0f){
        glm::mat4x4  toPush(1.0f);
        toPush[0][0] /= factor;
        toPush[1][1] /= factor;
        toPush[2][2] /= factor;

        transformations.push_back(toPush);
        return *this;
    };

    TransfBuff& addDilatation(glm::vec3 dilationVector = glm::vec3 (1.0f,1.0f,1.0f)){
        glm::mat4x4  toPush(1.0f);
        toPush[0][0] *= dilationVector[0];
        toPush[1][1] *= dilationVector[1];
        toPush[2][2] *= dilationVector[2];

        transformations.push_back(toPush);
        return *this;
    };

    TransfBuff& addShearX(float hy = 1.0f,float hz = 1.0f){
        glm::mat4x4  toPush(1.0f);
        toPush[0][1] = hy;
        toPush[1][2] = hz;

        transformations.push_back(toPush);
        return *this;
    };

    TransfBuff& addShearZ(float hx = 1.0f,float hy = 1.0f){
        glm::mat4x4  toPush(1.0f);
        toPush[2][0] = hx;
        toPush[2][1] = hy;

        transformations.push_back(toPush);
        return *this;
    };

    TransfBuff& addShearY(float hx = 0.0f,float hz = 0.0f){
        glm::mat4x4  toPush(1.0f);
        toPush[1][0] = hx;
        toPush[1][2] = hz;

        transformations.push_back(toPush);
        return *this;
    };
    TransfBuff* addTransformation(glm::mat4x4 toAdd){
        transformations.push_back(toAdd);
        return this;
    };

    template<typename T>
    T doTransformations(int start = 0,int end = 0,bool reverse = false){
        if(end==0)
            end = transformations.size();

        int inc = 1;

        glm::mat4x4 toRet(1.0);
        for(int i=start;i<end;i=i+inc) {
            auto toMul = reverse ? glm::inverse(transformations[i]) : transformations[i];
            toRet = reverse ? toRet*toMul:
                    toMul*toRet;
            if(isDebug)
                std::cout<<toMul<<std::endl;

        }



        if(isDebug)
            std::cout<<toRet<<std::endl;

        if constexpr(std::same_as<T,float*>) {


            for (int i = 0; i < 16; i++)
                arrayRapp[i] = toRet[i % 4][i / 4];

            return arrayRapp;
        }else if constexpr(std::same_as<T,glm::mat4x4>) {
            return toRet;
        }
    };

    static TransfBuff* instance();
};




class ProjectBuffer{
    std::vector<glm::mat4x4> transformations;
    float arrayRapp[16];
    float w = 1.0f,n = 1.0f,f = 1.0f;
    bool isVulkan = true;


public:
    static ProjectBuffer* instance(float w,float n,float f);

    ProjectBuffer(
            float w = 1.0f,
            float n = 1.0f,
            float f = 1.0f
    ){
        if(isVulkan)
        {
            n *=-1;
            f *=-1;
        }

        this->w = w;
        this->n = n;
        this->f = f;
    };

    const glm::vec3
            xAxis = glm::vec3 (1.0f,0.0f,0.0f),
            yAxis = glm::vec3 (0.0f,1.0f,0.0f),
            zAxis = glm::vec3 (0.0f,0.0f,1.0f);

    glm::mat4  orthogonalProj(float a){
        glm::mat4x4  toRet = glm::mat4x4 (
                1.0f/w,0.0f,0.0f,0.0f,
                0.0f,-a/w,0.0f,0.0f,
                0.0f,0.0f,-2/(f-n),0.0f,
                0.0f,0.0f,-(f+n)/(f-n),1.0f
        );

        if(isVulkan)
            toRet[1][1] *= -1;

        return toRet;
    };


    glm::mat4   isometricProj(float a){
        glm::mat4x4 xRotation = TransfBuff::instance()->addXRotation(35.26f).doTransformations<glm::mat4x4>();
        glm::mat4x4 yRotation = TransfBuff::instance()->addYRotation(45.00f).doTransformations<glm::mat4x4>();

        return perspectiveProj(a)*xRotation*yRotation;
    }


    glm::mat4  dimetricProj(float a,float angle){
        glm::mat4x4 xRotation = TransfBuff::instance()->addXRotation(angle).doTransformations<glm::mat4x4>();
        glm::mat4x4 yRotation = TransfBuff::instance()->addYRotation(45.00f).doTransformations<glm::mat4x4>();

        return perspectiveProj(a)*xRotation*yRotation;

    }


    glm::mat4  trimetricProj(float a,float alpha,float betha){
        glm::mat4x4 xRotation = TransfBuff::instance()->addXRotation(alpha).doTransformations<glm::mat4x4>();
        glm::mat4x4 yRotation = TransfBuff::instance()->addYRotation(betha).doTransformations<glm::mat4x4>();

        return perspectiveProj(a)*xRotation*yRotation;
    }


    glm::mat4  cabinetProj(float a){
        auto obl = glm::mat4x4(1.0f);
        obl[2][0] = -0.5* 1/sqrt(2);
        obl[2][1] = -0.5* 1/sqrt(2);
        return perspectiveProj(a)*obl;
    }

    glm::mat4  cavalierProj(float a){
        auto obl = glm::mat4x4(1.0f);
        obl[2][0] = -1/sqrt(2);
        obl[2][1] = -1/sqrt(2);
        return perspectiveProj(a)*obl;
    }



    glm::mat4  perspectiveProj(float a){
        glm::mat4x4  toRet = glm::mat4x4 (
                1.0f/w,0.0f,0.0f,0.0f,
                0.0f,-a/w,0.0f,0.0f,
                0.0f,0.0f,-1/(f-n),-n/(f-n),
                0.0f,0.0f,0.0f,1.0f
        );


        if(isVulkan)
            toRet[1][1] *= -1;

        return toRet;
    };


    glm::mat4  perspectiveProj(
            float tetha ,
            float ration,
            std::map<Screen,float> mapping = {}
    ) {

        float t,b,r,l;

        if(!mapping.contains(Screen::top))
            t = n* tan(tetha/2);
        else
            t = mapping.at(Screen::top);

        if(!mapping.contains(Screen::bottom))
            b = -n* tan(tetha/2);
        else
            b = mapping.at(Screen::bottom);

        if(!mapping.contains(Screen::left))
            l = -ration*n* tan(tetha/2);
        else
            l = mapping.at(Screen::left);


        if(!mapping.contains(Screen::right))
            r = ration*n* tan(tetha/2);
        else
            r = mapping.at(Screen::right);



        glm::mat4 toRet = glm::mat4(1.0f);

        toRet[0][0] = 2*n/(r-l);
        toRet[1][1] = 2*n/(t-b);
        toRet[2][2] = (this->f + this->n) / (this->n - this->f);
        toRet[2][3] = -1;
        toRet[3][3] = 0;
        toRet[3][2] = (2 * this->f * this->n) / (this->n - this->f);
        toRet[2][0] = (r+l) / (r-l);


        if(isVulkan)
            toRet[1][1] *= -1;

        return toRet;
    }
};




class Camera_{
private:
    glm::vec3 position;
public:
    Camera_(glm::vec3 position){
        this->position = position;
    }

    glm::mat4  lookIn(float rho,float bheta,float alpha){
        return glm::inverse(TransfBuff::instance()
                                    ->addZRotation(-rho)
                                    .addXRotation(-bheta)
                                    .addYRotation(-alpha)
                                    .addTranslation(-this->position)
                                    .doTransformations<glm::mat4x4>());
    }

    glm::mat4 lookAt(glm::vec3 obj,glm::vec3 upVector = glm::vec3(0.0f,1.0f,0.0f)){
        upVector = glm::normalize(upVector);

        auto vz = (this->position-obj);
        vz = glm::normalize(vz);

        auto vx = glm::cross(upVector,vz);
        vx = glm::normalize(vx);

        auto vy = glm::cross(vz,vx);

        auto toRet = glm::mat4 (1.0f);

        toRet[0][0] = vx[0];
        toRet[0][1] = vx[1];
        toRet[0][2] = vx[2];

        toRet[1][0] = vy[0];
        toRet[1][1] = vy[1];
        toRet[1][2] = vy[2];

        toRet[2][0] = vz[0];
        toRet[2][1] = vz[1];
        toRet[2][2] = vz[2];

        toRet[3][0] = this->position[0];
        toRet[3][1] = this->position[1];
        toRet[3][2] = this->position[2];

        return glm::inverse(toRet);
    }

    static Camera_* instance(glm::vec3 position){
        static Camera_ *instance;
        instance = new Camera_(position);

        return instance;
    }
};

#endif