#include "VulkanDevices.h"

using namespace Vulkan;

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

    Terminal::getInstance().printOnCondition("to create logical device",result!=VK_SUCCESS,result);


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
