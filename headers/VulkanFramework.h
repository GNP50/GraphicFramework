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
    virtual void setAppConfig (VkAppConfiguration config) override;
    virtual void setInstance() override;

    //window handlers
    virtual void initWindow(int w,int h) override;
    virtual void defaultWindowResizeHandler(GLFWwindow* window, int width, int height) override;
    virtual void setResizeWindowCallBack(void* newFunc) override;

    virtual bool checkValidationLayerSupport() override;

    virtual void setupDebugMessenger() override{
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


    //clean up logic
    virtual void cleanup() override;

    virtual std::vector<const char*> getRequiredInstanceExtensions() override;


private:
};



//todo decide if it have to been general

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
        VkExtensionProperties,
        VkDevice,
        VkEngine,
        VkQueue,
        VkImage,
        VkSwapchainKHR,
        VkFormat,
        VkExtent2D
>{
protected:
    virtual bool isDevicesSuitable(vector<VkPhysicalDevice> devices) override;
    virtual bool isFeatureSupported(int offset) override;
    virtual bool isExtensionSupported(int offset,vector<VkExtensionProperties> requiredExtensions) override;

    virtual VkPhysicalDeviceFeatures getSupportedExtensions(int offset) override;
    virtual bool isSwapChainAdeguate(int offset,VkSurfaceKHR surface) override;

    virtual void initDevices(VkInstance instance) override;
    virtual SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,VkSurfaceKHR surface) override;

    template<typename Criteria>
    void getFirstSuitable(Criteria criteria);
    virtual VkSwapchainKHR createSwapChain(int off) override;


public:
    VkPhyDevice(VkEngine engine,VkInstance instance, vector<const char *> deviceExts) : PhysicalDevice(engine,instance, deviceExts) {
        initDevices(instance);
    }
    VkPhyDevice(vector<const char *> extension = vector<const char *>({
                                                                              VK_KHR_SWAPCHAIN_EXTENSION_NAME
                                                                      }))
            : PhysicalDevice(extension) {
    }
    virtual QueueFamilyIndices  findQueueFamilies(int off) override;
    virtual VkDevice createLogicalDevice(int off) override;

    //swaps function valutate if i have to make generic
    virtual VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats);

    virtual VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

};




