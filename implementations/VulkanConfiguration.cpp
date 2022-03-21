//
// Created by Pierluigi Carlino on 20/03/22.
//

#include <VulkanConfigurations.h>


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

void VkAppConfiguration::setRequiredExtension(bool enableValidationLayers = false) {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions =
            glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    this->extension = vector<const char*>(glfwExtensions,
                                          glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers)
        extension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}


