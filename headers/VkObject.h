//
// Created by Pierluigi Carlino on 08/07/22.
//

#ifndef DB_VKOBJECT_H
#define DB_VKOBJECT_H
#include "commonVulkanIncludes.h"
#include "VulkanFramework.h"

namespace Vulkan{

    class Object{
    protected:
        vector<glm::vec3> vertexes;
        vector<int> indicies;
        vector<glm::vec2> uvs;

        vector<GlobalUniformBuffer<UniformBufferObject>> ubo;

    public:
        void setVertex(vector<glm::vec3> vertex);
        void setIndicies(vector<int> indicies);

        virtual void loadData(){};
    public:
        const vector<glm::vec3> &getVertex() const;
        const vector<int> &getIndicies() const;

        void create(bool points = false){
            this->loadData();

            this->setVertex(vertexes);
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

        void addUbo(GlobalUniformBuffer<UniformBufferObject> &ubo){
            this->ubo.push_back(ubo);
        }



    };


    class BlenderObj : public Object{

        string filename;


        virtual void loadData() override{
            std::ifstream file(filename);
            std::string line;
            while (std::getline(file, line))
            {
                auto start = splitLine(line,' ')[0];

                if(start == "v")
                        parseVertex(line);
                else if(start == "f")
                        parseIndex(line);
                else if(start == "vt")
                        parseTextureUV(line);

            }
        }

        void parseVertex(string vertex){
            auto vertexes = splitLine(vertex,' ');
            this->vertexes.push_back(glm::vec3{
                    std::stof(vertexes[1]),
                    std::stof(vertexes[2]),
                    std::stof(vertexes[3])});
        }

        void parseIndex(string index){
            //the format is
            //f v/vt/vn v/vt/vn v/vt/vn v/vt/vn

            auto indexes = splitLine(index,' ');
            indexes.erase(indexes.begin());

            for(auto ind:indexes) {
                auto inds = splitLine(ind,'/');
                this->indicies.push_back(std::stoi(inds[0]));
            }

        }

        void parseTextureUV(string uv){
            auto uvs = splitLine(uv,' ');
            this->uvs.push_back(glm::vec2{
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
}
#endif //DB_VKOBJECT_H
