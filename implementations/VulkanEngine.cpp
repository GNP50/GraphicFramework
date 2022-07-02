#include "VulkanEngine.h"

using namespace Vulkan;

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


bool VkEngine::mainLoop() {
    if(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        std::this_thread::sleep_for(2000ms);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            return false;
        }

        return true;
    }

    return false;
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

