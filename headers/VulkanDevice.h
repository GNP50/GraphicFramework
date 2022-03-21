//
// Created by Pierluigi Carlino on 20/03/22.
//

#ifndef DB_VULKANDEVICE_H
#define DB_VULKANDEVICE_H

#include<GraphicsFramework.h>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <VulkanCommons.h>

class VkPhyDevice : public PhysicalDevice<
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
> {
protected:
    virtual bool isDevicesSuitable(vector<VkPhysicalDevice> devices) override;

    virtual bool isFeatureSupported(int offset) override;

    virtual bool isExtensionSupported(int offset, vector<VkExtensionProperties> requiredExtensions) override;

    virtual VkPhysicalDeviceFeatures getSupportedExtensions(int offset) override;

    virtual bool isSwapChainAdeguate(int offset, VkSurfaceKHR surface) override;

    virtual void initDevices(VkInstance instance) override;

    virtual SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) override;

    template<typename Criteria>
    void getFirstSuitable(Criteria criteria);

    virtual VkSwapchainKHR createSwapChain(int off) override;

public:
    VkPhyDevice(vector<const char *> deviceExts = vector<const char *>({
                                                                               VK_KHR_SWAPCHAIN_EXTENSION_NAME
                                                                       })) : PhysicalDevice(deviceExts) {
        initDevices(instance);
    }

    virtual QueueFamilyIndices findQueueFamilies(int off) override;

    virtual VkDevice createLogicalDevice(int off) override;

//swaps function valutate if i have to make generic
    virtual VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR> &availableFormats);

    virtual VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    virtual vector<string> getAllDevicesName() override;
};


#endif //DB_VULKANDEVICE_H
