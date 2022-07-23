//
// Created by Pierluigi Carlino on 08/06/22.
//

#ifndef DB_VULKANGEOMTRIC_H
#define DB_VULKANGEOMTRIC_H
#include "commonVulkanIncludes.h"
#include "VulkanModel.h"
#include "VulkanFramework.h"

using namespace glm;

namespace Vulkan {

    class NonTexturedRender:public Render{
        UniformBuffer<UniformBufferObject> *ubo;
        UniformBufferObject hostUbo = {
                glm::mat4 (1.0f),
                glm::mat4 (1.0f),
                glm::mat4 (1.0f)
        };

    protected:


        void setVertexesAndIndicies(vector<glm::vec3> &vert,vector<int> &ind){
            Object *obj = new Object();
            obj->setVertex(vert);
            obj->setIndicies(ind);

            this->model = new Model();
            model->setDeviceRepository(this->deviceRepository);
            model->addObject(*obj);
            model->createBuffers();

        }
    public:
        NonTexturedRender(shared_ptr<VkPhyDevice> repo,shared_ptr<VkSwapChain> swap):Render(repo,swap){
            ubo =  new UniformBuffer<UniformBufferObject>();

            descrPool->addDescriptorSet<UniformBufferObject>(*ubo);

        }
    };
    class Triangle : public NonTexturedRender {
    public:
        Triangle(vec3 first,
                 vec3 second,
                 vec3 third,
                 Enviroment &env,
                 bool textured = false):NonTexturedRender(env.getRepository(),env.getSwapChain())
        {
            vector<glm::vec3> app = vector<vec3>();
            app.push_back(first);
            app.push_back(second);
            app.push_back(third);

            vector<int> ind = {{1,2,3}};

            NonTexturedRender::setVertexesAndIndicies(app,ind);
            createPipeline(*this->swapChain);

            env.addRender(*this);
        }
    };

}
#endif //DB_VULKANGEOMTRIC_H
