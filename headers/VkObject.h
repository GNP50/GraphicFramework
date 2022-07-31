//
// Created by Pierluigi Carlino on 08/07/22.
//

#ifndef DB_VKOBJECT_H
#define DB_VKOBJECT_H
#include "commonVulkanIncludes.h"
#include "VulkanBuffers.h"

namespace Vulkan{

    class Object{
    protected:
        vector<glm::vec3> vertexes;
        vector<glm::vec2> textCoords;
        vector<uint32_t> indicies;
        vector<glm::vec2> uvs;

        vector<UniformBuffer<UniformBufferObject>> ubo;

    public:
        void setVertex(vector<glm::vec3> vertex);
        void setIndicies(vector<uint32_t> indicies);
        void setTextCoords(vector<glm::vec2> coords);
        virtual void loadData(){};
    public:
        const vector<glm::vec3> &getVertex() const;
        const vector<uint32_t> &getIndicies() const;
        const vector<glm::vec2> &getTextCoords() const;

        void create(bool points = false){
            this->loadData();

            this->setVertex(vertexes);
            this->setTextCoords(uvs);

            if(!points)
                this->setIndicies(indicies);
            else{
                indicies.erase(indicies.begin(),indicies.end());
                indicies.resize(vertexes.size()*3);
                for(int i=0;i<vertexes.size();i++){
                    indicies.push_back(i);
                    indicies.push_back(i);
                    indicies.push_back(i);
                }
            }



        }


        template<typename Info,typename InfoExtracter>
        void useInformation(std::function<void()> infoExtractFunc,InfoExtracter extracter ){
            extracter.extractInfo(this);
            Info& information = extracter.getInfo();

            infoExtractFunc(information);
        }

    };


    class BlenderObj : public Object{

        string filename;
        vector<glm::vec2> file_uv;
        vector<glm::vec3> file_vert;
        vector<glm::vec<3,uint>> file_ind;

        const int triangle_in_a_face = 2;

        virtual void loadData() override{
            std::ifstream file(filename);
            std::string line;
            while (std::getline(file, line))
            {
                if(line.empty())
                    continue;

                auto start = splitLine(line,' ')[0];

                if(start == "v")
                        parseVertex(line);
                else if(start == "f")
                        parseIndex(line);
                else if(start == "vt")
                        parseTextureUV(line);

            }

            int f=0;
            for(int i=0;i<file_ind.size();i+=3*triangle_in_a_face-2){
                    for(int j=0;j<3;j++){
                        auto ind_to_select = file_ind[i+j];
                        vertexes.push_back(file_vert[ind_to_select.x]);
                        uvs.push_back(file_uv[ind_to_select.y]);
                        indicies.push_back(f++);
                    }

                    auto ind_to_select = file_ind[i+2];
                    vertexes.push_back(file_vert[ind_to_select.x]);
                    uvs.push_back(file_uv[ind_to_select.y]);
                    indicies.push_back(f++);

                    ind_to_select = file_ind[i+3];
                    vertexes.push_back(file_vert[ind_to_select.x]);
                    uvs.push_back(file_uv[ind_to_select.y]);
                    indicies.push_back(f++);

                    ind_to_select = file_ind[i];
                    vertexes.push_back(file_vert[ind_to_select.x]);
                    uvs.push_back(file_uv[ind_to_select.y]);
                    indicies.push_back(f++);
            }
        }

        void parseVertex(string vertex){
            auto vertexes = splitLine(vertex,' ');
            this->file_vert.push_back(glm::vec3{
                    std::stof(vertexes[1]),
                    std::stof(vertexes[2]),
                    std::stof(vertexes[3])}
                    );
        }

        void parseIndex(string index){
            //the format is
            //f v/vt/vn v/vt/vn v/vt/vn v/vt/vn

            auto indexes = splitLine(index,' ');
            indexes.erase(indexes.begin());

            for(auto ind:indexes) {
                auto inds = splitLine(ind,'/');
                this->file_ind.push_back(glm::vec<3,uint>(
                        std::stoi(inds[0])-1,
                        std::stoi(inds[1])-1,
                        std::stoi(inds[2])-1));
            }

        }

        void parseTextureUV(string uv){
            auto uvs = splitLine(uv,' ');
            this->file_uv.push_back(glm::vec2{
                    std::stof(uvs[1]),
                    std::stof(uvs[2])});
        }

        vector<string> splitLine(const string toSplit,const char separator){

            std::vector<string> outputArray;
            std::stringstream streamData(toSplit);
            std::string val;

            while (std::getline(streamData, val, separator)) {
                outputArray.push_back(val);
            }

            return outputArray;
        }

    public:
        BlenderObj(string filename):filename(filename){

        }


    };

    template<typename Information>
    class ObjectInformationExtracter{
    protected:
        Information info;
    public:
        virtual void extractInfo(Object &object) = 0;
        Information& getInfo(){return info;}
    };

    struct GroundInformation{
            float w;
            float h;
            float h_step;
            float w_step;

            float** f_mapping;

        int getMappingW(){return std::ceil(w/this->w_step);}
        int getMappingH(){return std::ceil(h/this->h_step);}

        void destroyMapping(){
            const int w = getMappingW();
            #pragma omp parallel for num_threads(MAX)
            for(int i=0;i<w;i++){
                free(this->f_mapping[i]);
            }

            free(this->f_mapping);
        }
    };

    class GroundInformationExtracter:public ObjectInformationExtracter<GroundInformation>{
    public:
        virtual void extracInfo(Object &object){
            float max_x = std::numeric_limits<float>::min();
            float max_z = std::numeric_limits<float>::min();

            float min_x = std::numeric_limits<float>::max();
            float min_z = std::numeric_limits<float>::max();

            float min_x_step = std::numeric_limits<float>::max();
            float min_z_step = std::numeric_limits<float>::max();

#pragma region ExtractInformationToBuildTheMatrixGround

            auto vertexes = object.getVertex();
            for(int i =0;i<vertexes.size();i++){
                if(max_x<vertexes[i].x)
                    max_x = vertexes[i].x;

                if(max_z<vertexes[i].z)
                    max_z = vertexes[i].z;


                if(min_x>vertexes[i].x)
                    min_x = vertexes[i].x;

                if(min_z>vertexes[i].z)
                    min_z = vertexes[i].z;

                for(int j=i+1;j<vertexes.size();j++){
                    auto steps = glm::abs(vertexes[i]-vertexes[j]);

                    if(steps.x<min_x_step)
                        min_x_step = steps.x;

                    if(steps.z<min_z_step)
                        min_z_step = steps.z;
                }
            }

            this->info.w = max_x-min_x;
            this->info.h = max_z-min_z;

            this->info.h_step = min_z_step;
            this->info.w_step = min_x_step;




#pragma endregion ExtractInformationToBuildTheMatrixGround

#pragma region BuildMatrixGround
            const int w = info.getMappingW();
            this->info.f_mapping = (float**)malloc(sizeof(float*)*w);

#pragma omp parallel for num_threads(MAX)
            for(int i=0;i<w;i++){
                const int h = this->info.getMappingH();
                this->info.f_mapping = (float**)malloc(sizeof(float)*h);
            }

#pragma omp parallel for num_threads(MAX)
            for(auto vertex:vertexes){
                int i = std::floor(vertex.x/this->info.w_step);
                int j = std::floor(vertex.z/this->info.h_step);

                this->info.f_mapping[i][j] = vertex.y;
            }
#pragma region FinishEndMatrixGround

        }
    };
}
#endif //DB_VKOBJECT_H
