//
//
// Created by Pierluigi Carlino on 06/03/22.
//

#ifndef ASS0_VULKANFRAMEWORK_H
#define ASS0_VULKANFRAMEWORK_H

#include "commonVulkanIncludes.h"
#include "VulkanAppConfiguration.h"
#include "VulkanEngine.h"
#include "VulkanDevices.h"
#include "VulkanBuffers.h"
#include "VulkanShaders.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"

namespace Vulkan {

    class Object{
        vector<glm::vec3> vertex;
        vector<int> indicies;
    public:
        void setVertex(vector<glm::vec3> vertex);
        void setIndicies(vector<int> indicies);


    public:
        const vector<glm::vec3> &getVertex() const;
        const vector<int> &getIndicies() const;
    };

    class Model{
        vector<Object> objects;
        vector<Vertex> vertex;
        vector<int> indicies;

        VkVertexBuffer<Vertex> *vertexBuffer;
        VkIndexBuffer<int> *indBuffer;

        int choosenDevice;
        std::shared_ptr<VkPhyDevice> deviceRepository;


    public:
        ~Model(){
            delete vertexBuffer;
            delete indBuffer;

        }
        void createBuffers(){
            this->copyData();

            vertexBuffer = new VkVertexBuffer<Vertex>(vertex.size()*3,this->choosenDevice);
            indBuffer = new VkIndexBuffer<int>(indicies.size()*3,this->choosenDevice);

            vertexBuffer->setPhysicalDevice(deviceRepository);
            indBuffer->setPhysicalDevice(deviceRepository);



            vertexBuffer->setData(vertex);
            indBuffer->setData(indicies);

            vertexBuffer->create();
            indBuffer->create();
        }

    protected:
        int getVertexSize(){
            uint32_t  toRet =0;
            for(auto obj:objects)
                toRet += obj.getVertex().size();
            return toRet;
        }

        int getIndSize(){
            uint32_t  toRet =0;
            for(auto obj:objects)
                toRet += obj.getIndicies().size();
            return toRet;
        }

        void copyData(){
            vertex.resize(getVertexSize());
            indicies.resize(getIndSize());

            for(auto toAdd:objects){
                int i=0;
                for(auto vert:toAdd.getVertex()){
                    vertex[i++].pos = vert;
                }

                i=0;

                for(auto ind:toAdd.getIndicies()){
                    indicies[i++] = ind;
                }

            }
        }



    public:


        void setDeviceRepository(std::shared_ptr<VkPhyDevice> phydevice ){
            this->deviceRepository = phydevice;
        }
        void addObject(Object& object){
            objects.push_back(object);
        }

        void create(){
            copyData();
        }

        VkBuffer getVertexBuffers(){

           return vertexBuffer->getBuffer();
        };

        VkBuffer getIndexBuffers(){
            return indBuffer->getBuffer();
        };

        auto getIndicies(){return indicies;};
        auto getVerticies(){return vertex;};
    };
    class Render{
        std::shared_ptr<VkPhyDevice> deviceRepository;
        VkCommandPool commandPool;
        VkPipeLine *pipeLine;
        VkSwapChain *swapChain;
        std::vector<VkCommandBuffer> commandBuffers;
        Model *model;

        //Synchronization objects
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        int choosen;

        const int MAX_FRAMES_IN_FLIGHT = 2;
        int currentFrame = 0;

    public:
        Render(std::shared_ptr<VkPhyDevice>deviceRepo,VkPipeLine *pipeLine,VkSwapChain *swapChain,Model* model){
            this->pipeLine = pipeLine;

            deviceRepository = deviceRepo;
            choosen = *deviceRepo->getPerfectCriteriaMatcher().begin();

            this->swapChain = swapChain;
            this->model = model;
        }

        void createCommandPool() {
            auto queueFamilyIndices =  deviceRepository->getUniqueIndicies<vector<uint32_t>>(
                        choosen
                    );

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.queueFamilyIndex = queueFamilyIndices[0];
            poolInfo.flags = 0; // Optional

            VkResult result = vkCreateCommandPool(deviceRepository->getLogicalDevice(choosen), &poolInfo, nullptr, &commandPool);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create command pool!");
            }
        }



        void createCommandBuffers() {
            // Lesson 13
            auto frameBuffers = pipeLine->getSwapChainFramebuffers();

            commandBuffers.resize(frameBuffers.size());

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = commandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

            VkResult result = vkAllocateCommandBuffers(swapChain->getChoosenDevice(), &allocInfo,
                                                       commandBuffers.data());
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to allocate command buffers!");
            }

