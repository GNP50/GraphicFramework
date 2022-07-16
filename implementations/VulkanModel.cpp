//
// Created by Pierluigi Carlino on 10/07/22.
//

#include "VulkanModel.h"

using namespace Vulkan;

void Render::createPipeline(VkSwapChain &swapChain) {

    fragShader = new VkFragmentShader(vertexPath);
    vertexShader = new VkVertexShader(fragmPath);

    fragShader->create(swapChain.getChoosenDevice());
    vertexShader->create(swapChain.getChoosenDevice());


    pipeline = new VkPipeLine(*fragShader, *vertexShader, deviceRepository);
    descrPool.createDescriptorPool(swapChain);
    pipeline->createPipelineWithPolyMode(swapChain, descrPool.getDescriptorSetLayout(),VK_POLYGON_MODE_POINT);


}

void Render::createCommandBuffers(VkCommandBuffer commandBuffer ) {

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeLine->getGraphicsPipeline());

    auto vBuff = model->getVertexBuffers();
    auto iBuff = model->getIndexBuffers();

    VkBuffer vertexBuffers[] = {vBuff};
    VkBuffer indexBuffer = iBuff;

    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);



    //vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(3), 1, 0, 0);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0,
                         VK_INDEX_TYPE_UINT16);
    //todo decide how to handle
    /**vkCmdBindDescriptorSets(commandBuffer,
                           VK_PIPELINE_BIND_POINT_GRAPHICS,
                           pipeLine->getPipelineLayout(), 0, 1, &descriptorSets[i],
                           0, nullptr);


   **/
    vkCmdDrawIndexed(commandBuffer,
                     static_cast<uint32_t>(model->getIndicies().size()), 1, 0, 0, 0);


}

