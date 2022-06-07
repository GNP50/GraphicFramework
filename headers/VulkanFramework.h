//
//
// Created by Pierluigi Carlino on 06/03/22.
//

#ifndef ASS0_VULKANFRAMEWORK_H
#define ASS0_VULKANFRAMEWORK_H

#include<GraphicsFramework.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#define GLFW_INCLUDE_VULKAN

#include <map>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <typeindex>

namespace Vulkan {
//default debug caller
    static VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    class VkAppConfiguration : public AppConfiguration<VkApplicationInfo, VkStructureType> {
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

        virtual void setRequiredExtension(bool enableValidationLayers) override;

        virtual VkApplicationInfo &returnObj() {


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

            appRappresentation.apiVersion = VK_API_VERSION_1_0;
            appRappresentation.pNext = NULL;
            return appRappresentation;
        };
    };

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

        virtual void mainLoop() override;

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

        virtual void setupDebugCallback() override {
            setupDebugMessenger();
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




//todo decide if it have to been general

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };


    class VkPhyDevice : public PhysicalDevice<
            VkPhysicalDevice,
            VkInstance,
            VkSurfaceKHR,
            VkPhysicalDeviceFeatures,
            VkDevice,
            VkEngine,
            VkQueue,
            VkQueueFlagBits
    > {
    protected:
        virtual bool isDevicesSuitable(vector<VkPhysicalDevice> devices) override;

        virtual bool isFeatureSupported(int offset) override;


        virtual VkPhysicalDeviceFeatures getSupportedExtensions(int offset) override;


        virtual void initDevices(VkInstance instance) override;


        template<typename DeviceExt>
        bool isExtensionSupported(int offset, DeviceExt requiredExtensions);


        template<>
        bool isExtensionSupported(int offset, VkPhysicalDeviceType requiredExtensions) {
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceProperties(this->devices[offset], &deviceProperties);
            vkGetPhysicalDeviceFeatures(this->devices[offset], &deviceFeatures);


            return deviceProperties.deviceType == requiredExtensions;
        };

        template<>
        bool isExtensionSupported(int offset, vector<VkQueueFlagBits> requiredExtensions) {
            if (this->supportQueues(offset, requiredExtensions))
                return true;
            return false;
        };

        template<>
        bool isExtensionSupported(int offset, vector<VkExtensionProperties> requiredExtensions) {

            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(devices[offset], nullptr,
                                                 &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(devices[offset], nullptr,
                                                 &extensionCount, availableExtensions.data());

            for (const auto extension: availableExtensions) {
                bool found = false;
                string first(extension.extensionName);
                for (const auto reqExtension: requiredExtensions) {
                    string second(reqExtension.extensionName);
                    if (first == second && reqExtension.specVersion == extension.specVersion) {
                        continue;
                    } else {
                        found = false;
                        break;
                    }
                }
                if (!found)
                    return false;
            }

            return false;
        };


    public:
        VkPhyDevice(vector<const char *> deviceExts = vector<const char *>({
                                                                                   VK_KHR_SWAPCHAIN_EXTENSION_NAME
                                                                           })) : PhysicalDevice(deviceExts) {
            initDevices(instance);
        }

        virtual bool supportQueues(int off, vector<VkQueueFlagBits> flags) override;

        virtual VkDevice createLogicalDevice(int off) override;

        //swaps function valutate if i have to make generic
        virtual VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR> &availableFormats);

        virtual VkPresentModeKHR chooseSwapPresentMode(
                const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        virtual vector<string> getAllDevicesName() override;

        virtual string getDeviceName(int off) override;

        template<typename Criteria>
        void getFirstSuitable(Criteria criteria);

        template<>
        void getFirstSuitable<vector<VkExtensionProperties>>(vector<VkExtensionProperties> criteria) {
            //insert the type id if not present
            if (!this->supportedDevices.contains(typeid(VkExtensionProperties))) {
                auto toAdd = set<int>();


                this->supportedDevices.insert(
                        {
                                typeid(VkExtensionProperties),
                                toAdd
                        }
                );
            }

            for (int i = 0; i < this->devices.size(); i++) {
                if (this->isExtensionSupported(i, criteria))
                    this->supportedDevices[typeid(VkExtensionProperties)].insert(i);
                if (this->verbose) {
                    Terminal::getInstance().printnl("Found suitable:\t" + this->getDeviceName(i));
                }
            }

        };

        template<>
        void getFirstSuitable<vector<VkPhysicalDeviceType>>(vector<VkPhysicalDeviceType> criteria) {
            //insert the type id if not present
            if (!this->supportedDevices.contains(typeid(VkPhysicalDeviceType))) {
                auto toAdd = set<int>();

                this->supportedDevices.insert(
                        {
                                typeid(VkPhysicalDeviceType),
                                toAdd
                        }
                );
            }


            for (int i = 0; i < this->devices.size(); i++) {
                for (auto s_criteria: criteria) {
                    if (this->isExtensionSupported<VkPhysicalDeviceType>(i, s_criteria))
                        this->supportedDevices[typeid(VkPhysicalDeviceType)].insert(i);
                    if (this->verbose) {
                        Terminal::getInstance().printnl("Found suitable:\t" + this->getDeviceName(i));
                    }
                }
            }
        };

        template<>
        void getFirstSuitable(vector<VkQueueFlagBits> criteria) {
            //insert the type id if not present
            if (!this->supportedDevices.contains(typeid(VkQueueFlagBits))) {
                auto toAdd = set<int>();


                this->supportedDevices.insert(
                        {
                                typeid(VkQueueFlagBits),
                                toAdd
                        }
                );
            }


            for (int i = 0; i < this->devices.size(); i++) {

                if (this->isExtensionSupported<>(i, criteria))
                    this->supportedDevices[typeid(VkQueueFlagBits)].insert(i);
                if (this->verbose) {
                    Terminal::getInstance().printnl("Found suitable:\t" + this->getDeviceName(i));
                }
            }
        };

        template<typename T>
        T getUniqueIndicies(int off) {
            auto graphFamilyIndVec = this->queueIndicies[off];
            auto presentFamilyInd = this->presentIndicies[off];

            uint32_t queueFamilyIndices[] = {0, 0};

            for (auto gF: graphFamilyIndVec) {
                queueFamilyIndices[0] = gF.second;
                for (auto i: presentIndicies[off]) {
                    queueFamilyIndices[1] = i;

                    if (queueFamilyIndices[0] == queueFamilyIndices[1]) break;
                }
                if (queueFamilyIndices[0] == queueFamilyIndices[1]) break;
            }

            if constexpr(std::same_as<T, uint32_t *>) {
                return queueFamilyIndices;
            } else if constexpr(std::same_as<T, vector<uint32_t>>) {
                return vector<uint32_t>{{
                                                queueFamilyIndices[0], queueFamilyIndices[1]
                                        }};
            }
        };


    };


    template<typename BufferDataType>
    class VkGenericBuffer : public Buffer<
            VkBuffer,
            VkBufferUsageFlags,
            VkMemoryPropertyFlags,
            VkDeviceMemory,
            VkDevice, VkPhyDevice, BufferDataType> {
    protected:
        VkCommandPool commandPool;
        int phyDeviceOff;

    public:
        virtual void createBuffer() override {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = static_cast<VkDeviceSize>(this->bufferSize);
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
                                        VkMemoryPropertyFlags properties) override {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(this->physicalDevice->getDevice(this->phyDeviceOff), &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) &&
                    (memProperties.memoryTypes[i].propertyFlags & properties) ==
                    properties) {
                    return i;
                }
            }

            throw std::runtime_error("failed to find suitable memory type!");
        }