            // Lesson 22.5 --- Draw calls
            // This is where the commands that actually draw something on screen are!
            for (size_t i = 0; i < commandBuffers.size(); i++) {
                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags = 0; // Optional
                beginInfo.pInheritanceInfo = nullptr; // Optional

                if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) !=
                    VK_SUCCESS) {
                    throw std::runtime_error("failed to begin recording command buffer!");
                }

                VkRenderPassBeginInfo renderPassInfo{};
                VkClearValue clearColor = {1.0f, 0.0f, 0.0f, 1.0f};
                clearColor.color =  {1.0f, 0.0f, 0.0f, 1.0f};
                renderPassInfo.clearValueCount = 1;
                renderPassInfo.pClearValues = &clearColor;

                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassInfo.renderPass = pipeLine->getRenderPass();
                renderPassInfo.framebuffer = pipeLine->getSwapChainFramebuffers()[i];
                renderPassInfo.renderArea.offset = {0, 0};
                renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

                std::array<VkClearValue, 2> clearValues{};
                clearValues[0].color = {1.0f, 0.0f, 0.0f, 1.0f};
                clearValues[1].depthStencil = {1.0f, 0};

                renderPassInfo.clearValueCount =
                        static_cast<uint32_t>(clearValues.size());;
                renderPassInfo.pClearValues = clearValues.data();

                vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                                     VK_SUBPASS_CONTENTS_INLINE);
                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  pipeLine->getGraphicsPipeline());

                auto vBuff = model->getVertexBuffers();
                auto iBuff = model->getIndexBuffers();

                VkBuffer vertexBuffers[] = {vBuff};
                VkBuffer indexBuffer = iBuff;

                VkDeviceSize offsets[] = {0};

                 vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);



                //vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(3), 1, 0, 0);
                vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0,
                                     VK_INDEX_TYPE_UINT16);
                 //todo decide how to handle
                 /**vkCmdBindDescriptorSets(commandBuffers[i],
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipeLine->getPipelineLayout(), 0, 1, &descriptorSets[i],
                                        0, nullptr);


                **/
                vkCmdDrawIndexed(commandBuffers[i],
                                 static_cast<uint32_t>(model->getIndicies().size()), 1, 0, 0, 0);

                vkCmdEndRenderPass(commandBuffers[i]);
                if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                    throw std::runtime_error("failed to record command buffer!");
                }
            }
        }

        void createSyncObjects() {

            imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
            imagesInFlight.resize(swapChain->getSwapChainImages().size(), VK_NULL_HANDLE);

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                VkResult result1 = vkCreateSemaphore(swapChain->getChoosenDevice(), &semaphoreInfo, nullptr,
                                                     &imageAvailableSemaphores[i]);
                VkResult result2 = vkCreateSemaphore(swapChain->getChoosenDevice(), &semaphoreInfo, nullptr,
                                                     &renderFinishedSemaphores[i]);
                VkResult result3 = vkCreateFence(swapChain->getChoosenDevice(), &fenceInfo, nullptr,
                                                 &inFlightFences[i]);
                if (result1 != VK_SUCCESS ||
                    result2 != VK_SUCCESS ||
                    result3 != VK_SUCCESS) {
                    Terminal::getInstance().printnl(result1);
                    Terminal::getInstance().printnl(result2);
                    Terminal::getInstance().printnl(result3);
                    throw std::runtime_error("failed to create synchronization objects for a frame!!");
                }
            }
        }


        void drawFrame() {
            vkWaitForFences(swapChain->getChoosenDevice(), 1, &inFlightFences[currentFrame],
                            VK_TRUE, UINT64_MAX);

            uint32_t imageIndex;

            VkResult result = vkAcquireNextImageKHR(swapChain->getChoosenDevice(), swapChain->getSwapChain(), UINT64_MAX,
                                                    imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                //todo decide how to put
                //recreateSwapChain();
                return;
            } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                throw std::runtime_error("failed to acquire swap chain image!");
            }

            if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
                vkWaitForFences(swapChain->getChoosenDevice(), 1, &imagesInFlight[imageIndex],
                                VK_TRUE, UINT64_MAX);
            }
            imagesInFlight[imageIndex] = inFlightFences[currentFrame];

            //todo decide how to put
            //updateUniformBuffer(imageIndex);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
            VkPipelineStageFlags waitStages[] =
                    {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
            VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            vkResetFences(swapChain->getChoosenDevice(), 1, &inFlightFences[currentFrame]);

            if (vkQueueSubmit(deviceRepository->graphicsQueue, 1, &submitInfo,
                              inFlightFences[currentFrame]) != VK_SUCCESS) {
                throw std::runtime_error("failed to submit draw command buffer!");
            }

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;


            VkSwapchainKHR swapChains[] = {swapChain->getSwapChain()};
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr; // Optional

            result = vkQueuePresentKHR(deviceRepository->presentQueue, &presentInfo);

            /** todo decide how to handle
            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
                framebufferResized) {
                framebufferResized = false;
                recreateSwapChain();
            } else **/if (result != VK_SUCCESS) {
                throw std::runtime_error("failed to present swap chain image!");
            }

            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        }


        void mainLoop() {
            while (deviceRepository->getEngine().mainLoop()) {
                drawFrame();
            }

            vkDeviceWaitIdle(swapChain->getChoosenDevice());
        }
    };

}
#endif //ASS0_VULKANFRAMEWORK_H
