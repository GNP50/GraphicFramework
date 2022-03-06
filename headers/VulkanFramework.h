//
// Created by Pierluigi Carlino on 06/03/22.
//

#ifndef ASS0_VULKANFRAMEWORK_H
#define ASS0_VULKANFRAMEWORK_H
#include<GraphicsFramework.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>




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
    virtual void setRequiredExtension();

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
class VkEngine:public GraphicEngine<VkInstance,VkAppConfiguration,VkResult>{
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
private:
};
#endif //ASS0_VULKANFRAMEWORK_H
