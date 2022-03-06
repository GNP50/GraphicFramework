//
// Created by Pierluigi Carlino on 06/03/22.
//

#include "VulkanFramework.h"


void  VkAppConfiguration::setAppMajor(int appMajor = 1) {
    AppConfiguration::setApiVersion(appMajor);
};

void  VkAppConfiguration::setAppMinor(int appMinor = 0){
    AppConfiguration::setAppMinor(appMinor);
};

void  VkAppConfiguration::setAppPatch(int appPatch = 0){
    AppConfiguration::setAppPatch(appPatch);
};


void  VkAppConfiguration::setEngineMajor(int engineMajor = 1){
    AppConfiguration::setEngineMajor(engineMajor);
};


void  VkAppConfiguration::setEngineMinor(int engineMinor = 1){
    AppConfiguration::setEngineMinor(engineMinor);
};
void  VkAppConfiguration::setEnginePatch(int enginePatch = 0){
    AppConfiguration::setEnginePatch(enginePatch);
};

void  VkAppConfiguration::setEngineName(string engineName = "No engine"){
    AppConfiguration::setEngineName(engineName);
};

void  VkAppConfiguration::setAppName(string AppName = "No name"){
    AppConfiguration::setAppName(engineName);
};

void  VkAppConfiguration::setApiVersion(int apiVersion = VK_API_VERSION_1_0){
    AppConfiguration::setApiVersion(apiVersion);
};

void VkAppConfiguration::setStructType(VkStructureType structureType) {
    AppConfiguration::setStructType(structureType);
}

void VkAppConfiguration::setRequiredExtension() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions =
            glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    this->extension = vector<const char*>(glfwExtensions,
                                          glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers)
        extension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

void VkEngine::setAppConfig (VkAppConfiguration config){
    GraphicEngine::setAppConfig(config);
};

void VkEngine::setInstance() {
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &(this->appConfiguration.returnObj());

    createInfo.enabledLayerCount = 0;



    createInfo.enabledExtensionCount =
            static_cast<uint32_t>(this->appConfiguration.extension.size());
    createInfo.ppEnabledExtensionNames = this->appConfiguration.extension.data();

    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensionsPr(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensionsPr.data());

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount =
                static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)
                           &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if(result != VK_SUCCESS) {
        PrintVkError(result);
        throw std::runtime_error("failed to create instance!");
    }
}

