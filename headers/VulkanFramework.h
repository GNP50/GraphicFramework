//
//
// Created by Pierluigi Carlino on 06/03/22.
//

#ifndef ASS0_VULKANFRAMEWORK_H
#define ASS0_VULKANFRAMEWORK_H

#include<GraphicsFramework.h>

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


    class VkSwapChain:public GenericSwapChain<
            VkImage,
            VkImage,
            SwapChainSupportDetails,
            VkExtent2D,
            VkImage>{

    private:
        VkPhyDevice deviceRepository;

        VkSurfaceKHR getSurface(){deviceRepository.getSurface();};
        VkPhysicalDevice getDevice(int off){deviceRepository.getDevice(off);};
    public:


        template<typename ...T>
        void create(T...);

        template<typename ...T>
        SwapChainSupportDetails querySwapChainSupport(T...);

        template<typename ...T>
        bool isSwapChainAdeguate(T...);

        template<VkPhysicalDevice,VkSurfaceKHR>
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





        template<int,VkSurfaceKHR>
        bool isSwapChainAdeguate(int offset, VkSurfaceKHR surface){
            auto device = this->getDevice(offset);

            SwapChainSupportDetails swapChainSupport = querySwapChainSupport<VkPhysicalDevice,VkSurfaceKHR>(device,surface);
            return !swapChainSupport.formats.empty() &&
                   !swapChainSupport.presentModes.empty();
        };

        void setDeviceRepository(VkPhyDevice device){
            this->deviceRepository = device;
        }
    };

    template<typename BufferDataType>
    class VkGenericBuffer : public Buffer<
            VkBuffer,
            VkBufferUsageFlags,
            VkMemoryPropertyFlags,
            VkDeviceMemory,
            VkDevice, VkPhyDevice *, BufferDataType> {
    protected:
        VkCommandPool commandPool;
        int phyDeviceOff;

    public:
        virtual void createBuffer() override {
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

            memcpy(this->dataPointer, this->data.data(), (size_t) this->bufferSize);
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

            stagingBuffer.data = this->data;
            stagingBuffer.create();
            this->createBuffer();

            copyBuffer(stagingBuffer.getBuffer(), this->buffer, this->bufferSize);
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

            stagingBuffer.data = this->data;
            stagingBuffer.create();
            this->createBuffer();

            copyBuffer(stagingBuffer.getBuffer(), this->buffer, this->bufferSize);
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

    class VkShader : public Shader {
    protected:
        VkShaderModuleCreateInfo createInfo;
        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo stageInfo;

    public:
        VkShader(string filename) : Shader(filename) {
            createInfo = {};
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

            stageInfo.module = shaderModule;

            //TODO think about a better way to handle this
            stageInfo.pName = "main";

        };

        template<VkDevice>
        void create(VkDevice device) {
            if (this->data.size() == 0)
                this->readFile();

            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = this->data.size();
            createInfo.pCode = reinterpret_cast<const uint32_t *>(this->data.data());

            if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shader module!");
            }


        }

        template<VkDevice>
        void destroy(VkDevice device) {
            vkDestroyShaderModule(device, shaderModule, nullptr);
        }


    };

    class VkFragmentShader : public VkShader {
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlending;

    public:
        VkFragmentShader(string filename,
                         VkPolygonMode polyMode = VK_POLYGON_MODE_FILL
        ) : VkShader(filename) {
            stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

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
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
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
        VkPipelineVertexInputStateCreateInfo vertexInputInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly;

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


    class VkPipeline {
    protected:
        vector<VkDynamicState> dynamicStates;
        VkPipelineDynamicStateCreateInfo dynamicState;
        VkPipelineLayoutCreateInfo pipelineLayoutInfo;
        VkPipelineLayout pipelineLayout;

        VkPipeline(vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_LINE_WIDTH
        }){
            this->dynamicStates = vector<VkDynamicState>(dynamicStates);

            dynamicState = {};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

            pipelineLayoutInfo = {};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0; // Optional
            pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
            pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        }

        template<VkDevice>
        void create(VkDevice device){
            if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }
        };

        template<VkDevice>
        void cleanUp(VkDevice device){
            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        };

    };
/** logic to transfer

void createUniformBuffers() {

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
}
#endif //ASS0_VULKANFRAMEWORK_H