        VkGenericBuffer(int size, int choosenDev) {
            this->size = size;
            this->phyDeviceOff = choosenDev;
            this->device = nullptr;
        }

        VkGenericBuffer(const VkGenericBuffer &toCopy) {
            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0; // Optional copyRegion.dstOffset = 0; // Optional
            copyRegion.size = this->size;
            vkCmdCopyBuffer(commandBuffer, toCopy.buffer, this->buffer, 1, &copyRegion);

            endSingleTimeCommands(commandBuffer);
        }

        ~VkGenericBuffer() {
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
            vkQueueSubmit(this->physicalDevice->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(this->physicalDevice->graphicsQueue);

            vkFreeCommandBuffers(this->device, commandPool, 1, &commandBuffer);
        }

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
            createCommandPool();
            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0; // Optional copyRegion.dstOffset = 0; // Optional
            copyRegion.size = size;
            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

            endSingleTimeCommands(commandBuffer);
        }

        void createCommandPool() {
            auto graphFamilyIndVec = this->physicalDevice->
                    template getUniqueIndicies<vector<uint32_t>>(this->phyDeviceOff);

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.queueFamilyIndex = graphFamilyIndVec[0];
            poolInfo.flags = 0; // Optional

            VkResult result = vkCreateCommandPool(this->device, &poolInfo, nullptr, &commandPool);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result, BackGroudColor::red, ForeGroudColor::red);
                throw std::runtime_error("failed to create command pool!");
            }
        }

        virtual void create() override {

        }

        void setPhysicalDevice(VkPhyDevice *phyDevice){
            Buffer<
                    VkBuffer,
                    VkBufferUsageFlags,
                    VkMemoryPropertyFlags,
                    VkDeviceMemory,
                    VkDevice, VkPhyDevice, BufferDataType>::setPhysicalDevice(phyDevice);
            setLogicalDevice(
                    *phyDevice->getPerfectCriteriaMatcher().begin()
                    );
        }
        void setLogicalDevice() {
            setLogicalDevice(this->phyDeviceOff);
        }
        void setLogicalDevice(int off) {
            this->phyDeviceOff = off;
            this->device = this->physicalDevice->getLogicalDevice(off);
        }

    };

    template<typename BufferDataType>
    class VkStagingBuffer : public VkGenericBuffer<BufferDataType> {
    protected:

    public:
        VkStagingBuffer(int size, int choosed, vector<BufferDataType> data) : VkGenericBuffer<BufferDataType>(size,
                                                                                                              choosed) {
            this->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            this->data = data;
        }

        virtual void create() override {
            this->createBuffer();
            void *ppData;
            vkMapMemory(this->device, this->bufferMemory, 0, this->bufferSize, 0, &ppData);

            memcpy(this->data.data(), ppData, (size_t) this->bufferSize);
            vkUnmapMemory(this->device, this->bufferMemory);
        };

        VkBuffer getBuffer() { return this->buffer; };
    };

    template<typename BufferDataType>
    class VkVertexBuffer : public VkGenericBuffer<BufferDataType> {
    public:

        VkVertexBuffer(int size, int choosenDev) : VkGenericBuffer<BufferDataType>(size, choosenDev) {
            this->usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            this->properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        }

        virtual void create() override {
            VkStagingBuffer<BufferDataType> stagingBuffer(this->size, this->phyDeviceOff,
                                                          vector<BufferDataType>(this->size));

            stagingBuffer.setData(this->data);
            stagingBuffer.setPhysicalDevice(this->physicalDevice);


            stagingBuffer.create();
            this->createBuffer();

            this->copyBuffer(stagingBuffer.getBuffer(), this->buffer, this->bufferSize);
        };
    };

    template<typename BufferDataType>
    class VkIndexBuffer : public VkGenericBuffer<BufferDataType> {
    public:

        VkIndexBuffer(int size, int choosenDev) : VkGenericBuffer<BufferDataType>(size, choosenDev) {
            this->usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            this->properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        }

        virtual void create() override {
            VkStagingBuffer<BufferDataType> stagingBuffer(this->size, this->phyDeviceOff,
                                                          vector<BufferDataType>(this->size));

            stagingBuffer.setData(this->data);
            stagingBuffer.setPhysicalDevice(this->physicalDevice);

            stagingBuffer.create();
            this->createBuffer();

            this->copyBuffer(stagingBuffer.getBuffer(), this->buffer, this->bufferSize);
        };
    };

    template<typename BufferDataType>
    class VkUniformBuffer : public VkGenericBuffer<BufferDataType> {


    public:

        VkUniformBuffer(int size, int choosenDev) : VkGenericBuffer<BufferDataType>(size, choosenDev) {
            this->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        }

        virtual void create() override {
            VkStagingBuffer<BufferDataType> stagingBuffer(this->size, this->phyDeviceOff,
                                                          vector<BufferDataType>(this->size));

            this->createBuffer();

            this->copyBuffer(stagingBuffer.getBuffer(), this->buffer, this->bufferSize);
        };


    };

    class VkShader : public Shader<VkPipelineShaderStageCreateInfo> {
    protected:
        VkShaderModuleCreateInfo createInfo = {};
        VkShaderModule shaderModule = {};

    public:
        VkShader(string filename) : Shader(filename) {
            createInfo = {};
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

            stageInfo.module = shaderModule;

            //TODO think about a better way to handle this
            stageInfo.pName = "main";

        };

        VkShader(VkShader& shader):Shader(shader){
            this->shaderModule = shader.shaderModule;
            this->createInfo = shader.createInfo;
        }

        template<typename... T>
        void create(T...);

        template<typename... T>
        void destroy(T...);


        template<>
        void create(VkDevice device) {
            if (this->data.size() == 0)
                this->readFile();

            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = this->data.size();
            createInfo.pCode = reinterpret_cast<const uint32_t *>(this->data.data());

            if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shader module!");
            }

            stageInfo.module = shaderModule;
        }

        template<>
        void destroy(VkDevice device) {
            vkDestroyShaderModule(device, shaderModule, nullptr);
        }

    };

    class VkFragmentShader : public VkShader {
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        VkPipelineColorBlendStateCreateInfo colorBlending = {};

    public:
        VkFragmentShader(string filename,
                         VkPolygonMode polyMode = VK_POLYGON_MODE_FILL
        ) : VkShader(filename) {
            stageInfo = {};
            stageInfo.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            stageInfo.pName = "main";

            rasterizer = {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;

            rasterizer.polygonMode = polyMode;

            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


            colorBlendAttachment = {};
            colorBlendAttachment.colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                    VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

            colorBlending = {};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional



        }
    };

    class VkVertexShader : public VkShader {
    protected:
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};

    public:
        VkVertexShader(
                string filename,
                VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
        ) : VkShader(filename) {
            stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

            vertexInputInfo = {};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

            inputAssembly = {};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;
        }
    };

    class VkSwapChain : public GenericSwapChain<
            VkSwapchainKHR ,
            VkSurfaceFormatKHR,
            SwapChainSupportDetails,
            VkExtent2D,
            VkImage,VkImageView> {

    private:
        VkPhyDevice *deviceRepository;
        int choosenDevice = 0;

        VkSurfaceKHR getSurface() { deviceRepository->getSurface(); };

        VkPhysicalDevice getDevice(int off) { return deviceRepository->getDevice(off); };
    public:

        ~VkSwapChain(){
            if(!isCreated){

                vkDestroySwapchainKHR(deviceRepository->getLogicalDevice(choosenDevice), swapChain, nullptr);
                for (auto imageView : swapChainImageViews) {
                    vkDestroyImageView(deviceRepository->getLogicalDevice(choosenDevice), imageView, nullptr);
                }
                isCreated = false;
            }
        }
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
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
        };

        SwapChainSupportDetails querySwapChainSupport(int off) {
            auto device = this->getDevice(off);
            auto surface = deviceRepository->getSurface();
            return querySwapChainSupport(device, surface);
        }

        bool isSwapChainAdeguate(int offset) {
            auto device = this->getDevice(offset);
            auto surface = deviceRepository->getSurface();

            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
            return !swapChainSupport.formats.empty() &&
                   !swapChainSupport.presentModes.empty();
        };


        void setDeviceRepository(VkPhyDevice *device) {
            this->deviceRepository = device;
        }

        template<typename Criteria>
        void getFirstSuitable(Criteria criteria);

        template<>
        void getFirstSuitable<vector<VkSurfaceFormatKHR>>(vector<VkSurfaceFormatKHR> criteria) {
            //insert the type id if not present
            if (!this->supportedCapabilities.contains(typeid(VkSurfaceFormatKHR))) {
                auto toAdd = set<int>();


                this->supportedCapabilities.insert(
                        {
                                typeid(VkSurfaceFormatKHR),
                                toAdd
                        }
                );
            }


            for (int i = 0; i < this->deviceRepository->getAllDevicesName().size(); i++) {
                if (this->isExtensionSupported(i, criteria))
                    this->supportedCapabilities[typeid(VkExtensionProperties)].insert(i);
                if (this->verbose) {
                    Terminal::getInstance().printnl("Found suitable:\t" + deviceRepository->getDeviceName(i));
                }
            }

        };

        template<>
        void getFirstSuitable<vector<VkPresentModeKHR>>(vector<VkPresentModeKHR> criteria) {
            //insert the type id if not present
            if (!this->supportedCapabilities.contains(typeid(VkPresentModeKHR))) {
                auto toAdd = set<int>();


                this->supportedCapabilities.insert(
                        {
                                typeid(VkPresentModeKHR),
                                toAdd
                        }
                );
            }


            for (int i = 0; i < this->deviceRepository->getAllDevicesName().size(); i++) {
                if (this->isExtensionSupported(i, criteria))
                    this->supportedCapabilities[typeid(VkPresentModeKHR)].insert(i);
                if (this->verbose) {
                    Terminal::getInstance().printnl("Found suitable:\t" + deviceRepository->getDeviceName(i));
                }
            }

        };

        VkPhyDevice* getDeviceRepository(){return deviceRepository;}
        template<>
        void getFirstSuitable<VkSurfaceCapabilitiesKHR>(VkSurfaceCapabilitiesKHR criteria) {
            //insert the type id if not present
            if (!this->supportedCapabilities.contains(typeid(VkSurfaceCapabilitiesKHR))) {
                auto toAdd = set<int>();


                this->supportedCapabilities.insert(
                        {
                                typeid(VkSurfaceCapabilitiesKHR),
                                toAdd
                        }
                );
            }


            for (int i = 0; i < this->deviceRepository->getAllDevicesName().size(); i++) {
                if (this->isExtensionSupported(i, criteria))
                    this->supportedCapabilities[typeid(VkSurfaceCapabilitiesKHR)].insert(i);
                if (this->verbose) {
                    Terminal::getInstance().printnl("Found suitable:\t" + deviceRepository->getDeviceName(i));
                }
            }

        };


        template<typename Criteria>
        bool isExtensionSupported(int off, Criteria criteria);

        template<>
        bool isExtensionSupported<vector<VkSurfaceFormatKHR>>(int off, vector<VkSurfaceFormatKHR> criteria) {
            auto cab = this->querySwapChainSupport(off).formats;
            for (auto c: criteria) {
                bool found = false;
                for (auto f: cab)
                    if (f.colorSpace == c.colorSpace || f.format == c.format) {
                        found = true;
                        break;
                    }
                if(found==false)
                    return false;
            }

            return true;
        }

        template<>
        bool isExtensionSupported<vector<VkPresentModeKHR>>(int off, vector<VkPresentModeKHR> criteria) {
            auto cab = this->querySwapChainSupport(off).presentModes;
            for (auto c: criteria) {
                bool found = false;
                for (auto f: cab)
                    if (f == c){
                        found = true;
                        break;
                    }
                if(!found)
                    return false;
            }
            return true;
        }

        template<>
        bool isExtensionSupported<VkSurfaceCapabilitiesKHR>(int off, VkSurfaceCapabilitiesKHR criteria) {
            auto cab = this->querySwapChainSupport(off).capabilities;

            bool countCondition = cab.minImageCount>=criteria.minImageCount
                            && cab.maxImageCount>=criteria.maxImageCount
                            && cab.maxImageArrayLayers>=criteria.maxImageArrayLayers;

            //implement other conditions
            return countCondition;
        }

        set<int> getPerfectCriteriaMatcher() {
            auto toRet = set<int>();
            for (auto choosed: supportedCapabilities) {
                auto Update = set<int>();
                if (toRet.empty())
                    set_union(toRet.begin(), toRet.end(), choosed.second.begin(), choosed.second.end(),
                              inserter(Update, Update.begin()));
                else
                    set_intersection(toRet.begin(), toRet.end(), choosed.second.begin(), choosed.second.end(),
                                     inserter(Update, Update.begin()));

                toRet = Update;
            }

            return toRet;
        }

        VkDevice getChoosenDevice(){
            return deviceRepository->getLogicalDevice(choosenDevice);
        }

        void createSwapChain(int off,VkSurfaceFormatKHR& surfaceFormat,VkPresentModeKHR& presentMode,VkExtent2D& extent  ) {
            if(isCreated)
                return;
            this->swapChainExtent = extent;
            this->swapChainImageFormat = surfaceFormat;

            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(off);

            uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
            swapChainImages.resize(imageCount);

            VkSwapchainCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = deviceRepository->getSurface();

            createInfo.minImageCount = imageCount;
            createInfo.imageFormat = surfaceFormat.format;
            createInfo.imageColorSpace = surfaceFormat.colorSpace;
            createInfo.imageExtent = extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            auto queueFamilyIndices = this->deviceRepository->
                    template getUniqueIndicies<uint32_t*>(off);



            if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
                createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices = queueFamilyIndices;
            } else {
                createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0; // Optional
                createInfo.pQueueFamilyIndices = nullptr; // Optional
            }

            createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            createInfo.presentMode = presentMode;
            createInfo.clipped = VK_TRUE;
            createInfo.oldSwapchain = VK_NULL_HANDLE;
            deviceRepository->createLogicalDevice(off);

            if (vkCreateSwapchainKHR(deviceRepository->getLogicalDevice(off), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
                throw std::runtime_error("failed to create swap chain!");
            }

            choosenDevice = off;
            isCreated = true;

        }

        void updateSwapImages(){
            uint32_t count = 0;
            vkGetSwapchainImagesKHR(deviceRepository->getLogicalDevice(choosenDevice), swapChain, &count, nullptr);
            swapChainImages.resize(count);
            vkGetSwapchainImagesKHR(deviceRepository->getLogicalDevice(choosenDevice), swapChain, &count, swapChainImages.data());
        }

        void createImageView(){
            swapChainImageViews.resize(swapChainImages.size());
            for (size_t i = 0; i < swapChainImages.size(); i++) {
                VkImageViewCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                createInfo.image = swapChainImages[i];

                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                createInfo.format = swapChainImageFormat.format;

                createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

                createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                createInfo.subresourceRange.baseMipLevel = 0;
                createInfo.subresourceRange.levelCount = 1;
                createInfo.subresourceRange.baseArrayLayer = 0;
                createInfo.subresourceRange.layerCount = 1;

                if (vkCreateImageView(deviceRepository->getLogicalDevice(choosenDevice), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create image views!");
                }

            }
        }
    };

    class VkPipeLine:public GenericGraphicPipeline<
            VkPipeline,
            VkPipelineLayout,
            VkPipelineShaderStageCreateInfo,
            VkPipelineVertexInputStateCreateInfo,
            VkPipelineInputAssemblyStateCreateInfo,
            VkPipelineRasterizationStateCreateInfo,
            VkPipelineMultisampleStateCreateInfo,
            VkPipelineColorBlendAttachmentState,
            VkPipelineColorBlendStateCreateInfo,
            VkPipelineLayoutCreateInfo,
            VkPipelineDepthStencilStateCreateInfo,
            VkGraphicsPipelineCreateInfo,
            VkShader,
            VkViewport,
            VkRect2D,
            VkDescriptorSetLayoutBinding,
            VkDescriptorSetLayoutCreateInfo,
            VkDescriptorSetLayout,
            VkAttachmentDescription,
            VkAttachmentReference,
            VkSubpassDescription,
            VkSubpassDependency,
            VkRenderPassCreateInfo,
            VkRenderPass,
            VkFramebuffer>{

        // Lesson 17
        struct Vertex {
            glm::vec3 pos;
            glm::vec3 norm;
            glm::vec2 texCoord;

            static VkVertexInputBindingDescription getBindingDescription() {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return bindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 3>
            getAttributeDescriptions() {
                std::array<VkVertexInputAttributeDescription, 3>
                        attributeDescriptions{};

                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(Vertex, pos);

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(Vertex, norm);

                attributeDescriptions[2].binding = 0;
                attributeDescriptions[2].location = 2;
                attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

                return attributeDescriptions;
            }
        };


    public:
        VkAttachmentDescription colorAttachmentResolve{};
        VkPhyDevice *deviceRepo;
        // Questi dovrebbero essere i render target
        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;

        VkPipeLine(VkShader& fragShader, VkShader& vertexShader,VkPhyDevice *devicerepo) : GenericGraphicPipeline(fragShader, vertexShader) {
            this->deviceRepo = devicerepo;
        }

        VkFormat findDepthFormat() {
            return findSupportedFormat({VK_FORMAT_D32_SFLOAT,
                                        VK_FORMAT_D32_SFLOAT_S8_UINT,
                                        VK_FORMAT_D24_UNORM_S8_UINT},
                                       VK_IMAGE_TILING_OPTIMAL,
                                       VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
        }

        VkFormat findSupportedFormat(const std::vector<VkFormat> candidates,
                                     VkImageTiling tiling, VkFormatFeatureFlags features) {
            for (VkFormat format : candidates) {
                VkFormatProperties props;

                vkGetPhysicalDeviceFormatProperties(deviceRepo->getDevice(
                        *deviceRepo->getPerfectCriteriaMatcher().begin()
                        ), format, &props);
                if (tiling == VK_IMAGE_TILING_LINEAR &&
                    (props.linearTilingFeatures & features) == features) {
                    return format;
                } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                           (props.optimalTilingFeatures & features) == features) {
                    return format;
                }
            }

            throw std::runtime_error("failed to find supported format!");
        }

        void createRenderPass(VkSwapChain swapChain){

            colorAttachment.format = swapChain.getSwapChainImageFormat().format;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;


            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = 1;
            renderPassInfo.pAttachments = &colorAttachment;
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;

            VkResult result = vkCreateRenderPass(swapChain.getChoosenDevice(), &renderPassInfo, nullptr,
                                                 &renderPass);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create render pass!");
            }
        }


        void createDescriptorSetLayout(VkSwapChain swapChain) {
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr;


            samplerLayoutBinding.binding = 1;
            samplerLayoutBinding.descriptorType =
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            //

            std::array<VkDescriptorSetLayoutBinding, 2> bindings =
                    {uboLayoutBinding, samplerLayoutBinding};

            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
            layoutInfo.pBindings = bindings.data();

            VkResult result = vkCreateDescriptorSetLayout(swapChain.getChoosenDevice(), &layoutInfo,
                                                          nullptr, &descriptorSetLayout);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create descriptor set layout!");
            }
        }

        void createGraphicsPipeline(VkSwapChain swapChain) {
            createRenderPass(swapChain);
            createDescriptorSetLayout(swapChain);

            createDescriptorSetLayout(swapChain);
            createColorResources(swapChain);

            VkPipelineShaderStageCreateInfo shaderStages[] =
                    {vertexShader.getStageCreateInfo(), fragmentShader.getStageCreateInfo()};

            vertexInputInfo.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            auto attributeDescriptions = Vertex::getAttributeDescriptions();
            auto bindingDescription = Vertex::getBindingDescription();
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount =
                    static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.pVertexAttributeDescriptions =
                    attributeDescriptions.data();

            inputAssembly.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            auto swapChainExtend = swapChain.getSwapChainExtent();

            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) swapChainExtend.width;
            viewport.height = (float) swapChainExtend.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;


            scissor.offset = {0, 0};
            scissor.extent = swapChainExtend;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

            rasterizer.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


            multisampling.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional

            colorBlendAttachment.colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT |
                    VK_COLOR_COMPONENT_G_BIT |
                    VK_COLOR_COMPONENT_B_BIT |
                    VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor =
                    VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstColorBlendFactor =
                    VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.colorBlendOp =
                    VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.srcAlphaBlendFactor =
                    VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstAlphaBlendFactor =
                    VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp =
                    VK_BLEND_OP_ADD; // Optional

            colorBlending.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional

            pipelineLayoutInfo.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
            pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

            VkResult result = vkCreatePipelineLayout(swapChain.getChoosenDevice(), &pipelineLayoutInfo, nullptr,
                                                     &pipelineLayout);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create pipeline layout!");
            }

            depthStencil.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.minDepthBounds = 0.0f; // Optional
            depthStencil.maxDepthBounds = 1.0f; // Optional
            depthStencil.stencilTestEnable = VK_FALSE;
            depthStencil.front = {}; // Optional
            depthStencil.back = {}; // Optional

            pipelineInfo.sType =
                    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = nullptr; // Optional
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional

            result = vkCreateGraphicsPipelines(swapChain.getChoosenDevice(), VK_NULL_HANDLE, 1,
                                               &pipelineInfo, nullptr, &graphicsPipeline);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create graphics pipeline!");
            }

        }

        uint32_t findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) {
            VkPhysicalDeviceMemoryProperties memProperties;
            auto device = deviceRepo->getDevice(*deviceRepo->getPerfectCriteriaMatcher().begin());
            vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) &&
                    (memProperties.memoryTypes[i].propertyFlags & properties) ==
                    properties) {
                    return i;
                }
            }

            throw std::runtime_error("failed to find suitable memory type!");
        }

        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                         VkSampleCountFlagBits numSamples, VkFormat format,
                         VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties, VkImage& image,
                         VkDeviceMemory& imageMemory) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = mipLevels;
            imageInfo.arrayLayers = 1;
            imageInfo.format = format;
            imageInfo.tiling = tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = usage;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.samples = numSamples;
            imageInfo.flags = 0; // Optional

            auto device = deviceRepo->getLogicalDevice(
                    *deviceRepo->getPerfectCriteriaMatcher().begin()
            );

            VkResult result = vkCreateImage(device, &imageInfo, nullptr, &image);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create image!");
            }

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(device, image, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                                       properties);
            if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to allocate image memory!");
            }

            vkBindImageMemory(device, image, imageMemory, 0);
        }

        VkImageView createImageView(VkImage image, VkFormat format,
                                    VkImageAspectFlags aspectFlags,
                                    uint32_t mipLevels, VkImageViewType type, int layerCount) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = image;
            viewInfo.viewType = type;
            viewInfo.format = format;
            viewInfo.subresourceRange.aspectMask = aspectFlags;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = mipLevels;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = layerCount;
            VkImageView imageView;

            auto device = deviceRepo->getLogicalDevice(
                    *deviceRepo->getPerfectCriteriaMatcher().begin()
            );

            VkResult result = vkCreateImageView(device, &viewInfo, nullptr,
                                                &imageView);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create image view!");
            }
            return imageView;
        }

        void createColorResources(VkSwapChain swapChain) {
            auto swapChainExtent = swapChain.getSwapChainExtent();
            auto swapChainImageFormat = swapChain.getSwapChainImageFormat().format;

            VkFormat colorFormat = swapChainImageFormat;
            createImage(swapChainExtent.width, swapChainExtent.height, 1,
                        VK_SAMPLE_COUNT_1_BIT, colorFormat, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        colorImage, colorImageMemory);
            colorImageView = createImageView(colorImage, colorFormat,
                                             VK_IMAGE_ASPECT_COLOR_BIT, 1,
                                             VK_IMAGE_VIEW_TYPE_2D, 1);
        }
         void createDepthResources(VkSwapChain swapChain) {
            auto swapChainExtent = swapChain.getSwapChainExtent();
            auto swapChainImageFormat = swapChain.getSwapChainImageFormat().format;


            VkFormat depthFormat = findDepthFormat();

            createImage(swapChainExtent.width, swapChainExtent.height, 1,
                        VK_SAMPLE_COUNT_1_BIT, depthFormat, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        depthImage, depthImageMemory);
            depthImageView = createImageView(depthImage, depthFormat,
                                             VK_IMAGE_ASPECT_DEPTH_BIT, 1,
                                             VK_IMAGE_VIEW_TYPE_2D, 1);

        }


        void createFrameBuffer(VkSwapChain swapChain){
            auto swapChainImageViews = swapChain.getSwapChainImageViews();
            auto swapChainExtent = swapChain.getSwapChainExtent();

            swapChainFramebuffers.resize(swapChainImageViews.size());
            for (size_t i = 0; i < swapChainImageViews.size(); i++) {
                std::array<VkImageView, 1> attachments = {
                        swapChainImageViews[i],
                };

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType =
                        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = renderPass;
                framebufferInfo.attachmentCount =
                        static_cast<uint32_t>(attachments.size());;
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = swapChainExtent.width;
                framebufferInfo.height = swapChainExtent.height;
                framebufferInfo.layers = 1;

                VkResult result = vkCreateFramebuffer(swapChain.getChoosenDevice(), &framebufferInfo, nullptr,
                                                      &swapChainFramebuffers[i]);
                if (result != VK_SUCCESS) {
                    Terminal::getInstance().printnl(result);
                    throw std::runtime_error("failed to create framebuffer!");
                }
            }
        }
    };

    class Object{
        vector<glm::vec3> vertex;
        vector<glm::vec3> indicies;
    public:
        void setVertex(vector<glm::vec3> vertex);
        void setIndicies(vector<glm::vec3> indicies);


    public:
        const vector<glm::vec3> &getVertex() const;
        const vector<glm::vec3> &getIndicies() const;
    };
    class Model{
        vector<Object> objects;
        vector<float> vertex;
        vector<int> indicies;

        VkVertexBuffer<float> *vertexBuffer;
        VkIndexBuffer<int> *indBuffer;

        int choosenDevice;
        VkPhyDevice *deviceRepository;

    public:
        void createBuffers(){
            this->copyData();

            vertexBuffer = new VkVertexBuffer<float>(vertex.size()*3,this->choosenDevice);
            indBuffer = new VkIndexBuffer<int>(indicies.size()*3,this->choosenDevice);

            vertexBuffer->setPhysicalDevice(deviceRepository);
            indBuffer->setPhysicalDevice(deviceRepository);



            vertexBuffer->setData(vertex);
            indBuffer->setData(indicies);

            vertexBuffer->create();
            indBuffer->create();
        }

    protected:
        int getVertexSize(){
            uint32_t  toRet =0;
            for(auto obj:objects)
                toRet += obj.getVertex().size()*3;
            return toRet;
        }

        int getIndSize(){
            uint32_t  toRet =0;
            for(auto obj:objects)
                toRet += obj.getIndicies().size()*3;
            return toRet;
        }

        void copyData(){
            vertex.resize(getVertexSize());
            indicies.resize(getIndSize());

            for(auto toAdd:objects){
                int i=0;
                for(auto vert:toAdd.getVertex()){
                    vertex[i++] = vert.x;
                    vertex[i++] = vert.y;
                    vertex[i++] = vert.z;
                }

                i=0;

                for(auto ind:toAdd.getIndicies()){
                    indicies[i++] = ind.x;
                    indicies[i++] = ind.y;
                    indicies[i++] = ind.z;
                }

            }
        }



    public:
        Model(){

        }

        void setDeviceRepository(VkPhyDevice * phydevice ){
            this->deviceRepository = phydevice;
        }
        void addObject(Object& object){
            objects.push_back(object);
        }

        void create(){
            copyData();
        }

        VkBuffer getVertexBuffers(){

           return vertexBuffer->getBuffer();
        };

        VkBuffer getIndexBuffers(){
            return indBuffer->getBuffer();
        };

        auto getIndicies(){return indicies;};
        auto getVerticies(){return vertex;};
    };
    class Render{
        VkPhyDevice *deviceRepository;
        VkCommandPool commandPool;
        VkPipeLine *pipeLine;
        VkSwapChain *swapChain;
        std::vector<VkCommandBuffer> commandBuffers;
        Model *model;

        //Synchronization objects
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        int choosen;

        const int MAX_FRAMES_IN_FLIGHT = 2;
        int currentFrame = 0;

    public:
        Render(VkPhyDevice *deviceRepo,VkPipeLine *pipeLine,VkSwapChain *swapChain,Model* model){
            this->pipeLine = pipeLine;

            deviceRepository = deviceRepo;
            choosen = *deviceRepo->getPerfectCriteriaMatcher().begin();

            this->swapChain = swapChain;
            this->model = model;
        }

        void createCommandPool() {
            auto queueFamilyIndices =  deviceRepository->getUniqueIndicies<vector<uint32_t>>(
                        choosen
                    );

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.queueFamilyIndex = queueFamilyIndices[0];
            poolInfo.flags = 0; // Optional

            VkResult result = vkCreateCommandPool(deviceRepository->getLogicalDevice(choosen), &poolInfo, nullptr, &commandPool);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create command pool!");
            }
        }



        void createCommandBuffers() {
            // Lesson 13
            auto frameBuffers = pipeLine->getSwapChainFramebuffers();

            commandBuffers.resize(frameBuffers.size());

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = commandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

            VkResult result = vkAllocateCommandBuffers(swapChain->getChoosenDevice(), &allocInfo,
                                                       commandBuffers.data());
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to allocate command buffers!");
            }

            // Lesson 22.5 --- Draw calls
            // This is where the commands that actually draw something on screen are!
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
                renderPassInfo.renderPass = pipeLine->getRenderPass();
                renderPassInfo.framebuffer = pipeLine->getSwapChainFramebuffers()[i];
                renderPassInfo.renderArea.offset = {0, 0};
                renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

                std::array<VkClearValue, 2> clearValues{};
                clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
                clearValues[1].depthStencil = {1.0f, 0};

                renderPassInfo.clearValueCount =
                        static_cast<uint32_t>(clearValues.size());;
                renderPassInfo.pClearValues = clearValues.data();

                vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                                     VK_SUBPASS_CONTENTS_INLINE);
                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  pipeLine->getGraphicsPipeline());

                auto vBuff = model->getVertexBuffers();
                auto iBuff = model->getIndexBuffers();

                VkBuffer vertexBuffers[] = {vBuff};
                VkBuffer indexBuffer = iBuff;

                VkDeviceSize offsets[] = {0};

                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

                vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0,
                                     VK_INDEX_TYPE_UINT32);
                /**vkCmdBindDescriptorSets(commandBuffers[i],
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipeLine->getPipelineLayout(), 0, 1, &descriptorSets[i],
                                        0, nullptr);**/

                vkCmdDrawIndexed(commandBuffers[i],
                                 static_cast<uint32_t>(model->getIndicies().size()), 1, 0, 0, 0);
                vkCmdEndRenderPass(commandBuffers[i]);
                if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                    throw std::runtime_error("failed to record command buffer!");
                }
            }
        }

        void createSyncObjects() {

            imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
            imagesInFlight.resize(swapChain->getSwapChainImages().size(), VK_NULL_HANDLE);

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                VkResult result1 = vkCreateSemaphore(swapChain->getChoosenDevice(), &semaphoreInfo, nullptr,
                                                     &imageAvailableSemaphores[i]);
                VkResult result2 = vkCreateSemaphore(swapChain->getChoosenDevice(), &semaphoreInfo, nullptr,
                                                     &renderFinishedSemaphores[i]);
                VkResult result3 = vkCreateFence(swapChain->getChoosenDevice(), &fenceInfo, nullptr,
                                                 &inFlightFences[i]);
                if (result1 != VK_SUCCESS ||
                    result2 != VK_SUCCESS ||
                    result3 != VK_SUCCESS) {
                    Terminal::getInstance().printnl(result1);
                    Terminal::getInstance().printnl(result2);
                    Terminal::getInstance().printnl(result3);
                    throw std::runtime_error("failed to create synchronization objects for a frame!!");
                }
            }
        }

        void drawFrame() {
            vkWaitForFences(swapChain->getChoosenDevice(), 1, &inFlightFences[currentFrame],
                            VK_TRUE, UINT64_MAX);

            uint32_t imageIndex;

            VkResult result = vkAcquireNextImageKHR(swapChain->getChoosenDevice(), swapChain->getSwapChain(), UINT64_MAX,
                                                    imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

            if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
                vkWaitForFences(swapChain->getChoosenDevice(), 1, &imagesInFlight[imageIndex],
                                VK_TRUE, UINT64_MAX);
            }
            imagesInFlight[imageIndex] = inFlightFences[currentFrame];

            //updateUniformBuffer(imageIndex); //todo remeber to update

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
            VkPipelineStageFlags waitStages[] =
                    {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
            VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            vkResetFences(swapChain->getChoosenDevice(), 1, &inFlightFences[currentFrame]);

            if (vkQueueSubmit(deviceRepository->graphicsQueue, 1, &submitInfo,
                              inFlightFences[currentFrame]) != VK_SUCCESS) {
                throw std::runtime_error("failed to submit draw command buffer!");
            }

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;

            VkSwapchainKHR swapChains[] = {swapChain->getSwapChain()};
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr; // Optional

            result = vkQueuePresentKHR(deviceRepository->presentQueue, &presentInfo);

            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        }

    };

}
#endif //ASS0_VULKANFRAMEWORK_H
