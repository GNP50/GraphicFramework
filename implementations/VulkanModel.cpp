//
// Created by Pierluigi Carlino on 10/07/22.
//

#include "VulkanModel.h"

using namespace Vulkan;

void Render::createPipeline(VkSwapChain &swapChain) {

    fragShader = new VkFragmentShader("shaders/Fragment.spv");
    vertexShader = new VkVertexShader("shaders/Vertex.spv");

    fragShader->create(swapChain.getChoosenDevice());
    vertexShader->create(swapChain.getChoosenDevice());


    pipeline = new VkPipeLine(*fragShader, *vertexShader, deviceRepository);
    pipeline->createPipelineWithPolyMode(swapChain, VK_POLYGON_MODE_POINT);

    pipeline->createFrameBuffer(swapChain);
}
