//
// Created by Pierluigi Carlino on 02/07/22.
//

#ifndef DB_VULKANSWAPCHAIN_H
#define DB_VULKANSWAPCHAIN_H
#include "commonVulkanIncludes.h"
#include "VulkanDevices.h"

namespace Vulkan{
    class VkSwapChain : public GenericSwapChain<
            VkSwapchainKHR ,
            VkSurfaceFormatKHR,
            SwapChainSupportDetails,
            VkExtent2D,
            VkImage,VkImageView> {

    private:
        std::shared_ptr<VkPhyDevice> deviceRepository;
        int choosenDevice = 0;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkImage colorImage = {};
        VkDeviceMemory colorImageMemory = {};
        VkImageView colorImageView = {};

        VkImage depthImage = {};
        VkDeviceMemory depthImageMemory{};
        VkImageView depthImageView{};

        VkAttachmentDescription depthAttachment = {};
        VkAttachmentDescription colorAttachment = {};
        VkAttachmentReference colorAttachmentRef = {};
        VkSubpassDescription subpass = {};
        VkRenderPass renderPass = {};

        VkSubpassDependency dependency = {};
        VkRenderPassCreateInfo renderPassInfo = {};


        VkSurfaceKHR getSurface() { deviceRepository->getSurface(); };
        VkAttachmentDescription colorAttachmentResolve{};

        VkPhysicalDevice getDevice(int off) { return deviceRepository->getDevice(off); };
    public:



        ~VkSwapChain(){
            if(isCreated){

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


        void setDeviceRepository(std::shared_ptr<VkPhyDevice> device) {
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

        std::shared_ptr<VkPhyDevice> getDeviceRepository(){return deviceRepository;}
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

            auto result = vkCreateSwapchainKHR(deviceRepository->getLogicalDevice(off), &createInfo, nullptr, &swapChain);
            Terminal::getInstance().printOnCondition("to create swap chain",result!=VK_SUCCESS,result);


            choosenDevice = off;
            isCreated = true;

            updateSwapImages();
            createColorResources();
            createDepthResources();
            createRenderPass();

            createImageView();
            createFrameBuffer();
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

                auto result = vkCreateImageView(deviceRepository->getLogicalDevice(choosenDevice), &createInfo, nullptr, &swapChainImageViews[i]);

                Terminal::getInstance().printOnCondition("to create image views",result!=VK_SUCCESS,result);

            }
        }

        VkSampleCountFlagBits getMaxUsableSampleCount(){
            return deviceRepository->getMaxUsableSampleCount(choosenDevice);
        }

        vector<VkFramebuffer> getFramwBuffers(){return this->swapChainFramebuffers;}

        void createFrameBuffer(){
            auto swapChainImageViews = this->getSwapChainImageViews();
            auto swapChainExtent = this->getSwapChainExtent();

            swapChainFramebuffers.resize(swapChainImageViews.size());
            for (size_t i = 0; i < swapChainImageViews.size(); i++) {
                std::array<VkImageView, 3> attachments = {
                        colorImageView,
                        depthImageView,
                        swapChainImageViews[i]
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

                VkResult result = vkCreateFramebuffer(getChoosenDevice(), &framebufferInfo, nullptr,
                                                      &swapChainFramebuffers[i]);

                Terminal::getInstance().printOnCondition("to create framebuffer",result!=VK_SUCCESS,result);


            }


        }

        void createColorResources() {
            VkFormat colorFormat = this->getSwapChainImageFormat().format;
            createImage(this->getSwapChainExtent().width, this->getSwapChainExtent().height, 1,
                        getMaxUsableSampleCount(), colorFormat, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        colorImage, colorImageMemory);
            colorImageView = createImageView(colorImage, colorFormat,
                                             VK_IMAGE_ASPECT_COLOR_BIT, 1,
                                             VK_IMAGE_VIEW_TYPE_2D, 1);
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

            auto device = deviceRepository->getLogicalDevice(
                    *deviceRepository->getPerfectCriteriaMatcher().begin()
            );

            VkResult result = vkCreateImage(device, &imageInfo, nullptr, &image);
            Terminal::getInstance().printOnCondition("to create image",result!=VK_SUCCESS,result);


            VkMemoryRequirements memRequirements;
             vkGetImageMemoryRequirements(device, image, &memRequirements);
            Terminal::getInstance().printOnCondition("to bind image memory",result!=VK_SUCCESS,result);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                                       properties);

            result = vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory);
            Terminal::getInstance().printOnCondition("to allocat image memory",result!=VK_SUCCESS,result);

            result = vkBindImageMemory(device, image, imageMemory, 0);
            Terminal::getInstance().printOnCondition("to bind image memory",result!=VK_SUCCESS,result);
        }

        VkRenderPass getRenderPass(){return renderPass;}


        uint32_t findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) {
            VkPhysicalDeviceMemoryProperties memProperties;
            auto device = deviceRepository->getDevice(*deviceRepository->getPerfectCriteriaMatcher().begin());
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

            auto device = deviceRepository->getLogicalDevice(
                    *deviceRepository->getPerfectCriteriaMatcher().begin()
            );

            VkResult result = vkCreateImageView(device, &viewInfo, nullptr,
                                                &imageView);
            Terminal::getInstance().printOnCondition("to create image view",result!=VK_SUCCESS,result);

            return imageView;
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

                vkGetPhysicalDeviceFormatProperties(deviceRepository->getDevice(
                        *deviceRepository->getPerfectCriteriaMatcher().begin()
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


        void createDepthResources() {
            VkFormat depthFormat = findDepthFormat();


            auto swapChainExtent = getSwapChainExtent();
            auto swapChainImageFormat = getSwapChainImageFormat().format;


            createImage(swapChainExtent.width, swapChainExtent.height, 1,
                        getMaxUsableSampleCount(), depthFormat, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        depthImage, depthImageMemory);
            depthImageView = createImageView(depthImage, depthFormat,
                                             VK_IMAGE_ASPECT_DEPTH_BIT, 1,
                                             VK_IMAGE_VIEW_TYPE_2D, 1);

            /**transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1);**/
        }
        void createRenderPass() {
            colorAttachmentResolve.format = getSwapChainImageFormat().format;
            colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentResolveRef{};
            colorAttachmentResolveRef.attachment = 2;
            colorAttachmentResolveRef.layout =
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            depthAttachment.format = findDepthFormat();
            depthAttachment.samples = getMaxUsableSampleCount();
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout =
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentReference depthAttachmentRef{};
            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout =
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            colorAttachment.format = getSwapChainImageFormat().format;
            colorAttachment.samples = getMaxUsableSampleCount();
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout =
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
            subpass.pDepthStencilAttachment = &depthAttachmentRef;
            subpass.pResolveAttachments = &colorAttachmentResolveRef;

            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            std::array<VkAttachmentDescription, 3> attachments =
                    {colorAttachment, depthAttachment,
                     colorAttachmentResolve};

            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;

            VkResult result = vkCreateRenderPass(getChoosenDevice(), &renderPassInfo, nullptr,
                                                 &renderPass);
            Terminal::getInstance().printOnCondition("to create render pass",result!=VK_SUCCESS,result);

        }


    };
}
#endif //DB_VULKANSWAPCHAIN_H
