//
// Created by Pierluigi Carlino on 20/03/22.
//

#ifndef DB_VULKANBUFFER_H
#define DB_VULKANBUFFER_H

#include <VulkanDevice.h>



template<typename BufferDataType>
class VkGenericBuffer:public Buffer<
        VkBuffer,
        VkBufferUsageFlags,
        VkMemoryPropertyFlags,
        VkDeviceMemory,
        VkDevice,VkPhyDevice,BufferDataType>{
protected:
    VkCommandPool  commandPool;
    int phyDeviceOff;

public:
    virtual void createBuffer() override{
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = static_cast<VkDeviceSize>(this->size);
        bufferInfo.usage = this->usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result =
                vkCreateBuffer(this->device, &bufferInfo, nullptr, &this->buffer);
        if (result != VK_SUCCESS) {
            Terminal::getInstance().print(result);
            Terminal::getInstance().print("\tfailed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(this->device, this->buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex =
                findMemoryType(memRequirements.memoryTypeBits, this->properties);

        result = vkAllocateMemory(this->device, &allocInfo, nullptr,
                                  &this->bufferMemory);
        if (result != VK_SUCCESS) {
            Terminal::getInstance().print(result);
            Terminal::getInstance().print("\tfailed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(this->device, this->buffer, this->bufferMemory, 0);
    };
    virtual uint32_t findMemoryType(uint32_t typeFilter,
                                    VkMemoryPropertyFlags properties) override{
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(this->physicalDevice.getDevice(phyDeviceOff), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    VkGenericBuffer(int size): Buffer<VkBuffer, VkBufferUsageFlags, VkMemoryPropertyFlags, VkDeviceMemory, VkDevice, VkPhyDevice,BufferDataType>(size),
                               VkPhyDevice(nullptr, vector<const char *>()) {

    }

    VkGenericBuffer(const VkGenericBuffer& toCopy): Buffer<VkBuffer, VkBufferUsageFlags, VkMemoryPropertyFlags, VkDeviceMemory, VkDevice, VkPhyDevice,BufferDataType>(toCopy),
                                                    VkPhyDevice(nullptr, vector<const char *>()) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional copyRegion.dstOffset = 0; // Optional
        copyRegion.size = this->size;
        vkCmdCopyBuffer(commandBuffer, toCopy.buffer, this->buffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    ~VkGenericBuffer(){
        vkDestroyBuffer(this->device, this->buffer, nullptr);
        vkFreeMemory(this->device, this->bufferMemory, nullptr);
    }

    VkCommandBuffer beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(this->device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        vkQueueSubmit(this->physicalDevice.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(this->physicalDevice.graphicsQueue);

        vkFreeCommandBuffers(this->device, commandPool, 1, &commandBuffer);
    }
    void createCommandPool() {
        QueueFamilyIndices queueFamilyIndices =
                this->physicalDevice.findQueueFamilies(phyDeviceOff);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = 0; // Optional

        VkResult result = vkCreateCommandPool(this->device, &poolInfo, nullptr, &commandPool);
        if (result != VK_SUCCESS) {
            Terminal::getInstance().printnl(result,BackGroudColor::red,ForeGroudColor::red);
            throw std::runtime_error("failed to create command pool!");
        }
    }


};

template<typename BufferDataType>
class VkStagingBuffer:VkGenericBuffer<BufferDataType> {
protected:

public:
    VkStagingBuffer(int size, vector<BufferDataType> data):VkGenericBuffer<BufferDataType>(size) {
        this->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        this->data = data;
    }

    virtual void create() override {
        this->createBuffer();
        void *ppData;
        vkMapMemory(this->device, this->bufferMemory, 0, this->bufferSize, 0, &ppData);

        memcpy(data, this->data.data(), (size_t) this->bufferSize);
        vkUnmapMemory(this->device, this->stagingBufferMemory);
    };
};

template<typename BufferDataType>
class VkVertexBuffer:VkGenericBuffer<BufferDataType>
{
public:

    VkVertexBuffer(int size):VkGenericBuffer<BufferDataType>(size){
        this->usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        this->properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    }
    virtual void create() override{
        VkStagingBuffer<BufferDataType> stagingBuffer;
        stagingBuffer.data = this->data;
        stagingBuffer.create();
        this->createBuffer();

        copyBuffer(stagingBuffer.buffer, this->buffer, this->bufferSize);
    };
};

template<typename BufferDataType>
class VkIndexBuffer:VkGenericBuffer<BufferDataType>
{
public:

    VkIndexBuffer(int size):VkGenericBuffer<BufferDataType>(size){
        this->usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                      VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        this->properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    }
    virtual void create() override{
        VkStagingBuffer<BufferDataType> stagingBuffer;
        stagingBuffer.data = this->data;
        stagingBuffer.create();
        this->createBuffer();

        copyBuffer(stagingBuffer.buffer, this->buffer, this->bufferSize);
    };
};

template<typename BufferDataType>
class VkUniformBuffer:VkGenericBuffer<BufferDataType>
{


public:

    VkUniformBuffer(int size):VkGenericBuffer<BufferDataType>(size){
        this->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    }
    virtual void create() override{
        VkStagingBuffer<BufferDataType> stagingBuffer;

        this->createBuffer();

        copyBuffer(stagingBuffer.buffer, this->buffer, this->bufferSize);
    };


};



/** logic to transfer

void createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(swapChainImages.size() * Scene.size());
    uniformBuffersMemory.resize(swapChainImages.size() * Scene.size());

    for (size_t i = 0; i < swapChainImages.size() * Scene.size(); i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     uniformBuffers[i], uniformBuffersMemory[i]);
    }

    bufferSize = sizeof(GlobalUniformBufferObject);

    globalUniformBuffers.resize(swapChainImages.size());
    globalUniformBuffersMemory.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     globalUniformBuffers[i], globalUniformBuffersMemory[i]);
    }

    bufferSize = sizeof(UniformBufferObject);
}


void createCommandBuffers() {
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo,
                                               commandBuffers.data());
    if (result != VK_SUCCESS) {
        PrintVkError(result);
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount =
                static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          PhongPipeline);

        for(int j = 0; j < Scene.size(); j++) {
            VkBuffer vertexBuffers[] = {Scene[j].MD.vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], Scene[j].MD.indexBuffer, 0,
                                 VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(commandBuffers[i],
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    PhongPipelineLayout, 0, 1,
                                    &PhongDescriptorSets[i + j * swapChainImages.size()],
                                    0, nullptr);

            vkCmdDrawIndexed(commandBuffers[i],
                             static_cast<uint32_t>(Scene[j].MD.indices.size()), 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffers[i]);
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject {
    alignas(16) glm::vec3 lightDir;
    alignas(16) glm::vec4 lightColor;
    alignas(16) glm::vec3 eyePos;
};


struct TextureData {
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    uint32_t mipLevels;
};

**/

#endif //DB_VULKANBUFFER_H
