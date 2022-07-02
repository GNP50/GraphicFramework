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
        VkPhyDevice *deviceRepository;
        int choosenDevice = 0;

        VkSurfaceKHR getSurface() { deviceRepository->getSurface(); };

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
}
#endif //DB_VULKANSWAPCHAIN_H
