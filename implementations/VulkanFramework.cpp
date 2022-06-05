//
// Created by Pierluigi Carlino on 06/03/22.
//

#include "VulkanFramework.h"

using namespace Vulkan;

void VkAppConfiguration::setAppMajor(int appMajor = 1) {
    AppConfiguration::setApiVersion(appMajor);
};

void VkAppConfiguration::setAppMinor(int appMinor = 0) {
    AppConfiguration::setAppMinor(appMinor);
};

void VkAppConfiguration::setAppPatch(int appPatch = 0) {
    AppConfiguration::setAppPatch(appPatch);
};


void VkAppConfiguration::setEngineMajor(int engineMajor = 1) {
    AppConfiguration::setEngineMajor(engineMajor);
};


void VkAppConfiguration::setEngineMinor(int engineMinor = 1) {
    AppConfiguration::setEngineMinor(engineMinor);
};

void VkAppConfiguration::setEnginePatch(int enginePatch = 0) {
    AppConfiguration::setEnginePatch(enginePatch);
};

void VkAppConfiguration::setEngineName(string engineName = "No engine") {
    AppConfiguration::setEngineName(engineName);
};

void VkAppConfiguration::setAppName(string AppName = "No name") {
    AppConfiguration::setAppName(AppName);
};

void VkAppConfiguration::setApiVersion(int apiVersion = VK_API_VERSION_1_0) {
    AppConfiguration::setApiVersion(apiVersion);
};

void VkAppConfiguration::setStructType(VkStructureType structureType) {
    AppConfiguration::setStructType(structureType);
}

void VkAppConfiguration::setRequiredExtension(bool enableValidationLayers = false) {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions =
            glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    this->extension = vector<const char *>(glfwExtensions,
                                           glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers)
        extension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

void VkEngine::setAppConfig(VkAppConfiguration config = VkAppConfiguration()) {
    GraphicEngine::setAppConfig(config);
};


bool VkEngine::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount,
                                       availableLayers.data());

    for (const char *layerName: validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VkEngine::defaultDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}


void VkEngine::setInstance() {
    this->setAppConfig();
    this->appConfiguration.setRequiredExtension(enableValidationLayers);

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &(this->appConfiguration.returnObj());
    createInfo.pNext = NULL;
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
    setupDebugMessenger();

    if (enableValidationLayers) {
        setValidationLayers();
        createInfo.enabledLayerCount =
                static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)
                &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    setupDebugCallback();
    if (result != VK_SUCCESS) {
        printError(result);
        throw std::runtime_error("failed to create instance!");
    }

}

void VkEngine::initWindow(int w, int h) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    this->window = glfwCreateWindow(w, h,
                                    this->appConfiguration.appName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, reinterpret_cast<GLFWframebuffersizefun>(this->framebufferResizeCallback));

};

void VkEngine::createSurface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface)
        != VK_SUCCESS)
        Terminal::getInstance().printnl("failed to create window surface!");
}

std::vector<const char *> VkEngine::getRequiredInstanceExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions =
            glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}


void VkEngine::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();


        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            break;
        }
    }

    //TODO WRITE PROPERLY THIS SHIT OF CODE
    //vkDeviceWaitIdle(device);
}


vector<string> VkPhyDevice::getAllDevicesName() {
    std::vector<std::string> toRet;

    for (int i = 0; i < devices.size(); i++)
        toRet.push_back(this->getDeviceName(i));

    return toRet;
}

string VkPhyDevice::getDeviceName(int off) {
    VkPhysicalDeviceProperties prop;
    vkGetPhysicalDeviceProperties(devices[off], &prop);
    return string(prop.deviceName);

};

VkSurfaceFormatKHR VkPhyDevice::chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat: availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VkPhyDevice::chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode: availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VkPhyDevice::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(this->engine.getWindow(), &width, &height);

        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };
        actualExtent.width = std::max(capabilities.minImageExtent.width,
                                      std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height, actualExtent.height));
        return actualExtent;
    }
}

