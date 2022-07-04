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

/**todo decide how to put into
void VkEngine::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    static float lastTime = 0.0f;

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>
            (currentTime - startTime).count();
    float deltaT = time - lastTime;
    lastTime = time;

    static float debounce = time;




    const float ROT_SPEED = glm::radians(60.0f);
    const float MOVE_SPEED = 1.25f;
    const float MOUSE_RES = 500.0f;

    static double old_xpos = 0, old_ypos = 0;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    double m_dx = xpos - old_xpos;
    double m_dy = ypos - old_ypos;
    old_xpos = xpos; old_ypos = ypos;
//std::cout << xpos << " " << ypos << " " << m_dx << " " << m_dy << "\n";

    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        CamAng.y += m_dx * ROT_SPEED / MOUSE_RES;
        CamAng.x += m_dy * ROT_SPEED / MOUSE_RES;
    }

    if(glfwGetKey(window, GLFW_KEY_LEFT)) {
        CamAng.y += deltaT * ROT_SPEED;
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT)) {
        CamAng.y -= deltaT * ROT_SPEED;
    }
    if(glfwGetKey(window, GLFW_KEY_UP)) {
        CamAng.x += deltaT * ROT_SPEED;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN)) {
        CamAng.x -= deltaT * ROT_SPEED;
    }
    if(glfwGetKey(window, GLFW_KEY_Q)) {
        CamAng.z -= deltaT * ROT_SPEED;
    }
    if(glfwGetKey(window, GLFW_KEY_E)) {
        CamAng.z += deltaT * ROT_SPEED;
    }

    glm::mat3 CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f), CamAng.y, glm::vec3(0.0f, 1.0f, 0.0f))) *
                       glm::mat3(glm::rotate(glm::mat4(1.0f), CamAng.x, glm::vec3(1.0f, 0.0f, 0.0f))) *
                       glm::mat3(glm::rotate(glm::mat4(1.0f), CamAng.z, glm::vec3(0.0f, 0.0f, 1.0f)));


    if(glfwGetKey(window, GLFW_KEY_A)) {
        CamPos -= MOVE_SPEED * glm::vec3(glm::rotate(glm::mat4(1.0f), CamAng.y,
                                                     glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1,0,0,1)) * deltaT;
    }
    if(glfwGetKey(window, GLFW_KEY_D)) {
        CamPos += MOVE_SPEED * glm::vec3(glm::rotate(glm::mat4(1.0f), CamAng.y,
                                                     glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1,0,0,1)) * deltaT;
    }
    if(glfwGetKey(window, GLFW_KEY_S)) {
        CamPos += MOVE_SPEED * glm::vec3(glm::rotate(glm::mat4(1.0f), CamAng.y,
                                                     glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0,0,1,1)) * deltaT;
    }
    if(glfwGetKey(window, GLFW_KEY_W)) {
        CamPos -= MOVE_SPEED * glm::vec3(glm::rotate(glm::mat4(1.0f), CamAng.y,
                                                     glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0,0,1,1)) * deltaT;
    }
    if(glfwGetKey(window, GLFW_KEY_F)) {
        CamPos -= MOVE_SPEED * glm::vec3(0,1,0) * deltaT;
    }
    if(glfwGetKey(window, GLFW_KEY_R)) {
        CamPos += MOVE_SPEED * glm::vec3(0,1,0) * deltaT;
    }
// std::cout << "Cam Pos: " << CamPos[0] << " " << CamPos[1] << " " << CamPos[2] << "\n";

    glm::mat4 CamMat = glm::translate(glm::transpose(glm::mat4(CamDir)), -CamPos);

    glm::mat4 Prj = glm::perspective(glm::radians(45.0f),
                                     width / (float) heigth,
                                     0.1f, 50.0f);
    Prj[1][1] *= -1;

    {
        int j = 0;
        UniformBufferObject ubo{};
        glm::vec3 delta;

        ubo.mMat = glm::mat4(1);

        ubo.mvpMat = Prj * CamMat * ubo.mMat;
        ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

        int i = j * swapChainImages.size() + currentImage;

        void* data;
        vkMapMemory(, uniformBuffersMemory[i], 0,
                    sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, uniformBuffersMemory[i]);
    }

    // updates global uniforms
    GlobalUniformBufferObject gubo{};
    gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)), sin(glm::radians(135.0f)), 0.0f);
    gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    gubo.eyePos = CamPos;

    void* data;
    vkMapMemory(device, globalUniformBuffersMemory[currentImage], 0,
                sizeof(gubo), 0, &data);
    memcpy(data, &gubo, sizeof(gubo));
    vkUnmapMemory(device, globalUniformBuffersMemory[currentImage]);
}**/
