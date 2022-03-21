//
// Created by Pierluigi Carlino on 20/03/22.
//

#ifndef DB_VULKANCOMMONS_H
#define DB_VULKANCOMMONS_H

#include<GraphicsFramework.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>



struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};



#endif //DB_VULKANCOMMONS_H