void VkEngine::cleanup() {
    /*

cleanupSwapChain();

for (size_t i = 0; i < Scene.size(); i++) {
    vkDestroySampler(device, Scene[i].TD.textureSampler, nullptr);
    vkDestroyImageView(device, Scene[i].TD.textureImageView, nullptr);
    vkDestroyImage(device, Scene[i].TD.textureImage, nullptr);
    vkFreeMemory(device, Scene[i].TD.textureImageMemory, nullptr);

    vkDestroyBuffer(device, Scene[i].MD.indexBuffer, nullptr);
    vkFreeMemory(device, Scene[i].MD.indexBufferMemory, nullptr);

    vkDestroyBuffer(device, Scene[i].MD.vertexBuffer, nullptr);
    vkFreeMemory(device, Scene[i].MD.vertexBufferMemory, nullptr);
}

vkDestroyDescriptorSetLayout(device, PhongDescriptorSetLayout, nullptr);

for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(device, inFlightFences[i], nullptr);
}

vkDestroyCommandPool(device, commandPool, nullptr);

vkDestroyDevice(device, nullptr);

if (enableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
}*/

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
}

void VkEngine::defaultWindowResizeHandler(GLFWwindow *window, int width, int height) {
    //todo
    (glfwGetWindowUserPointer(window));

};

void VkEngine::setResizeWindowCallBack(void *newFunc) {
    this->framebufferResizeCallback = framebufferResizeCallback;
};

bool VkPhyDevice::isDevicesSuitable(vector<VkPhysicalDevice_T *> devices) {
    return true;
    //TODO real implementation
//    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
//           supportedFeatures.samplerAnisotropy;
};

VkPhysicalDeviceFeatures VkPhyDevice::getSupportedExtensions(int offset) {
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(devices[offset], &supportedFeatures);
    return supportedFeatures;
};

VkDevice VkPhyDevice::createLogicalDevice(int off) {
    if (logicalDevices[off] != nullptr)
        return logicalDevices[off];


    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    auto uniqueQueueFamilies = this->getUniqueIndicies<vector<uint32_t>>(off);


    float queuePriority = 1.0f;
    for (uint32_t queueFamily: uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount =
            static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount =
            static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (engine.enableValidationLayers) {
        createInfo.enabledLayerCount =
                static_cast<uint32_t>(engine.validationLayers.size());
        createInfo.ppEnabledLayerNames = engine.validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkDevice device;
    VkResult result = vkCreateDevice(this->devices[off], &createInfo, nullptr, &device);

    if (result != VK_SUCCESS) {
        Terminal::getInstance().print(result);
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, uniqueQueueFamilies[0], 0, &graphicsQueue);
    vkGetDeviceQueue(device, uniqueQueueFamilies[1], 0, &presentQueue);
    logicalDevices[off] = device;

    return logicalDevices[off];
}

bool VkPhyDevice::isFeatureSupported(int offset) {
    //todo implementation
    return false;
};

bool VkPhyDevice::supportQueues(int off, vector<VkQueueFlagBits> flags) {

    presentIndicies.clear();
    queueIndicies.clear();

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(devices[off], &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(devices[off], &queueFamilyCount,
                                             queueFamilies.data());

    int founded = 0, i = 0;
    auto c_flags = vector<VkQueueFlagBits>(flags);

    for (const auto &queueFamily: queueFamilies) {
        for (int j = 0; j < c_flags.size(); j++) {
            if (queueFamily.queueFlags & c_flags[j]) {
                founded++;
                //c_flags.erase(c_flags.begin()+j);
                if (!queueIndicies.contains(off))
                    queueIndicies.insert({off, vector<pair<VkQueueFlagBits, int>>()});

                queueIndicies[off].push_back({c_flags[j], i});
                //break;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(this->devices[off], i, surface, &presentSupport);

            if (presentSupport) {
                if (!presentIndicies.contains(off))
                    presentIndicies.insert({off, vector<int>()});

                presentIndicies[off].push_back(i);
            }

        }

        //if(founded==flags.size())
        //    return true;

        i++;
    }

    return presentIndicies.size() > 0 && queueIndicies.size() > 0;
}


void VkPhyDevice::initDevices(VkInstance instance) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    this->devices = vector<VkPhysicalDevice>(deviceCount);
    this->logicalDevices = vector<VkDevice>(deviceCount);

    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    this->surface = engine.getSurface();

    Terminal::getInstance().print("Physical devices found: ");
    Terminal::getInstance().printnl(deviceCount);
};

const vector<glm::vec3> &Object::getVertex() const {
    return vertex;
}

const vector<glm::vec3> &Object::getIndicies() const {
    return indicies;
}
