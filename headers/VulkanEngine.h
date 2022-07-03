//
// Created by Pierluigi Carlino on 02/07/22.
//

#ifndef DB_VULKANENGINE_H
#define DB_VULKANENGINE_H

#include "commonVulkanIncludes.h"
#include "VulkanAppConfiguration.h"
namespace Vulkan {
//default debug caller
    static VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

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



    protected:
    public:
        VkEngine() {
            errorStorer.insert(pair<VkResult, string>{VK_NOT_READY, "Not Ready"});
            errorStorer.insert(pair<VkResult, string>{VK_TIMEOUT, "Timeout"});
            errorStorer.insert(pair<VkResult, string>{VK_EVENT_SET, "Event Set"});
            errorStorer.insert(pair<VkResult, string>{VK_EVENT_RESET, "Event Reset"});
            errorStorer.insert(pair<VkResult, string>{VK_INCOMPLETE, "Incomplete"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_OUT_OF_HOST_MEMORY, "Out of Host Memory"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_OUT_OF_DEVICE_MEMORY, "Out of Device Memory"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_INITIALIZATION_FAILED, "Initialization Failed"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_DEVICE_LOST, "Device Lost"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_MEMORY_MAP_FAILED, "Memory Map Failed"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_LAYER_NOT_PRESENT, "Layer Not Present"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_EXTENSION_NOT_PRESENT, "Extension Not Present"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_FEATURE_NOT_PRESENT, "Feature Not Present"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_INCOMPATIBLE_DRIVER, "Incompatible Driver"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_TOO_MANY_OBJECTS, "Too Many Objects"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_FORMAT_NOT_SUPPORTED, "Format Not Supported"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_FRAGMENTED_POOL, "Fragmented Pool"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_SURFACE_LOST_KHR, "Surface Lost"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "Native Window in Use"});
            errorStorer.insert(pair<VkResult, string>{VK_SUBOPTIMAL_KHR, "Suboptimal"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_OUT_OF_DATE_KHR, "Error Out of Date"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "Incompatible Display"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_VALIDATION_FAILED_EXT, "Valuidation Failed"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_INVALID_SHADER_NV, "Invalid Shader"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_OUT_OF_POOL_MEMORY_KHR, "Out of Pool Memory"});
            errorStorer.insert(pair<VkResult, string>{VK_ERROR_INVALID_EXTERNAL_HANDLE, "Invalid External Handle"});

            this->setAppConfig(VkAppConfiguration());

            this->initWindow(12, 12);
            this->setInstance();
            this->createSurface();

        }

        VkEngine(int w, int h, VkAppConfiguration configuration) {
            width = w;
            heigth = h;

            this->setAppConfig(configuration);

            this->initWindow(w, h);
            this->setInstance();
            this->createSurface();
        }


        virtual void setAppConfig(VkAppConfiguration config) override;

        virtual void setInstance() override;

        virtual void createSurface() override;

        //window handlers
        virtual void initWindow(int w, int h) override;

        virtual void defaultWindowResizeHandler(GLFWwindow *window, int width, int height) override;

        virtual void setResizeWindowCallBack(void *newFunc) override;

        bool mainLoop() override;

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
            if (CreateDebugUtilsMessengerEXT(&createInfo) != VK_SUCCESS)
                Terminal::getInstance().print(
                        "failed to set up debug messenger!",
                        BackGroudColor::yellow,
                        ForeGroudColor::red);
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


    private:
    };
}

#endif //DB_VULKANENGINE_H
