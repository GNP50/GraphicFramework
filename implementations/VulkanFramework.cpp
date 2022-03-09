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
        printError(result);
        throw std::runtime_error("failed to create instance!");
    }
}

void VkEngine::initWindow(int w,int h) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    this->window = glfwCreateWindow(w, h,
    this->appConfiguration.appName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, reinterpret_cast<GLFWframebuffersizefun>(this->framebufferResizeCallback));

    if (glfwCreateWindowSurface(instance, window, nullptr, &surface)
        != VK_SUCCESS)
        Terminal::getInstance().printnl("failed to create window surface!");
};

void VkEngine::defaultWindowResizeHandler(GLFWwindow* window, int width, int height) {
    //todo
    (glfwGetWindowUserPointer(window));

};

void VkEngine::setResizeWindowCallBack(void* newFunc) {
    this->framebufferResizeCallback = framebufferResizeCallback;
};

bool VkPhyDevice::isDevicesSuitable(vector<VkPhysicalDevice_T *> devices){
    return true;
    //TODO real implementation
//    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
//           supportedFeatures.samplerAnisotropy;
};

VkPhysicalDeviceFeatures VkPhyDevice::getSupportedExtensions(int offset){
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(devices[offset], &supportedFeatures);
    return supportedFeatures;
};

VkPhyDevice::QueueFamilyIndices  VkPhyDevice::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             queueFamilies.data());

    int i=0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                             &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }
        i++;
    }

    return indices;
}

SwapChainSupportDetails VkPhyDevice::queryChainSupport(VkPhysicalDevice device,VkSurfaceKHR surface) {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                              &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface,
                                             &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                              &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                                  &presentModeCount, details.presentModes.data());
    }

    return details;
}


bool VkPhyDevice::isSwapChainAdeguate(int offset,VkSurfaceKHR surface){
    SwapChainSupportDetails swapChainSupport = queryChainSupport(devices.at(offset),surface);
    return !swapChainSupport.formats.empty() &&
           !swapChainSupport.presentModes.empty();
};


void VkPhyDevice::initDevices(VkInstance instance){
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    this->devices = vector<VkPhysicalDevice>(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    Terminal::getInstance().print("Physical devices found: ");
    Terminal::getInstance().printnl(deviceCount);
};

bool VkPhyDevice::isExtensionSupported(int offset,vector<VkExtensionProperties> requiredExtensions){
    QueueFamilyIndices indices = findQueueFamilies(devices[offset]);

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(devices[offset], nullptr,
                                         &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(devices[offset], nullptr,
                                         &extensionCount, availableExtensions.data());

    requiredExtensions(deviceExtensions.begin(),
                                             deviceExtensions.end());

    for (const auto& extension : availableExtensions){
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
};



//todo add logic for get the best suitable(this piece is given by the init devices)
//for (const auto& device : devices) {
//if (isDeviceSuitable(device)) {
//physicalDevice = device;
//msaaSamples = getMaxUsableSampleCount();
//std::cout << "\n\nMaximum samples for anti-aliasing: " << msaaSamples << "\n\n\n";
//break;
//}
//}

//if (physicalDevice == VK_NULL_HANDLE) {
//throw std::runtime_error("failed to find a suitable GPU!");
//}
