cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)

project(DB)


file(GLOB DB_SRC CONFIGURE_DEPENDS "headers/*.h" "implementations/*.cpp")


add_library(DB SHARED implementations/VulkanFramework.cpp)

execute_process (
        COMMAND brew --prefix glfw
        OUTPUT_VARIABLE baseGLFW
)

execute_process (
        COMMAND brew --prefix glm
        OUTPUT_VARIABLE baseGLM
)

execute_process (
        COMMAND whoami
        OUTPUT_VARIABLE username
)


set(VULKANLIBRARY /Users/${username}/VulkanSDK/1.3.204.0/macOS/lib/libvulkan.1.3.204.dylib)
string(REPLACE "\n" "" VULKANLIBRARY ${VULKANLIBRARY})

set(GLFWLIBRARY ${baseGLFW}/lib/libglfw.3.dylib)
string(REPLACE "\n" "" GLFWLIBRARY ${GLFWLIBRARY})

set(GLMLIBRARY ${baseGLM}/lib)
string(REPLACE "\n" "" GLMLIBRARY ${GLMLIBRARY})

set(VULKANINCLUDE /Users/${username}/VulkanSDK/1.3.204.0/macOS/include)
string(REPLACE "\n" "" VULKANINCLUDE ${VULKANINCLUDE})

set(GLFWINCLUDE ${baseGLFW}/include)
string(REPLACE "\n" "" GLFWINCLUDE ${GLFWINCLUDE})

set(GLMINCLUDE ${baseGLM}/include)
string(REPLACE "\n" "" GLMINCLUDE ${GLMINCLUDE})


MESSAGE(STATUS ${GLFWLIBRARY})
MESSAGE(STATUS ${GLMLIBRARY})
MESSAGE(STATUS ${VULKANLIBRARY})
MESSAGE(STATUS ${VULKANINCLUDE})
MESSAGE(STATUS ${GLFWINCLUDE})
MESSAGE(STATUS ${GLMINCLUDE})

include_directories (${GLFWINCLUDE})
include_directories (${GLMINCLUDE})
include_directories (${VULKANINCLUDE})
include_directories(headers)


target_link_libraries(${CMAKE_PROJECT_NAME} ${VULKANLIBRARY})
target_link_libraries(${CMAKE_PROJECT_NAME} ${GLFWLIBRARY})
link_directories(${GLMLIBRARY})