template<typename BufferDataType>
class VkGenericBuffer:public Buffer<
        VkBuffer,
        VkBufferUsageFlags,
        VkMemoryPropertyFlags,
        VkDeviceMemory,
        VkDevice,VkPhyDevice,BufferDataType>{
protected:
    VkCommandPool  commandPool;
    int phyDeviceOff;

public:
    virtual void createBuffer() override{
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = static_cast<VkDeviceSize>(this->size);
        bufferInfo.usage = this->usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result =
                vkCreateBuffer(this->device, &bufferInfo, nullptr, &this->buffer);
        if (result != VK_SUCCESS) {
            Terminal::getInstance().print(result);
            Terminal::getInstance().print("\tfailed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(this->device, this->buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex =
                findMemoryType(memRequirements.memoryTypeBits, this->properties);

        result = vkAllocateMemory(this->device, &allocInfo, nullptr,
                                  &this->bufferMemory);
        if (result != VK_SUCCESS) {
            Terminal::getInstance().print(result);
            Terminal::getInstance().print("\tfailed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(this->device, this->buffer, this->bufferMemory, 0);
    };
    virtual uint32_t findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties) override{
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(this->physicalDevice.getDevice(phyDeviceOff), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    VkGenericBuffer(int size): Buffer<VkBuffer, VkBufferUsageFlags, VkMemoryPropertyFlags, VkDeviceMemory, VkDevice, VkPhyDevice,BufferDataType>(size),
                               VkPhyDevice(nullptr, vector<const char *>()) {

    }

    VkGenericBuffer(const VkGenericBuffer& toCopy): Buffer<VkBuffer, VkBufferUsageFlags, VkMemoryPropertyFlags, VkDeviceMemory, VkDevice, VkPhyDevice,BufferDataType>(toCopy),
                                                    VkPhyDevice(nullptr, vector<const char *>()) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional copyRegion.dstOffset = 0; // Optional
        copyRegion.size = this->size;
        vkCmdCopyBuffer(commandBuffer, toCopy.buffer, this->buffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    ~VkGenericBuffer(){
        vkDestroyBuffer(this->device, this->buffer, nullptr);
        vkFreeMemory(this->device, this->bufferMemory, nullptr);
    }

    VkCommandBuffer beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(this->device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        vkQueueSubmit(this->physicalDevice.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(this->physicalDevice.graphicsQueue);

        vkFreeCommandBuffers(this->device, commandPool, 1, &commandBuffer);
    }
    void createCommandPool() {
        QueueFamilyIndices queueFamilyIndices =
                this->physicalDevice.findQueueFamilies(phyDeviceOff);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = 0; // Optional

        VkResult result = vkCreateCommandPool(this->device, &poolInfo, nullptr, &commandPool);
        if (result != VK_SUCCESS) {
            Terminal::getInstance().printnl(result,BackGroudColor::red,ForeGroudColor::red);
            throw std::runtime_error("failed to create command pool!");
        }
    }


};

template<typename BufferDataType>
class VkStagingBuffer:VkGenericBuffer<BufferDataType> {
protected:

public:
    VkStagingBuffer(int size, vector<BufferDataType> data):VkGenericBuffer<BufferDataType>(size) {
        this->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        this->data = data;
    }

    virtual void create() override {
        this->createBuffer();
        void *ppData;
        vkMapMemory(this->device, this->bufferMemory, 0, this->bufferSize, 0, &ppData);

        memcpy(data, this->data.data(), (size_t) this->bufferSize);
        vkUnmapMemory(this->device, this->stagingBufferMemory);
    };
};

template<typename BufferDataType>
class VkVertexBuffer:VkGenericBuffer<BufferDataType>
{
public:

    VkVertexBuffer(int size):VkGenericBuffer<BufferDataType>(size){
        this->usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        this->properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    }
    virtual void create() override{
        VkStagingBuffer<BufferDataType> stagingBuffer;
        stagingBuffer.data = this->data;
        stagingBuffer.create();
        this->createBuffer();

        copyBuffer(stagingBuffer.buffer, this->buffer, this->bufferSize);
    };
};

template<typename BufferDataType>
class VkIndexBuffer:VkGenericBuffer<BufferDataType>
{
public:

    VkIndexBuffer(int size):VkGenericBuffer<BufferDataType>(size){
        this->usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        this->properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    }
    virtual void create() override{
        VkStagingBuffer<BufferDataType> stagingBuffer;
        stagingBuffer.data = this->data;
        stagingBuffer.create();
        this->createBuffer();

        copyBuffer(stagingBuffer.buffer, this->buffer, this->bufferSize);
    };
};

template<typename BufferDataType>
class VkUniformBuffer:VkGenericBuffer<BufferDataType>
{


public:

    VkUniformBuffer(int size):VkGenericBuffer<BufferDataType>(size){
        this->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    }
    virtual void create() override{
        VkStagingBuffer<BufferDataType> stagingBuffer;

        this->createBuffer();

        copyBuffer(stagingBuffer.buffer, this->buffer, this->bufferSize);
    };


};

/** logic to transfer

void createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(swapChainImages.size() * Scene.size());
    uniformBuffersMemory.resize(swapChainImages.size() * Scene.size());

    for (size_t i = 0; i < swapChainImages.size() * Scene.size(); i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     uniformBuffers[i], uniformBuffersMemory[i]);
    }

    bufferSize = sizeof(GlobalUniformBufferObject);

    globalUniformBuffers.resize(swapChainImages.size());
    globalUniformBuffersMemory.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     globalUniformBuffers[i], globalUniformBuffersMemory[i]);
    }

    bufferSize = sizeof(UniformBufferObject);
}


void createCommandBuffers() {
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo,
                                               commandBuffers.data());
    if (result != VK_SUCCESS) {
        PrintVkError(result);
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount =
                static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          PhongPipeline);

        for(int j = 0; j < Scene.size(); j++) {
            VkBuffer vertexBuffers[] = {Scene[j].MD.vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], Scene[j].MD.indexBuffer, 0,
                                 VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(commandBuffers[i],
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    PhongPipelineLayout, 0, 1,
                                    &PhongDescriptorSets[i + j * swapChainImages.size()],
                                    0, nullptr);

            vkCmdDrawIndexed(commandBuffers[i],
                             static_cast<uint32_t>(Scene[j].MD.indices.size()), 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffers[i]);
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject {
    alignas(16) glm::vec3 lightDir;
    alignas(16) glm::vec4 lightColor;
    alignas(16) glm::vec3 eyePos;
};


struct TextureData {
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    uint32_t mipLevels;
};

**/

#endif //ASS0_VULKANFRAMEWORK_H
