//
// Created by Pierluigi Carlino on 02/07/22.
//

#ifndef DB_VULKANBUFFERS_H
#define DB_VULKANBUFFERS_H
#include "commonVulkanIncludes.h"
#include "VulkanDevices.h"
#include "VulkanSwapChain.h"
#include "stb_image.h"

namespace Vulkan{


    template<typename BufferDataType>
    class VkGenericBuffer : public Buffer<
            VkBuffer,
            VkBufferUsageFlags,
            VkMemoryPropertyFlags,
            VkDeviceMemory,
            VkDevice, VkPhyDevice, BufferDataType> {
    protected:
        VkCommandPool commandPool;
        int phyDeviceOff;

    public:
        virtual void createBuffer() override {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = static_cast<VkDeviceSize>(this->bufferSize);
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
                                        VkMemoryPropertyFlags properties) override {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(this->physicalDevice->getDevice(this->phyDeviceOff), &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) &&
                    (memProperties.memoryTypes[i].propertyFlags & properties) ==
                    properties) {
                    return i;
                }
            }

            throw std::runtime_error("failed to find suitable memory type!");
        }

        VkGenericBuffer(int size, int choosenDev) {
            this->size = size;
            this->phyDeviceOff = choosenDev;
            this->device = nullptr;
        }

        VkGenericBuffer(const VkGenericBuffer &toCopy) {
            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0; // Optional copyRegion.dstOffset = 0; // Optional
            copyRegion.size = this->size;
            vkCmdCopyBuffer(commandBuffer, toCopy.buffer, this->buffer, 1, &copyRegion);

            endSingleTimeCommands(commandBuffer);
        }

        ~VkGenericBuffer() {
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
            vkQueueSubmit(this->physicalDevice->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(this->physicalDevice->graphicsQueue);

            vkFreeCommandBuffers(this->device, commandPool, 1, &commandBuffer);
        }

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
            createCommandPool();
            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0; // Optional copyRegion.dstOffset = 0; // Optional
            copyRegion.size = size;
            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

            endSingleTimeCommands(commandBuffer);
            vkDestroyCommandPool(this->device,commandPool,nullptr);
        }

        void createCommandPool() {
            auto graphFamilyIndVec = this->physicalDevice->
                    template getUniqueIndicies<vector<uint32_t>>(this->phyDeviceOff);

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.queueFamilyIndex = graphFamilyIndVec[0];
            poolInfo.flags = 0; // Optional

            VkResult result = vkCreateCommandPool(this->device, &poolInfo, nullptr, &commandPool);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result, BackGroudColor::red, ForeGroudColor::red);
                throw std::runtime_error("failed to create command pool!");
            }
        }

        virtual void create() override {

        }

        void setPhysicalDevice(std::shared_ptr<VkPhyDevice> phyDevice){
            Buffer<
                    VkBuffer,
                    VkBufferUsageFlags,
                    VkMemoryPropertyFlags,
                    VkDeviceMemory,
                    VkDevice, VkPhyDevice, BufferDataType>::setPhysicalDevice(phyDevice);
            setLogicalDevice(
                    *phyDevice->getPerfectCriteriaMatcher().begin()
            );
        }
        void setLogicalDevice() {
            setLogicalDevice(this->phyDeviceOff);
        }
        void setLogicalDevice(int off) {
            this->phyDeviceOff = off;
            this->device = this->physicalDevice->getLogicalDevice(off);
        }

    };

    template<typename BufferDataType>
    class VkStagingBuffer : public VkGenericBuffer<BufferDataType> {
    protected:

    public:
        VkStagingBuffer(int size, int choosed, vector<BufferDataType> data) : VkGenericBuffer<BufferDataType>(size,
                                                                                                              choosed) {
            this->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            this->data = data;
        }

        virtual void create() override {
            this->createBuffer();

            void *ppData;
            vkMapMemory(this->device, this->bufferMemory, 0, this->bufferSize, 0, &ppData);

            memcpy(ppData,this->data.data(), (size_t) this->bufferSize);
            vkUnmapMemory(this->device, this->bufferMemory);
        };

        VkBuffer getBuffer() { return this->buffer; };
    };

    template<typename BufferDataType>
    class VkVertexBuffer : public VkGenericBuffer<BufferDataType> {
    public:

        VkVertexBuffer(int size, int choosenDev) : VkGenericBuffer<BufferDataType>(size, choosenDev) {
            this->usage =  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        }

        virtual void create() override {
            VkStagingBuffer<BufferDataType> stagingBuffer(this->size, this->phyDeviceOff,
                                                          vector<BufferDataType>(this->size));

            stagingBuffer.setData(this->data);
            stagingBuffer.setPhysicalDevice(this->physicalDevice);


            stagingBuffer.create();
            this->createBuffer();

             void *ppData;
            vkMapMemory(this->device, this->bufferMemory, 0, this->bufferSize, 0, &ppData);

            memcpy(ppData,this->data.data(), (size_t) this->bufferSize);
            vkUnmapMemory(this->device, this->bufferMemory);

            this->copyBuffer(stagingBuffer.getBuffer(), this->buffer, this->bufferSize);
        };
    };

    template<typename BufferDataType>
    class VkIndexBuffer : public VkGenericBuffer<BufferDataType> {
    public:

        VkIndexBuffer(int size, int choosenDev) : VkGenericBuffer<BufferDataType>(size, choosenDev) {
            this->usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        }

        virtual void create() override {
            VkStagingBuffer<BufferDataType> stagingBuffer(this->size, this->phyDeviceOff,
                                                          vector<BufferDataType>(this->size));

            stagingBuffer.setData(this->data);
            stagingBuffer.setPhysicalDevice(this->physicalDevice);

            stagingBuffer.create();
            this->createBuffer();

            this->copyBuffer(stagingBuffer.getBuffer(), this->buffer, this->bufferSize);
        };
    };

    template<typename BufferDataType>
    class VkUniformBuffer : public VkGenericBuffer<BufferDataType> {


    public:

        VkUniformBuffer(int size, int choosenDev) : VkGenericBuffer<BufferDataType>(size, choosenDev) {
            this->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        }

        virtual void create() override {
            VkStagingBuffer<BufferDataType> stagingBuffer(this->size, this->phyDeviceOff,
                                                          vector<BufferDataType>(this->size));

            this->createBuffer();

            this->copyBuffer(stagingBuffer.getBuffer(), this->buffer, this->bufferSize);
        };


    };


    template<typename UniformType>
    class DescriptorSet{
    protected:
        std::shared_ptr<VkDescriptorPool> descriptorPool;


        VkDescriptorSetAllocateInfo allocInfo{};
        VkDescriptorSetLayout descriptorLayout {};
        vector<VkUniformBuffer<UniformType>> descriptorSets;
        vector<VkGenericBuffer<UniformType>> buffers;

        int bufferSize;
        int binding;
        const VkDescriptorType type;


    public:
        DescriptorSet(const VkDescriptorType type):type(type){

        }

        void createDescriptorSet(VkSwapChain &swapchain){
            bufferSize = sizeof(UniformType)*swapchain.getSwapChainImages().size();
            createAllocateInfo(swapchain);
        };

        const VkDescriptorType getType(){return type;}
        void setDescriptorPool(std::shared_ptr<VkDescriptorPool> descriptorPool){
            this->descriptorPool = descriptorPool;
        }

        void setBindings(int i){
            binding = i;
        }

        void createAllocateInfo(VkSwapChain &swapchain){
            auto swapChainImages = swapchain.getSwapChainImages();


            std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size());

            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = *descriptorPool.get();
            allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
            allocInfo.pSetLayouts = layouts.data();

            descriptorSets.resize(swapChainImages.size());

            VkResult result = vkAllocateDescriptorSets(swapchain.getChoosenDevice(), &allocInfo,
                                                       descriptorSets.data());
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to allocate descriptor sets!");
            }
        }


    };

    class DescriptorPool{
        std::shared_ptr<VkDescriptorPool> descriptorPool;
        std::vector<VkDescriptorPoolSize> poolSizes{};
        VkDescriptorPoolCreateInfo poolInfo{};


        std::shared_ptr<VkPhyDevice> device;
        std::shared_ptr<VkSwapChain> swapchain;

        int added = 0;

        DescriptorPool(shared_ptr<VkSwapChain> swapchain,
                       shared_ptr<VkPhyDevice> device):
                        swapchain(swapchain),
                        device(device){

        }

        ~DescriptorPool(){
            vkDestroyDescriptorPool(swapchain->getChoosenDevice(), *descriptorPool, nullptr);
        }

        template<typename DescriptorType>
        void addDescriptorSet(DescriptorSet<DescriptorType> &descriptorSet){
            auto swapChainImages = swapchain->getSwapChainImages();
            VkDescriptorPoolSize toAdd{};

            toAdd.type = descriptorSet.getType();
            toAdd.descriptorCount = static_cast<uint32_t>(swapChainImages.size());

            poolSizes.push_back(toAdd);
            descriptorSet.setDescriptorPool(descriptorPool);
            descriptorSet.setBindings(added);
            added++;
        }
        void createDescriptorPool(VkSwapChain &swapchain) {
            auto swapChainImages = swapchain.getSwapChainImages();


            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();
            poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size() * (0 + 2));

            VkResult result = vkCreateDescriptorPool(swapchain.getChoosenDevice(), &poolInfo, nullptr,
                                                     descriptorPool.get());
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create descriptor pool!");
            }
        }


    };

    struct UniformBufferObject {
        alignas(16) glm::mat4 mvpMat;
        alignas(16) glm::mat4 mMat;
        alignas(16) glm::mat4 nMat;
    };

    template<typename GlobalUniform>
    class GlobalUniformBuffer:public DescriptorSet<GlobalUniform>,VkGenericBuffer<GlobalUniform> {

        GlobalUniformBuffer(int size, int choosenDev) : GlobalUniformBuffer<GlobalUniform>(size, choosenDev) {
            this->usage =  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        }

        virtual void create() override {
            this->createBuffer();
            void *ppData;
            vkMapMemory(this->device, this->bufferMemory, 0, this->bufferSize, 0, &ppData);

            memcpy(ppData,this->data.data(), (size_t) this->bufferSize);
            vkUnmapMemory(this->device, this->bufferMemory);

        };

        void createDescriptorSet(VkSwapChain &swapchain) {
            DescriptorSet<GlobalUniform>::createDescriptorSet(swapchain);
            auto swapChainImages = swapchain.getSwapChainImages();
            for (size_t k = 0; k < swapChainImages.size(); k++) {




                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = this->buffers[k].getBuffer();
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(GlobalUniform);

                std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
                descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[0].dstSet = this->descriptorSets[k];
                descriptorWrites[0].dstBinding = this->binding;
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[0].descriptorCount =  1;
                descriptorWrites[0].pBufferInfo = &bufferInfo;


                //todo understand where to call
                vkUpdateDescriptorSets(swapchain.getChoosenDevice(),
                                       static_cast<uint32_t>(descriptorWrites.size()),
                                       descriptorWrites.data(), 0, nullptr);
            }

        }
    };




    struct TextureData {
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
        uint32_t mipLevels;
    };



