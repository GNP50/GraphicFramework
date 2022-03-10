//
// Created by Pierluigi Carlino on 06/03/22.
//

#ifndef ASS0_VULKANFRAMEWORK_H
#define ASS0_VULKANFRAMEWORK_H
#include<GraphicsFramework.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>


//default debug caller
static VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

class VkAppConfiguration:public AppConfiguration<VkApplicationInfo,VkStructureType>{
public:

    virtual void setAppMajor(int appMajor) override;
    virtual void setAppMinor(int appMinor) override;
    virtual void setAppPatch(int appPatch) override;
    virtual void setEngineMajor(int engineMajor) override;
    virtual void setEngineMinor(int engineMinor) override;
    virtual void setEnginePatch(int enginePatch) override;

    virtual void setAppName(string appName) override;
    virtual void setEngineName(string engineName) override;
    virtual void setApiVersion(int apiVersion) override;
    virtual void setStructType(VkStructureType structureType) override;
    virtual void setRequiredExtension() override;

    virtual VkApplicationInfo& returnObj(){


        appRappresentation.sType = this->structureType;
        appRappresentation.pApplicationName = this->appName.c_str();
        appRappresentation.applicationVersion = VK_MAKE_VERSION(
                this->appMajor,
                this->appMinor,
                this->appPatch);


        appRappresentation.pEngineName = this->engineName.c_str();
        appRappresentation.engineVersion = VK_MAKE_VERSION(
                this->engineMajor,
                this->engineMinor,
                this->enginePatch);

        appRappresentation.apiVersion = this->apiVersion;
        return appRappresentation;
    };
};
class VkEngine:
        public GraphicEngine<
                VkInstance,
                VkAppConfiguration,
                VkResult,
                GLFWwindow,
                VkSurfaceKHR>{
private:
    VkDebugUtilsMessengerEXT debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    void (*debugCallback)();


protected:
public:
    VkEngine(){
        errorStorer.insert(pair<VkResult,string>{ VK_NOT_READY, "Not Ready" });
        errorStorer.insert(pair<VkResult,string>{ VK_TIMEOUT, "Timeout" });
        errorStorer.insert(pair<VkResult,string>{ VK_EVENT_SET, "Event Set" });
        errorStorer.insert(pair<VkResult,string>{ VK_EVENT_RESET, "Event Reset" });
        errorStorer.insert(pair<VkResult,string>{ VK_INCOMPLETE, "Incomplete" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_OUT_OF_HOST_MEMORY, "Out of Host Memory" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_OUT_OF_DEVICE_MEMORY, "Out of Device Memory" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_INITIALIZATION_FAILED, "Initialization Failed" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_DEVICE_LOST, "Device Lost" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_MEMORY_MAP_FAILED, "Memory Map Failed" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_LAYER_NOT_PRESENT, "Layer Not Present" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_EXTENSION_NOT_PRESENT, "Extension Not Present" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_FEATURE_NOT_PRESENT, "Feature Not Present" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_INCOMPATIBLE_DRIVER, "Incompatible Driver" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_TOO_MANY_OBJECTS, "Too Many Objects" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_FORMAT_NOT_SUPPORTED, "Format Not Supported" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_FRAGMENTED_POOL, "Fragmented Pool" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_SURFACE_LOST_KHR, "Surface Lost" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "Native Window in Use" });
        errorStorer.insert(pair<VkResult,string>{ VK_SUBOPTIMAL_KHR, "Suboptimal" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_OUT_OF_DATE_KHR, "Error Out of Date" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "Incompatible Display" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_VALIDATION_FAILED_EXT, "Valuidation Failed" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_INVALID_SHADER_NV, "Invalid Shader" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_OUT_OF_POOL_MEMORY_KHR, "Out of Pool Memory" });
        errorStorer.insert(pair<VkResult,string>{ VK_ERROR_INVALID_EXTERNAL_HANDLE, "Invalid External Handle" });
    }
    virtual void setAppConfig (VkAppConfiguration config);
    virtual void setInstance();

    //window handlers
    virtual void initWindow(int w,int h) override;
    virtual void defaultWindowResizeHandler(GLFWwindow* window, int width, int height) override;
    virtual void setResizeWindowCallBack(void* newFunc) override;

    virtual bool checkValidationLayerSupport() override;

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

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

        if (CreateDebugUtilsMessengerEXT(&createInfo) != VK_SUCCESS)
            Terminal::getInstance().print(
                    "failed to set up debug messenger!",
                    BackGroudColor::yellow,
                    ForeGroudColor::red);
    }

    void DestroyDebugUtilsMessengerEXT() {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(instance,
                                      "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, nullptr); //fyi lather you have to understande the allocator
        }
    }

    VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(instance,
                                      "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, nullptr, &debugMessenger);//fyi lather you have to understande the allocator
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }





private:
};



struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


class VkPhyDevice:public PhysicalDevice<
        VkPhysicalDevice,
        VkInstance,
        VkSurfaceKHR,
        SwapChainSupportDetails,
        VkPhysicalDeviceFeatures,
        VkExtensionProperties
        >{
protected:
    virtual bool isDevicesSuitable(vector<VkPhysicalDevice> devices) override;
    virtual bool isDeviceSuitable(vector<VkPhysicalDevice> devices) override;
    virtual bool isFeatureSupported(int offset) override;
    virtual bool isExtensionSupported(int offset,vector<VkExtensionProperties> requiredExtensions) override;

    virtual VkPhysicalDeviceFeatures getSupportedExtensions(int offset) override;
    virtual bool isSwapChainAdeguate(int offset,VkSurfaceKHR surface) override;

    virtual void initDevices(VkInstance instance) override;
    virtual SwapChainSupportDetails queryChainSupport(VkPhysicalDevice device,VkSurfaceKHR surface) override;

    template<typename Criteria>
    void getFirstSuitable(Criteria criteria);

    virtual QueueFamilyIndices  findQueueFamilies(VkPhysicalDevice device) override;

public:
    VkPhyDevice(vector<const char *> deviceExts, vector<const char *> extension = vector<const char *>({
                                                                                                               VK_KHR_SWAPCHAIN_EXTENSION_NAME
                                                                                                       }))
            : PhysicalDevice(extension) {
    }


};
#endif //ASS0_VULKANFRAMEWORK_H
