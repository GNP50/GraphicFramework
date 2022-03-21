//
// Created by Pierluigi Carlino on 20/03/22.
//
#include <VulkanDevice.h>

VkExtent2D VkPhyDevice::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
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


VkSwapchainKHR VkPhyDevice::createSwapChain(int off) {
    SwapChainSupportDetails swapChainSupport =
            querySwapChainSupport(this->devices[off],this->surface);
    VkSurfaceFormatKHR surfaceFormat =
            chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =
            chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(off);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};
    if (indices.graphicsFamily != indices.presentFamily) {
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

    VkResult result = vkCreateSwapchainKHR(this->createLogicalDevice(off), &createInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS) {
        Terminal::getInstance().print(result);
        throw std::runtime_error("failed to create swap chain!");
    }


    vkGetSwapchainImagesKHR(this->createLogicalDevice(off), swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(this->createLogicalDevice(off), swapChain, &imageCount,
                            swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
    return swapChain;
}

vector<string> VkPhyDevice::getAllDevicesName() {
    std::vector<std::string> toRet;
    VkPhysicalDeviceProperties prop;
    for(int i=0;i<devices.size();i++)
    {
        vkGetPhysicalDeviceProperties(devices[i],&prop);
        toRet.push_back(string(prop.deviceName));
    };
    return toRet;
}

VkSurfaceFormatKHR VkPhyDevice::chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VkPhyDevice::chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

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

VkDevice VkPhyDevice::createLogicalDevice(int off) {
    if (logicalDevices[off] != nullptr)
        return logicalDevices[off];

    QueueFamilyIndices indices = findQueueFamilies(off);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies =
            {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
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

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    logicalDevices[off] = device;

    return logicalDevices[off];
}

bool VkPhyDevice::isFeatureSupported(int offset){
    //todo implementation
    return false;
};

QueueFamilyIndices  VkPhyDevice::findQueueFamilies(int off) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(devices[off], &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(devices[off], &queueFamilyCount,
                                             queueFamilies.data());

    int i=0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(devices[off], i, surface,
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

SwapChainSupportDetails VkPhyDevice::querySwapChainSupport(VkPhysicalDevice device,VkSurfaceKHR surface) {
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
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(devices[offset],surface);
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
    this->logicalDevices = vector<VkDevice>(deviceCount);

    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    Terminal::getInstance().print("Physical devices found: ");
    Terminal::getInstance().printnl(deviceCount);
};

bool VkPhyDevice::isExtensionSupported(int offset,vector<VkExtensionProperties> requiredExtensions){
    QueueFamilyIndices indices = findQueueFamilies(offset);

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(devices[offset], nullptr,
                                         &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(devices[offset], nullptr,
                                         &extensionCount, availableExtensions.data());

    for (const auto extension : availableExtensions) {
        bool found = false;
        string first(extension.extensionName);
        for (const auto reqExtension: requiredExtensions) {
            string second(reqExtension.extensionName);
            if (first == second && reqExtension.specVersion == extension.specVersion) {
                found = true;
                break;
            }
        }
        if(!found)
            return false;
    }

    return false;
};



