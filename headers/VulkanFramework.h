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

#include "customTransf.hpp"

namespace Vulkan {

    class Camera{
    protected:
        // Camera
        glm::mat3 CamDir = glm::mat3(1.0f);
        glm::vec3 CamPos = glm::vec3(0.0f, 0.0f, 0.f);
    public:
        void setCamPos(const glm::vec3 &camPos) {
            CamPos = camPos;
        }

        void setCamAng(const glm::vec3 &camAng) {
            CamAng = camAng;
            CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f), CamAng.y, glm::vec3(0.0f, 1.0f, 0.0f))) *
                     glm::mat3(glm::rotate(glm::mat4(1.0f), CamAng.x, glm::vec3(1.0f, 0.0f, 0.0f))) *
                     glm::mat3(glm::rotate(glm::mat4(1.0f), CamAng.z, glm::vec3(0.0f, 0.0f, 1.0f)));
        }

    private:
        glm::vec3 CamAng = glm::vec3(0.0f, 0.0f, 0.0f);

        float rotSpeed = 0.01f;
        float translSpeed = 0.1f;
        bool default_nav = false;



    public:
        Camera(float rotSpeed,float transSpeed,bool default_nav = false){
            this->rotSpeed = rotSpeed;
            this->translSpeed = transSpeed;
            this->default_nav = default_nav;

        }

        glm::vec3 getCamPos(){return CamPos;}
        glm::vec3 getCamAngle(){return CamPos;}
        glm::mat4 getCamDim(){return CamDir;}

        virtual void update(GLFWwindow *window) {
            if (default_nav) {
                if (glfwGetKey(window, GLFW_KEY_LEFT)) {
                    CamAng.y += rotSpeed;
                }
                if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
                    CamAng.y -= rotSpeed;
                }
                if (glfwGetKey(window, GLFW_KEY_UP)) {
                    CamAng.x += rotSpeed;
                }
                if (glfwGetKey(window, GLFW_KEY_DOWN)) {
                    CamAng.x -= rotSpeed;
                }
                if (glfwGetKey(window, GLFW_KEY_Q)) {
                    CamAng.z -= rotSpeed;
                }
                if (glfwGetKey(window, GLFW_KEY_E)) {
                    CamAng.z += rotSpeed;
                }


                CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f), CamAng.y, glm::vec3(0.0f, 1.0f, 0.0f))) *
                         glm::mat3(glm::rotate(glm::mat4(1.0f), CamAng.x, glm::vec3(1.0f, 0.0f, 0.0f))) *
                         glm::mat3(glm::rotate(glm::mat4(1.0f), CamAng.z, glm::vec3(0.0f, 0.0f, 1.0f)));


                if (glfwGetKey(window, GLFW_KEY_A)) {
                    CamPos -= translSpeed * glm::vec3(glm::rotate(glm::mat4(1.0f), CamAng.y,
                                                                  glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1, 0, 0, 1));
                }
                if (glfwGetKey(window, GLFW_KEY_D)) {
                    CamPos += translSpeed * glm::vec3(glm::rotate(glm::mat4(1.0f), CamAng.y,
                                                                  glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1, 0, 0, 1));
                }
                if (glfwGetKey(window, GLFW_KEY_S)) {
                    CamPos += translSpeed * glm::vec3(glm::rotate(glm::mat4(1.0f), CamAng.y,
                                                                  glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0, 0, 1, 1));
                }
                if (glfwGetKey(window, GLFW_KEY_W)) {
                    CamPos -= translSpeed * glm::vec3(glm::rotate(glm::mat4(1.0f), CamAng.y,
                                                                  glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0, 0, 1, 1));
                }
                if (glfwGetKey(window, GLFW_KEY_F)) {
                    CamPos -= translSpeed * glm::vec3(0, 1, 0);
                }
                if (glfwGetKey(window, GLFW_KEY_R)) {
                    CamPos += translSpeed * glm::vec3(0, 1, 0);
                }

            }
        }

        virtual glm::mat4 getProj(float n,float f,float a){
            return ProjectBuffer::instance(1.0f,n,f)->perspectiveProj(glm::pi<float>()/6,a);
        }

        virtual glm::mat4 getCameraMat(){
            return glm::translate(glm::transpose(glm::mat4(CamDir)), -CamPos);
        }
    };
    class Enviroment{

    protected:



        int choosen;
        int currentFrame = 0;

        bool bufferResized = false;

        std::map<int,vector<int>> objCameraAttached;


        shared_ptr<UniformBuffer<UniformBufferObject>> gubo;
        UniformBufferObject hostGubo;
        float n = 1.0,f = 100,a = 0.0;
        vector<Camera*> cameras;
        int currentCamera = 0;

        vector<Vulkan::Render*> renders;
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


        int choosenDeviceSwapChain = -1;
        string name;

        int w, h;

        void SetUp() ;
        void ChooseADevice(vector<VkPhysicalDeviceType> type= {
        {VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU}});
        void createUniformBuffer();
        void setAndCreateSwapChain();
        void createSyncObjects();
        void drawFrame();
        void destroy(bool all = true);

        void createCommandPool() {
            auto queueFamilyIndices =  deviceRepository->getUniqueIndicies<vector<uint32_t>>(
                    choosen
            );

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.queueFamilyIndex = queueFamilyIndices[0];
            poolInfo.flags = 0; // Optional

            VkResult result = vkCreateCommandPool(deviceRepository->getLogicalDevice(choosen), &poolInfo, nullptr, &commandPool);
            Terminal::getInstance().printOnCondition("to create command pool",result!=VK_SUCCESS,result);

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


            Terminal::getInstance().printOnCondition("to create command buffers",result!=VK_SUCCESS,result);

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
                    render->createCommandBuffers(commandBuffers[i],i);

                vkCmdEndRenderPass(commandBuffers[i]);

                Terminal::getInstance().printOnCondition("to create record command buffers "+i,result!=VK_SUCCESS,result);

                result = vkEndCommandBuffer(commandBuffers[i]);
                Terminal::getInstance().printOnCondition("to end command buffer "+i,result!=VK_SUCCESS,result);


            }
        };




    public:
        Enviroment(string name,int w = 640, int h = 480):
            name(name),
            w(w),
            h(h){

            a = w/h;

            SetUp();
            ChooseADevice();
            setAndCreateSwapChain();
            createSyncObjects();
            createCommandPool();

            gubo = std::make_shared<UniformBuffer<UniformBufferObject>>(*(new UniformBuffer<UniformBufferObject>()));
            hostGubo.mvpMat = glm::mat4(1.0);
            hostGubo.mMat = glm::mat4(1.0);
            hostGubo.nMat = glm::mat4(1.0);

            gubo->createBuffers(*swapChain, hostGubo);
            glfwSetWindowUserPointer(this->engine->getWindow(), this);
            glfwSetFramebufferSizeCallback(this->engine->getWindow(), framebufferResizeCallback);

        }


    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Enviroment*>
        (glfwGetWindowUserPointer(window));
        app->bufferResized = true;
    }

    void setAndRecreateSwapChain(){
        auto dev = this->deviceRepository->getLogicalDevice(*this->deviceRepository->getPerfectCriteriaMatcher().begin());
        vkDeviceWaitIdle(dev);
            auto window = this->engine->getWindow();
            glfwGetFramebufferSize(window, &w, &h);

            while (w == 0 || h == 0) {
                glfwGetFramebufferSize(window, &w, &h);
                glfwWaitEvents();
            }
            swapChain->destroy();
        setAndCreateSwapChain();
        this->destroy(false);

        createSyncObjects();
        createCommandPool();
        this->createRender();


    }

        float getN(){return this->n;}
        float getF(){return this->f;}

        shared_ptr<VkPhyDevice> getRepository(){
            return deviceRepository;
        }

        std::shared_ptr<VkSwapChain> getSwapChain(){return swapChain;}

        void createRender(){
            startRenderPass();
        }
        void addRender(Render* r){
             this->renders.push_back(r);
        }
        void mainLoop() {
            hostGubo.mvpMat = cameras[currentCamera]->getProj(-n,-f,a);
            hostGubo.mMat = cameras[currentCamera]->getCameraMat();
            hostGubo.nMat = glm::mat4 (1.0);
            gubo->updateData(hostGubo);
            for(auto rend:renders){
                rend->setF(f);
                rend->setN(n);
            }


            while (deviceRepository->getEngine().mainLoop(renders)) {
                drawFrame();

                this->update(deviceRepository->getEngine().getWindow());
            }

            vkDeviceWaitIdle(swapChain->getChoosenDevice());
            this->destroy();
        }

        std::shared_ptr<UniformBuffer<UniformBufferObject>> getGubo(){return gubo;}

        virtual void update(GLFWwindow *window){
            if (glfwGetKey(window, GLFW_KEY_SPACE)) {
                currentCamera = ++currentCamera%cameras.size();
                hostGubo.mvpMat = cameras[currentCamera]->getProj(-n,-f,a);
                std::this_thread::sleep_for(10*engine->getFramerate());

                hostGubo.mMat = cameras[currentCamera]->getCameraMat();
                hostGubo.nMat = glm::mat4 (1.0);

                gubo->updateData(hostGubo);
            }


        }

         void addCamera(Camera *camera){this->cameras.push_back(camera);}
    };
}
#endif //ASS0_VULKANFRAMEWORK_H
