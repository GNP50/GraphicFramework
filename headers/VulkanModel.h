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
                auto textCoords = toAdd.getTextCoords();
                for (auto vert: toAdd.getVertex()) {

                    vertex[i].texCoord = textCoords.size() > 0 ? textCoords[i%textCoords.size()] : glm::vec2(0.0f);
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
    protected:
        std::shared_ptr<VkPhyDevice> deviceRepository;
        std::shared_ptr<VkSwapChain> swapChain;
        Model *model;

        DescriptorPool *descrPool;

        string vertexPath = "shaders/Vertex.spv";
        string fragmPath = "shaders/Fragment.spv";

        //vulkan objects
        VkPipeLine *pipeline;
        VkFragmentShader *fragShader;
        VkVertexShader *vertexShader;

    public:
        VkFragmentShader *getFragShader() const;
        VkVertexShader *getVertexShader() const;
        virtual void update(GLFWwindow *window) {

        };


        void recreate(shared_ptr<VkSwapChain> swap ){
            swapChain = swap;

            descrPool->destroy();
            delete descrPool;
            descrPool = new DescriptorPool(swapChain,deviceRepository);
        }

    protected:


        Render(shared_ptr<VkPhyDevice> repo,shared_ptr<VkSwapChain> swap):
            deviceRepository(repo),
            swapChain(swap),
            descrPool(new DescriptorPool(swapChain,deviceRepository)){

        }




    public:
        Render(std::shared_ptr<VkPhyDevice>deviceRepo,VkPipeLine *pipeLine,std::shared_ptr<VkSwapChain> swapChain,Model* model): Render(deviceRepository,swapChain){
            this->pipeline = pipeLine;
            this->model = model;
        }

        void createBuffer(VkPhyDevice &deviceRepo){
            this->model->setDeviceRepository(deviceRepository);
            this->model->createBuffers();
        }

        virtual void createPipeline(VkSwapChain &swapChain);


        void createCommandBuffers(VkCommandBuffer commandBuffer,int i);


        void destroy(){
            pipeline->destroy(*swapChain);
            descrPool->destroy();
            delete this->model;
        }

    };
};


#endif //DB_VULKANMODEL_H
