//
// Created by Pierluigi Carlino on 02/07/22.
//

#ifndef DB_COMMONVULKANINCLUDES_H
#define DB_COMMONVULKANINCLUDES_H


#include<GraphicsFramework.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtx/io.hpp>


#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <map>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <typeindex>


#include <chrono>
#include <thread>

//todo decide if it have to been general
namespace Vulkan {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

};
#endif //DB_COMMONVULKANINCLUDES_H
