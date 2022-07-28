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

        UniformBuffer<UniformBufferObject> *ubo;
        UniformBufferObject hostUbo{};

        TextureBuffer<stbi_uc *> *texture;


    protected:
        vector<glm::vec2> textCoords;


        void setVertexesAndIndicies(vector<glm::vec3> &vert,vector<int> &ind){
            Object *obj = new Object();
            obj->setVertex(vert);
            obj->setIndicies(ind);
            obj->setTextCoords(textCoords);


            this->model = new Model();
            model->setDeviceRepository(this->deviceRepository);
            model->addObject(*obj);
            model->createBuffers();



        }
    public:
        TexturedRender(shared_ptr<VkPhyDevice> repo,shared_ptr<VkSwapChain> swap,string textPath):Render(repo,swap){
            ubo =  new UniformBuffer<UniformBufferObject>();
            ubo->createBuffers(*swap,hostUbo);

            descrPool->addDescriptorSet<UniformBufferObject>(*ubo);

            texture = new TextureBuffer<stbi_uc *>(textPath);
            texture->createBuffers(*swap);
            descrPool->addDescriptorSet<stbi_uc *>(*texture);
        }

        virtual void update() override{
            auto rotation = TransfBuff::instance()
                    ->addZRotation(angle)
                    .doTransformations<glm::mat4>();

            hostUbo.mvpMat = rotation;
            hostUbo.mMat = rotation;
            hostUbo.nMat = rotation;
            ubo->updateData(hostUbo);
            angle += glm::pi<float>();

        }
    };
    class Triangle : public TexturedRender {
    public:
        Triangle(vec3 first,
                 vec3 second,
                 vec3 third,const string textPath,
                 Enviroment &env,
                 bool textured = false):TexturedRender(env.getRepository(),env.getSwapChain(),textPath)
        {
            vector<glm::vec3> app = vector<vec3>();
            app.push_back(first);
            app.push_back(second);
            app.push_back(third);

            vector<int> ind = {{1,2,3}};

            textCoords.push_back(glm::vec2(0.0,0.5));
            textCoords.push_back(glm::vec2(0.5,0.5));
            textCoords.push_back(glm::vec2(0.0,0.0));

            TexturedRender::setVertexesAndIndicies(app,ind);
            createPipeline(*this->swapChain);

            env.addRender(this);
        }
    };

}
#endif //DB_VULKANGEOMTRIC_H
