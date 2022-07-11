//
// Created by Pierluigi Carlino on 09/07/22.
//

#ifndef DB_VULKANMODEL_H
#define DB_VULKANMODEL_H
#include "commonVulkanIncludes.h"
#include "VkObject.h"
#include "VulkanBuffers.h"
#include "VulkanShaders.h"
#include "VulkanDevices.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"

namespace Vulkan {
    class Model {
        vector<Object> objects;
        vector<Vertex> vertex;
        vector<int> indicies;

        VkVertexBuffer <Vertex> *vertexBuffer;
        VkIndexBuffer<int> *indBuffer;

        int choosenDevice;
        std::shared_ptr<VkPhyDevice> deviceRepository;


    public:
        ~Model() {
            delete vertexBuffer;
            delete indBuffer;

        }

        void createBuffers() {
            this->copyData();

            vertexBuffer = new VkVertexBuffer<Vertex>(vertex.size() * 3, this->choosenDevice);
            indBuffer = new VkIndexBuffer<int>(indicies.size() * 3, this->choosenDevice);

            vertexBuffer->setPhysicalDevice(deviceRepository);
            indBuffer->setPhysicalDevice(deviceRepository);


            vertexBuffer->setData(vertex);
            indBuffer->setData(indicies);

            vertexBuffer->create();
            indBuffer->create();
        }

    protected:
        int getVertexSize() {
            uint32_t toRet = 0;
            for (auto obj: objects)
                toRet += obj.getVertex().size();
            return toRet;
        }

        int getIndSize() {
            uint32_t toRet = 0;
            for (auto obj: objects)
                toRet += obj.getIndicies().size();
            return toRet;
        }

        void copyData() {
            vertex.resize(getVertexSize());
            indicies.resize(getIndSize());

            for (auto toAdd: objects) {
                int i = 0;
                for (auto vert: toAdd.getVertex()) {
                    vertex[i++].pos = vert;
                }

                i = 0;

                for (auto ind: toAdd.getIndicies()) {
                    indicies[i++] = ind;
                }

            }
        }


    public:


        void setDeviceRepository(std::shared_ptr<VkPhyDevice> phydevice) {
            this->deviceRepository = phydevice;
        }

        void addObject(Object &object) {
            objects.push_back(object);
        }

        void create() {
            copyData();
        }

        VkBuffer getVertexBuffers() {

            return vertexBuffer->getBuffer();
        };

        VkBuffer getIndexBuffers() {
            return indBuffer->getBuffer();
        };

        auto getIndicies() { return indicies; };

        auto getVerticies() { return vertex; };
    };
    class Render{
        std::shared_ptr<VkPhyDevice> deviceRepository;
        VkPipeLine *pipeLine;
        VkSwapChain *swapChain;
        Model *model;





        //vulkan objects
        VkPipeLine *pipeline;
        VkFragmentShader *fragShader;
        VkVertexShader *vertexShader;


    public:
        Render(std::shared_ptr<VkPhyDevice>deviceRepo,VkPipeLine *pipeLine,VkSwapChain *swapChain,Model* model){
            this->pipeLine = pipeLine;

            deviceRepository = deviceRepo;

            this->swapChain = swapChain;
            this->model = model;
        }

        void createBuffer(VkPhyDevice &deviceRepo){
            this->model->setDeviceRepository(deviceRepository);
            this->model->createBuffers();
        }

        virtual void createPipeline(VkSwapChain &swapChain);

        virtual void createSyncObjects();
        virtual void drawFrame(VkCommandBuffer commandBuffer);
        void createCommandBuffers(VkCommandBuffer commandBuffer );



    };
};


#endif //DB_VULKANMODEL_H
