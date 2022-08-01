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

            file.close();
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


        vector<glm::vec3> getFileVertexes(){return this->file_vert;}
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
        std::map<std::pair<int,int>,float> blueprint;
        float step;

        float get(std::pair<int,int> toGet){
            if(blueprint.contains(toGet))
                return blueprint[toGet];

            pair<int,int> closes = {std::numeric_limits<int>::max(),std::numeric_limits<int>::max()};
            float minus = std::sqrt(
                    pow(closes.first-toGet.first,2)+
                    pow(closes.second-toGet.second,2)
            );
            #pragma omp for
            for(auto elems:blueprint){
                auto key = elems.first;
                float square = std::sqrt(
                        pow(key.first-toGet.first,2)+
                        pow(key.second-toGet.second,2)
                        );

                if(square<minus){
                    minus = square;
                    closes = key;
                }
            }

            blueprint.insert({toGet,blueprint[closes]});
            return blueprint[toGet];

        }
    };

    class GroundInformationExtracter:public ObjectInformationExtracter<GroundInformation>{
    protected:
        float step;
    public:
        GroundInformationExtracter(float step):ObjectInformationExtracter<GroundInformation>(),step(step){this->info.step = step;}

        virtual void extractInfo(Object &object) override{


#pragma region ExtractInformationToBuildTheMatrixGround

            vector<glm::vec3> vertexes;

            try{
                vertexes = dynamic_cast<BlenderObj&>(object).getFileVertexes();
            }catch(std::bad_cast& ex){
                vertexes = object.getVertex();
            }

#pragma omp for
            for(int i=0;i<vertexes.size();i++){
                int x = std::floor(vertexes[i].x/step);
                int z = std::floor(vertexes[i].z/step);

                if(this->info.blueprint.contains({x,z}))
                    continue;

                this->info.blueprint.insert({{x,z},vertexes[i].y});
            }

        }
    };
}
#endif //DB_VKOBJECT_H
