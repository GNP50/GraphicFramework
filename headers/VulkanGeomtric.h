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
        vec3 ds = vec3 (0.0,0.0,0.0);

        UniformBuffer<UniformBufferObject> *ubo;
        UniformBufferObject hostUbo{};
        Object *obj;
        TextureBuffer<stbi_uc *> *texture;


    protected:
        vector<glm::vec2> textCoords;


        void setVertexesAndIndicies(vector<glm::vec3> &vert,vector<int> &ind){
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
    public:
        TexturedRender(shared_ptr<VkPhyDevice> repo,shared_ptr<VkSwapChain> swap,string textPath):Render(repo,swap){
            ubo =  new UniformBuffer<UniformBufferObject>();
            ubo->createBuffers(*swap,hostUbo);

            descrPool->addDescriptorSet<UniformBufferObject>(*ubo);

            texture = new TextureBuffer<stbi_uc *>(textPath);
            texture->createBuffers(*swap);
            descrPool->addDescriptorSet<stbi_uc *>(*texture);

            hostUbo.mvpMat = ProjectBuffer::instance(1.0,-1.0,10.0f)->
                    cabinetProj(this->a);

        }

        void recreate(shared_ptr<VkSwapChain> swap ){
            Render::recreate(swap);

        }


        virtual void update(GLFWwindow *window) override{

            if (glfwGetKey(window, GLFW_KEY_UP)) {
                ds.z +=speed;
            }else if (glfwGetKey(window, GLFW_KEY_DOWN)){
                ds.z -=speed;
            }else if (glfwGetKey(window, GLFW_KEY_LEFT)){
                ds.x +=speed;
            }else if (glfwGetKey(window, GLFW_KEY_RIGHT)){
                ds.x -=speed;
            }else if (glfwGetKey(window, GLFW_KEY_Q)){
                angle -=speed;
            }else if (glfwGetKey(window, GLFW_KEY_W)){
                angle +=3.0f;
            }


            mat4 m = TransfBuff::instance()
                    ->addTranslation(ds)
                    ->addZRotation(angle)
                    .doTransformations<glm::mat4>();

            hostUbo.mMat = m;
            hostUbo.nMat = m;

            ubo->updateData(hostUbo);
        }
    };
    class Triangle : public TexturedRender {
    public:
        Triangle(vec3 first,
                 vec3 second,
                 vec3 third,const string textPath,
                 Enviroment &env):TexturedRender(env.getRepository(),env.getSwapChain(),textPath)
        {
            vector<glm::vec3> app = vector<vec3>();
            app.push_back(first);
            app.push_back(second);
            app.push_back(third);

            vector<int> ind = {{1,2,3}};

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
                      Enviroment &env):TexturedRender(env.getRepository(),env.getSwapChain(),textPath)
        {
            obj_path = obj;
            this->createObject();

            TexturedRender::createModel();
            createPipeline(*this->swapChain);
            env.addRender(this);
        }

    };
}
#endif //DB_VULKANGEOMTRIC_H
