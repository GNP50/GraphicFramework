//
// Created by Pierluigi Carlino on 10/07/22.
//

#include "VulkanModel.h"

using namespace Vulkan;

void Render::createPipeline(VkSwapChain &swapChain) {

    fragShader = new VkFragmentShader(fragmPath);
    vertexShader = new VkVertexShader(vertexPath);

    fragShader->create(swapChain.getChoosenDevice());
    vertexShader->create(swapChain.getChoosenDevice());


    pipeline = new VkPipeLine(*fragShader, *vertexShader, deviceRepository);
    descrPool->createDescriptorPool(swapChain);
    pipeline->setDescriptorSetLayout(descrPool->getLayouts());
    pipeline->createPipelineWithPolyMode(swapChain,VK_POLYGON_MODE_FILL);

    descrPool->allocateDescriptorSets(swapChain);

}

void Render::createCommandBuffers(VkCommandBuffer commandBuffer,int i) {

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline->getGraphicsPipeline());

    auto vBuff = model->getVertexBuffers();
    auto iBuff = model->getIndexBuffers();

    VkBuffer vertexBuffers[] = {vBuff};
    VkBuffer indexBuffer = iBuff;

    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0,
                         VK_INDEX_TYPE_UINT32);
    auto descriptorsets = descrPool->getDescriptorSets();


    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline->getPipelineLayout(), 0, 1, &descriptorsets.data()[i],
                            0, nullptr);



    vkCmdDrawIndexed(commandBuffer,
                     static_cast<uint32_t>(model->getIndicies().size()), 1, 0, 0, 0);


}

VkFragmentShader *Render::getFragShader() const {
    return fragShader;
}

VkVertexShader *Render::getVertexShader() const {
    return vertexShader;
}

