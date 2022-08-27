//
// Created by Pierluigi Carlino on 08/06/22.
//

#ifndef DB_VULKANGEOMTRIC_H
#define DB_VULKANGEOMTRIC_H
#include "commonVulkanIncludes.h"
#include "VulkanModel.h"
#include "VulkanFramework.h"
#include "customTransf.hpp"

using namespace glm;

namespace Vulkan {

    class TexturedRender:public Render{
        float angle = 0.0;
        float speed = 0.1;


        TextureBuffer<stbi_uc *> *texture;


    protected:
        vector<glm::vec2> textCoords;
        Object *obj;



        void setVertexesAndIndicies(vector<glm::vec3> &vert,vector<uint32_t> &ind){
            obj->setVertex(vert);
            obj->setIndicies(ind);
            obj->setTextCoords(textCoords);
        }

        void createObject(){
            obj = this->getObject();
        }

        void createModel(){
            this->model = new Model();
            model->setDeviceRepository(this->deviceRepository);
            model->addObject(*obj);
            model->createBuffers();
        };


        virtual  Object* getObject(){
            return new Object();
        }

    protected:
        TexturedRender():Render(){}

        void init_texture(shared_ptr<VkPhyDevice> repo,shared_ptr<VkSwapChain> swap,
                       string textPath,
                       shared_ptr<UniformBuffer<UniformBufferObject>> gubo){
            Render::init(repo,swap,gubo);

            texture = new TextureBuffer<stbi_uc *>(textPath);
            texture->createBuffers(*swap);
            addDescriptorSet<stbi_uc *>(*texture);
        }
    public:





        virtual void update(GLFWwindow *window) override{

        }
    };
    class Triangle : public TexturedRender {
    public:
        Triangle(vec3 first,
                 vec3 second,
                 vec3 third,const string textPath,
                 Enviroment &env):TexturedRender()
        {
            init_texture(env.getRepository(),env.getSwapChain(),textPath,env.getGubo());
            vector<glm::vec3> app = vector<vec3>();
            app.push_back(first);
            app.push_back(second);
            app.push_back(third);

            vector<uint32_t> ind = {1,2,3};

            createObject();
            textCoords.push_back(glm::vec2(0.0,0.0));
            textCoords.push_back(glm::vec2(1,1));
            textCoords.push_back(glm::vec2(0.0,1));

            TexturedRender::setVertexesAndIndicies(app,ind);
            TexturedRender::createModel();

            createPipeline(*this->swapChain);

            env.addRender(this);
        }
    };

    class Sky: public TexturedRender{
    public:
        Sky(const string textPath,
                Enviroment &env):TexturedRender()
        {
            init_texture(env.getRepository(),env.getSwapChain(),textPath,env.getGubo());

            vector<glm::vec3> app = vector<vec3>();

            app.push_back({1,1,0});
            app.push_back({1,-1,0.0});
            app.push_back({-1,-1,0.0});
            app.push_back({-1,1,0.0});

            vector<uint32_t> ind = {0,1,2,2,0,3};

            createObject();
            textCoords.push_back({0.0,0.0});
            textCoords.push_back({1.0,0.0});
            textCoords.push_back({1.0,1.0});
            textCoords.push_back({0.0,1.0});

            TexturedRender::setVertexesAndIndicies(app,ind);
            TexturedRender::createModel();

            createPipeline(*this->swapChain);

            env.addRender(this);
        }

        virtual void update(GLFWwindow *window) override {}
    };
    class BlenderRender : public TexturedRender{
        string obj_path;

    protected:
        virtual Object* getObject() override{
            auto toRet =   new BlenderObj(this->obj_path);

            toRet->create();

            return toRet;
        };

    public:
        BlenderRender(string obj,string textPath,
                      Enviroment &env,bool finished = true):TexturedRender()
        {
            init_texture(env.getRepository(),env.getSwapChain(),textPath,env.getGubo());
            obj_path = obj;
            this->createObject();

            if(finished){
                TexturedRender::createModel();
                createPipeline(*this->swapChain);
            }
            env.addRender(this);
        }

    };
}
#endif //DB_VULKANGEOMTRIC_H
