//
// Created by Pierluigi Carlino on 02/07/22.
//

#ifndef DB_VULKANENGINE_H
#define DB_VULKANENGINE_H

#include "commonVulkanIncludes.h"

#include "VulkanAppConfiguration.h"
namespace Vulkan {
    static  bool debug = true;

//default debug caller
    static VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
        if(debug)
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }


    class VkEngine :
            public GraphicEngine<
                    VkInstance,
                    VkAppConfiguration,
                    VkResult,
                    GLFWwindow,
                    VkSurfaceKHR> {
    private:
        VkDebugUtilsMessengerEXT debugMessenger;
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};

        void (*debugCallback)();


        // Camera
        int width,heigth;
        glm::mat3 CamDir = glm::mat3(1.0f);
        glm::vec3 CamPos = glm::vec3(0.0f, 0.5f, 2.5f);
        glm::vec3 CamAng = glm::vec3(0.0f, 0.0f, 0.0f);


        std::chrono::duration<double, std::ratio<1,24>> frameRate = 33ms;

    protected:
    public:
        VkEngine() {
        }

        std::chrono::duration<double, std::ratio<1,24>> getFramerate(){return frameRate;}

        VkEngine(int w, int h, VkAppConfiguration configuration) {
            width = w;
            heigth = h;

            this->setAppConfig(configuration);

            this->initWindow(w, h);
            this->setInstance();
            this->createSurface();
        }

        template<typename Render>
        bool mainLoop(vector<Render> renders){
        if(!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            std::this_thread::sleep_for(frameRate);

            if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
                return false;
            }

            for(auto render:renders)
                render->update(window);

            return true;
        }

        return false;
    }




    virtual void setAppConfig(VkAppConfiguration config) override;

        virtual void setInstance() override;

        virtual void createSurface() override;

        //window handlers
        virtual void initWindow(int w, int h) override;

        virtual void defaultWindowResizeHandler(GLFWwindow *window, int width, int height) override;

        virtual void setResizeWindowCallBack(void *newFunc) override;


        virtual bool checkValidationLayerSupport() override;

        virtual void setupDebugMessenger(void (*callBack)() = NULL) override {
            if (!enableValidationLayers) return;

            if (callBack == NULL)
                debugCallback = reinterpret_cast<void (*)(void)>(defaultDebugCallback);
            createInfo.sType =
                    VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity =
                    //			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | // disabled in the tutorial
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(debugCallback);
            createInfo.pUserData = nullptr;

        }



        void setCamDir(glm::mat3 toSet){CamDir = toSet;};
        void setCamPos(glm::vec3 toSet){CamPos = toSet;};
        void setCamAng(glm::vec3 toSet){CamAng = toSet;};

        virtual void setupDebugCallback() override {
            setupDebugMessenger();
            VkResult  result = CreateDebugUtilsMessengerEXT(&createInfo);

            Terminal::getInstance().printOnCondition("to create debug callback",result!=VK_SUCCESS,result);

        }

        void updateUniformBuffer(uint32_t currentImage);

        void DestroyDebugUtilsMessengerEXT() {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
                    vkGetInstanceProcAddr(instance,
                                          "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr) {
                func(instance, debugMessenger, nullptr); //fyi lather you have to understande the allocator
            }
        }

        VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo) {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
                    vkGetInstanceProcAddr(instance,
                                          "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                return func(instance, pCreateInfo, nullptr,
                            &debugMessenger);//fyi lather you have to understande the allocator
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }


        //clean up logic
        virtual void cleanup() override;

        virtual std::vector<const char *> getRequiredInstanceExtensions() override;

        static VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);





    };
}

#endif //DB_VULKANENGINE_H
