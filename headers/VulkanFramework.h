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
#include "VkObject.h"
#include "VulkanModel.h"

namespace Vulkan {

    class Enviroment{

    protected:
        const int MAX_FRAMES_IN_FLIGHT = 2;
        int choosen;
        int currentFrame = 0;

        vector<Vulkan::Render> renders;
        std::shared_ptr<VkPhyDevice> deviceRepository;
        std::shared_ptr<VkSwapChain> swapChain;

        //Synchronization objects
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;


        VkEngine *engine;
        VkAppConfiguration configuration;

        int choosenDevice = -1;
        int choosenDeviceSwapChain = -1;
        string name;

        const int w, h;

        void SetUp() ;
        void ChooseADevice(vector<VkPhysicalDeviceType> type= {
        {VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU}});
        void createUniformBuffer();
        void setAndCreateSwapChain();
        void createSyncObjects();
        void drawFrame();

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



        void startRenderPass(){
            // Lesson 13
            auto frameBuffers = swapChain->getFramwBuffers();

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
                VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
                clearColor.color = {0.0f, 0.0f, 0.0f, 1.0f};
                renderPassInfo.clearValueCount = 1;
                renderPassInfo.pClearValues = &clearColor;

                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassInfo.renderPass = swapChain->getRenderPass();
                renderPassInfo.framebuffer = swapChain->getFramwBuffers()[i];
                renderPassInfo.renderArea.offset = {0, 0};
                renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

                std::array<VkClearValue, 2> clearValues{};
                clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
                clearValues[1].depthStencil = {1.0f, 0};

                renderPassInfo.clearValueCount =
                        static_cast<uint32_t>(clearValues.size());;
                renderPassInfo.pClearValues = clearValues.data();

                vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                                     VK_SUBPASS_CONTENTS_INLINE);

                for(auto render:this->renders)
                    render.createCommandBuffers(commandBuffers[i]);

                vkCmdEndRenderPass(commandBuffers[i]);
                if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                    throw std::runtime_error("failed to record command buffer!");
                }
            }
        };




    public:
        Enviroment(string name,int w = 640, int h = 480):
            name(name),
            w(w),
            h(h){


            SetUp();
            ChooseADevice();
            setAndCreateSwapChain();
            createSyncObjects();
            createCommandPool();
        }


        shared_ptr<VkPhyDevice> getRepository(){
            return deviceRepository;
        }

        std::shared_ptr<VkSwapChain> getSwapChain(){return swapChain;}

        void createRender(){
            startRenderPass();
        }
        void addRender(Render &r){
             this->renders.push_back(r);
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