template<TextureData>
class TextureDescriptorSet:public DescriptorSet<TextureData>,public VkGenericBuffer<TextureData>{

    TextureData tx;
    const char* filename;

    VkImageCreateInfo imageInfo{};

    TextureDescriptorSet(const char* filename):filename(filename){
        this->usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout,
                               uint32_t mipLevels, int layersCount) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            //todo understand what to do with this line

            //if (hasStencilComponent(format)) {
            //    barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            //}
        }else{
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layersCount;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
                   newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }
        vkCmdPipelineBarrier(commandBuffer,
                             sourceStage, destinationStage, 0,
                             0, nullptr, 0, nullptr, 1, &barrier);

        endSingleTimeCommands(commandBuffer);
    }

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t
    width, uint32_t height, int layerCount) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(commandBuffer);
    }

    virtual void create() override{
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(filename, &texWidth, &texHeight,
                                    &texChannels, STBI_rgb_alpha);
        if (!pixels) {
            std::cout << filename << "\n";
            throw std::runtime_error("failed to load texture image!");
        }
        std::cout << filename << " -> size: " << texWidth
                  << "x" << texHeight << ", ch: " << texChannels <<"\n";

        VkDeviceSize imageSize = texWidth * texHeight * 4;
        tx.mipLevels = static_cast<uint32_t>(std::floor(
                std::log2(std::max(texWidth, texHeight)))) + 1;

        VkStagingBuffer<TextureData> stagingBuffer(this->size, this->phyDeviceOff,
                                                      vector<TextureData>(this->size));

        stagingBuffer.setData(this->data);
        stagingBuffer.setPhysicalDevice(this->physicalDevice);


        stagingBuffer.create();

        stbi_image_free(pixels);

        createImage(texWidth, texHeight, tx.mipLevels,
                    VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_TILING_OPTIMAL, tx.textureImage,tx.textureImageMemory);

        transitionImageLayout(tx.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                              VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, tx.mipLevels, 1);
        copyBufferToImage(stagingBuffer, tx.textureImage,
                          static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1);

        generateMipmaps(tx.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                        texWidth, texHeight, tx.mipLevels, 1);


    }

    void generateMipmaps(VkImage image, VkFormat imageFormat,
                         int32_t texWidth, int32_t texHeight,
                         uint32_t mipLevels, int layerCount) {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat,
                                            &formatProperties);

        if (!(formatProperties.optimalTilingFeatures &
              VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layerCount;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                 0, nullptr, 0, nullptr,
                                 1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = layerCount;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1,
                                   mipHeight > 1 ? mipHeight/2:1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = layerCount;

            vkCmdBlitImage(commandBuffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &blit, VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                 0, nullptr, 0, nullptr,
                                 1, &barrier);
            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr, 0, nullptr,
                             1, &barrier);

        endSingleTimeCommands(commandBuffer);
    }

    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                     VkSampleCountFlagBits numSamples, VkFormat format,
                     VkImageTiling tiling, VkImage& image,
                     VkDeviceMemory& imageMemory) {
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = numSamples;
        imageInfo.flags = 0; // Optional

        VkResult result = vkCreateImage(device, &imageInfo, nullptr, &image);
        if (result != VK_SUCCESS) {
            Terminal::getInstance().printnl(result);
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                                   properties);
        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, imageMemory, 0);
    }

    void createDescriptorSet(VkSwapChain swapchain){
        DescriptorSet<TextureData>::createDescriptorSet(swapchain);

        auto swapChainImages = swapchain.getSwapChainImages();

        std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(),
                                                   descriptorLayout);
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = *descriptorPool.get();
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(swapChainImages.size());

        VkResult result = vkAllocateDescriptorSets(device, &allocInfo,
                                                   descriptorSets.data());
        if (result != VK_SUCCESS) {
            Terminal::getInstance().printnl(result);
            throw std::runtime_error("failed to allocate Text descriptor sets!");
        }

        for (size_t k = 0; k < swapChainImages.size(); k++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = descriptorSets[k];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(TextureData);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = tx.textureImageView;
            imageInfo.sampler = tx.textureSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[k];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount =  1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[k];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType =
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device,
                                   static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(), 0, nullptr);
        }
    }
};
}
#endif //DB_VULKANBUFFERS_H
