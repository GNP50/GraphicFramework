//
// Created by Pierluigi Carlino on 02/07/22.
//

#ifndef DB_VULKANBUFFERS_H
#define DB_VULKANBUFFERS_H
#include "commonVulkanIncludes.h"
#include "VulkanDevices.h"
#include "VulkanSwapChain.h"

#include <stb_image.h>

#include "string.h"

namespace Vulkan {


    template<typename BufferDataType>
    class VkGenericBuffer : public Buffer<
            VkBuffer,
            VkBufferUsageFlags,
            VkMemoryPropertyFlags,
            VkDeviceMemory,
            VkDevice, VkPhyDevice, BufferDataType> {
    protected:
        bool created = false;
        VkCommandPool commandPool;
        int phyDeviceOff;
        VkBufferCreateInfo create_info{};
    public:
        const VkBufferCreateInfo &getCreateInfo() const {
            return create_info;
        }

        void setCreateInfo(const VkBufferCreateInfo &createInfo) {
            create_info = createInfo;
        }

    public:
        virtual void createBuffer() override {
            if (created)
                return;

            create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            create_info.size = static_cast<VkDeviceSize>(this->bufferSize);
            create_info.usage = this->usage;
            create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VkResult result =
                    vkCreateBuffer(this->device, &create_info, nullptr, &this->buffer);

            Terminal::getInstance().printOnCondition("to create buffer", result != VK_SUCCESS, result);


            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(this->device, this->buffer, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex =
                    findMemoryType(memRequirements.memoryTypeBits, this->properties);

            result = vkAllocateMemory(this->device, &allocInfo, nullptr,
                                      &this->bufferMemory);


            Terminal::getInstance().printOnCondition("to allocate buffer memory!", result != VK_SUCCESS, result);


            result = vkBindBufferMemory(this->device, this->buffer, this->bufferMemory, 0);
            Terminal::getInstance().printOnCondition("to bind buffer memory!", result != VK_SUCCESS, result);

            created = true;
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


        ~VkGenericBuffer() {

            /** todo handle better (create a tresh class where put lambda to destroy everything at the end)if(this->device !=NULL && created){
                vkDestroyBuffer(this->device, this->buffer, nullptr);
                vkFreeMemory(this->device, this->bufferMemory, nullptr);

            }**/
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
            vkDestroyCommandPool(this->device, commandPool, nullptr);
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
            this->createBuffer();

            void *ppData;
            vkMapMemory(this->device, this->bufferMemory, 0, this->bufferSize, 0, &ppData);

            memcpy(ppData, this->data.data(), (size_t) this->bufferSize);
            vkUnmapMemory(this->device, this->bufferMemory);
        }

        void setPhysicalDevice(std::shared_ptr<VkPhyDevice> phyDevice) {
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

        ~VkStagingBuffer(){
            vkDestroyBuffer(this->device, getBuffer(), nullptr);
            vkFreeMemory(this->device, this->bufferMemory, nullptr);
        }

        virtual void setBufferSize(uint64_t size){
            this->bufferSize = size;
        }

        virtual void create() override {
            this->createBuffer();

            void *ppData;
            vkMapMemory(this->device, this->bufferMemory, 0, this->bufferSize, 0, &ppData);

            memcpy(ppData, this->data.data(), (size_t) this->bufferSize);
            vkUnmapMemory(this->device, this->bufferMemory);
        };

        VkBuffer getBuffer() { return this->buffer; };
    };

    template<typename BufferDataType>
    class VkVertexBuffer : public VkGenericBuffer<BufferDataType> {
    public:

        VkVertexBuffer(int size, int choosenDev) : VkGenericBuffer<BufferDataType>(size, choosenDev) {
            this->usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
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

            memcpy(ppData, this->data.data(), (size_t) this->bufferSize);
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
            this->usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        }

        virtual void create() override {

            this->createBuffer();
            void *ppData;
            vkMapMemory(this->device, this->bufferMemory, 0, this->bufferSize, 0, &ppData);

            memcpy(ppData, this->data.data(), (size_t) this->bufferSize);
            vkUnmapMemory(this->device, this->bufferMemory);
        };


    };



    struct TextureData {
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
        uint32_t mipLevels;
    };

    template<typename BufferDataType>
    class VkTextureMapper : public VkGenericBuffer<BufferDataType> {
        int texWidth, texHeight, texChannels;
        TextureData textData;
        string path;

    public:


        VkTextureMapper(int size, int choosenDev,string path) : VkGenericBuffer<BufferDataType>(size, choosenDev),path(path) {
            this->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            this->properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        }

        const TextureData &getTextureData(){return textData;}

        virtual void create() override {
            this->createCommandPool();
            createTextureImage();
            createImageView(VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_ASPECT_COLOR_BIT,VK_IMAGE_VIEW_TYPE_2D, 1);
            createTextureSampler();

        };

        void createTextureImage() {
            stbi_uc *pixels = stbi_load(path.c_str(), &texWidth, &texHeight,
                                        &texChannels, STBI_rgb_alpha);
            if (stbi_failure_reason()) std::cout << stbi_failure_reason();
            Terminal::getInstance().printOnCondition("to load pixels for texture " + path,pixels == NULL,"");


            VkDeviceSize imageSize = texWidth * texHeight * 4;
            textData.mipLevels = static_cast<uint32_t>(std::floor(
                    std::log2(std::max(texWidth, texHeight)))) + 1;

            //create staging buffer
            VkStagingBuffer<stbi_uc> stagingBuffer(imageSize, this->phyDeviceOff,
                                                          vector<stbi_uc>(imageSize));

           vector<stbi_uc> data(pixels,pixels+imageSize*sizeof(stbi_uc));
            stagingBuffer.setData(data);
            stagingBuffer.setPhysicalDevice(this->physicalDevice);


            stagingBuffer.setBufferSize(imageSize);
            stagingBuffer.create();


            createImage(
                        VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                                 VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            transitionImageLayout(textData.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                                  VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textData.mipLevels, 1);
            copyBufferToImage(stagingBuffer.getBuffer(), textData.textureImage,
                              static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1);

            generateMipmaps(textData.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                            texWidth, texHeight, textData.mipLevels, 1);

            stbi_image_free(pixels);

        };

        void generateMipmaps(VkImage image, VkFormat imageFormat,
                             int32_t texWidth, int32_t texHeight,
                             uint32_t mipLevels, int layerCount) {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(this->physicalDevice->getDevice(this->phyDeviceOff), imageFormat,
                                                &formatProperties);

            Terminal::getInstance().printOnCondition("to load texture linear blitting " ,!(formatProperties.optimalTilingFeatures &
                                                                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT),"");


            VkCommandBuffer commandBuffer = this->beginSingleTimeCommands();

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
                blit.srcOffsets[0] = {0, 0, 0};
                blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = layerCount;
                blit.dstOffsets[0] = {0, 0, 0};
                blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1,
                                      mipHeight > 1 ? mipHeight / 2 : 1, 1};
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

            this->endSingleTimeCommands(commandBuffer);
        }

        void createImage(VkSampleCountFlagBits numSamples, VkFormat format,
                         VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = texWidth;
            imageInfo.extent.height = texHeight;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels =textData.mipLevels;
            imageInfo.arrayLayers = 1;
            imageInfo.format = format;
            imageInfo.tiling = tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = usage;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.samples = numSamples;
            imageInfo.flags = 0; // Optional

            VkResult result = vkCreateImage(this->device, &imageInfo, nullptr, &textData.textureImage);

            Terminal::getInstance().printOnCondition("to create image",result != VK_SUCCESS,result);

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(this->device, textData.textureImage, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits,
                                                       properties);


            result = vkAllocateMemory(this->device, &allocInfo, nullptr, &textData.textureImageMemory);
            Terminal::getInstance().template printOnCondition("to create allocate image memory ",result != VK_SUCCESS,result);

            vkBindImageMemory(this->device, textData.textureImage, textData.textureImageMemory, 0);
        }

        bool hasStencilComponent(VkFormat format) {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
                   format == VK_FORMAT_D24_UNORM_S8_UINT;
        }

        void transitionImageLayout(VkImage image, VkFormat format,
                                   VkImageLayout oldLayout, VkImageLayout newLayout,
                                   uint32_t mipLevels, int layersCount) {
            VkCommandBuffer commandBuffer = this->beginSingleTimeCommands();

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;

            if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

                if (this->hasStencilComponent(format)) {
                    barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
            } else {
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

            this->endSingleTimeCommands(commandBuffer);
        }

        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t
        width, uint32_t height, int layerCount) {
            VkCommandBuffer commandBuffer = this->beginSingleTimeCommands();

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

            this->endSingleTimeCommands(commandBuffer);
        }

        void createTextureSampler() {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = 16;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = static_cast<float>(textData.mipLevels);

            VkResult result = vkCreateSampler(this->device, &samplerInfo, nullptr,
                                              &textData.textureSampler);
            Terminal::getInstance().printOnCondition("to create texture sampler!",result!=VK_SUCCESS,result);

            }

        void createImageView(VkFormat format,
                                    VkImageAspectFlags aspectFlags, VkImageViewType type, int layerCount) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = textData.textureImage;
            viewInfo.viewType = type;
            viewInfo.format = format;
            viewInfo.subresourceRange.aspectMask = aspectFlags;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = textData.mipLevels;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = layerCount;

            VkResult result = vkCreateImageView(this->device, &viewInfo, nullptr,
                                                &textData.textureImageView);
            Terminal::getInstance().printOnCondition(" to create image view",result!=VK_SUCCESS,result);


        }

    };


    template<typename UniformType>
    class DescriptorSet {
    protected:
        VkDescriptorSetLayoutBinding layoutBinding;
        const VkDescriptorType type;
        const VkStructureType write_type;
        VkDescriptorBufferInfo buffer_info = {};
        VkDescriptorImageInfo imageInfo = {};
    public:
        const VkDescriptorImageInfo &getImageInfo() const {
            return imageInfo;
        }

        void setImageInfo(const VkDescriptorImageInfo &imageInfo) {
            DescriptorSet::imageInfo = imageInfo;
        }

    protected:
        int binding = -1;
        vector<VkBuffer> buffers;
    public:
        const vector<VkBuffer> &getBuffers() const {
            return buffers;
        }

    public:

        const VkStructureType &getWriteType() const {
            return write_type;
        }

        const VkDescriptorBufferInfo &getBufferInfo() const {
            return buffer_info;
        }

        const VkDescriptorSetLayoutBinding &getLayoutBinding() const {
            return layoutBinding;
        }


    public:
        DescriptorSet(const VkDescriptorType type,
                      const VkStructureType write_type
        ) : type(type), write_type(write_type) {
            layoutBinding.descriptorCount = 1;
            layoutBinding.descriptorType = type;
            // we use it from the vertex shader
            layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        }


        const VkDescriptorType getType() { return type; }


        void setBindings(int bind) {
            this->binding = bind;
            layoutBinding.binding = bind;

        }

        void setStage(VkShaderStageFlags stage){layoutBinding.stageFlags = stage;}
    };

    class DescriptorPool {

        VkDescriptorPool descriptorPool{};
        std::vector<VkDescriptorPoolSize> poolSizes{};
        VkDescriptorPoolCreateInfo poolInfo{};
        VkDescriptorSetLayout descriptorSetLayout = {};


        std::shared_ptr<VkPhyDevice> device;
        std::shared_ptr<VkSwapChain> swapchain;

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        std::vector<vector<std::shared_ptr<VkDescriptorBufferInfo>>> createInfos;
        std::vector<vector<std::shared_ptr<VkDescriptorBufferInfo>>> imageInfos;

        vector<VkDescriptorSetLayoutBinding> bindings;
        vector<VkDescriptorSetLayout> layouts;
        vector<VkDescriptorSet> descriptorSets;

    public:
        const vector<VkDescriptorSet> &getDescriptorSets() const {
            return descriptorSets;
        }

    private:

        int added = 0;


        void createDescriptorSetLayout(VkSwapChain &swapChain) {

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());

            layoutInfo.pBindings = bindings.data();

            VkResult result = vkCreateDescriptorSetLayout(swapChain.getChoosenDevice(), &layoutInfo,
                                                          nullptr, &descriptorSetLayout);

            Terminal::getInstance().printOnCondition("to create description set layout", result != VK_SUCCESS, result);


        }


    public:
        ~DescriptorPool() {
            vkDestroyDescriptorPool(swapchain->getChoosenDevice(), descriptorPool, nullptr);
        }

        DescriptorPool(shared_ptr<VkSwapChain> swapchain,
                       shared_ptr<VkPhyDevice> device) :
                swapchain(swapchain),
                device(device) {
            descriptorPool = {};
        }

        template<typename DescriptorType>
        void addDescriptorSet(DescriptorSet<DescriptorType> &descriptorSet) {
            auto swapChainImages = swapchain->getSwapChainImages();
            VkDescriptorPoolSize toAdd{};

            toAdd.type = descriptorSet.getType();
            toAdd.descriptorCount = static_cast<uint32_t>(swapChainImages.size());

            poolSizes.push_back(toAdd);

            descriptorSet.setBindings(added);

            bindings.push_back(descriptorSet.getLayoutBinding());

            VkWriteDescriptorSet toPush{};

            toPush.sType = descriptorSet.getWriteType();
            toPush.dstBinding = added;
            toPush.dstArrayElement = 0;
            toPush.descriptorType = descriptorSet.getType();
            toPush.descriptorCount = 1;

            if(toPush.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                VkDescriptorBufferInfo *createInfo = new VkDescriptorBufferInfo{};
                memcpy(createInfo, &descriptorSet.getBufferInfo(), sizeof(VkDescriptorBufferInfo));

                auto buffers = vector<std::shared_ptr<VkDescriptorBufferInfo>>();
                auto returnedBuff = descriptorSet.getBuffers();
                for (int i = 0; i < returnedBuff.size(); i++) {
                    auto toAdd = std::make_shared<VkDescriptorBufferInfo>(*createInfo);
                    toAdd->buffer = returnedBuff[i];
                    buffers.push_back(toAdd);
                }

                createInfos.push_back(buffers);

                toPush.pBufferInfo = createInfo;
            }else if(toPush.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
                toPush.pImageInfo = &descriptorSet.getImageInfo();
            }
            descriptorWrites.push_back(toPush);

            added++;
            Terminal::getInstance().printOnCondition("description set layout added", false, "");
        }

        void createDescriptorPool(VkSwapChain &swapchain) {
            auto swapChainImages = swapchain.getSwapChainImages();


            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();
            poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size() * poolSizes.size());


            VkResult result = vkCreateDescriptorPool(swapchain.getChoosenDevice(), &poolInfo, nullptr,
                                                     &descriptorPool);


            Terminal::getInstance().printOnCondition("to create description pool", result != VK_SUCCESS, result);

            createDescriptorSetLayout(swapchain);
        }

        void allocateDescriptorSets(VkSwapChain &swapChain) {
            auto swapChainImages = swapChain.getSwapChainImages();
            layouts.resize(swapChainImages.size(), descriptorSetLayout);

            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
            allocInfo.pSetLayouts = layouts.data();

            descriptorSets.resize(layouts.size());
            auto result = vkAllocateDescriptorSets(swapChain.getChoosenDevice(), &allocInfo,
                                                   descriptorSets.data());

            Terminal::getInstance().printOnCondition("to allocate description set layout", result != VK_SUCCESS,
                                                     result);


            for (int j = 0; j < swapChainImages.size(); j++) {
                int buffFound = 0;
                for (int i = 0; i < added; i++) {
                    descriptorWrites[i].dstSet = descriptorSets[j];
                    if(descriptorWrites[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                        descriptorWrites[i].pBufferInfo = createInfos[buffFound++][j].get();



                }
                vkUpdateDescriptorSets(swapChain.getChoosenDevice(), static_cast<uint32_t>(descriptorWrites.size()),
                                       descriptorWrites.data(), 0, nullptr);
            }

        }


    public:
        VkDescriptorSetLayout getLayout(int binding) {
            return this->layouts[binding];
        }

        VkDescriptorSetLayout &getLayouts() {
            return descriptorSetLayout;
        }

    };

    struct UniformBufferObject {
        alignas(16) glm::mat4 mvpMat;
        alignas(16) glm::mat4 mMat;
        alignas(16) glm::mat4 nMat;
    };

    template<typename GlobalUniform,typename BufferType> requires std::is_base_of<VkGenericBuffer<GlobalUniform>, BufferType>::value
    class GenericBufferHandler : public DescriptorSet<GlobalUniform> {

    protected:
        vector<BufferType *> vkBuffers;


    public:
        GenericBufferHandler(const VkDescriptorType descrType) :
                DescriptorSet<GlobalUniform>(
                        descrType,
                        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET) {


        }


        virtual void createBuffers(VkSwapChain &swapchain, GlobalUniform &data) = 0;

        void updateData(GlobalUniform &data) {
            for (int i = 0; i < vkBuffers.size(); i++) {
                vkBuffers[i]->setData(vector<GlobalUniform>{{data}});
                vkBuffers[i]->create();
            }
        }

        virtual BufferType* createBuffer(VkSwapChain &swapchain) =0;
    };


    template<typename BufferDataType>
    class UniformBuffer:public GenericBufferHandler<BufferDataType,VkUniformBuffer<BufferDataType>>{
    public:
        UniformBuffer(): GenericBufferHandler<BufferDataType, VkUniformBuffer<BufferDataType>>(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){}

        virtual void createBuffers(VkSwapChain &swapchain,BufferDataType &data){
            auto images = swapchain.getSwapChainImages();
            for (int i = 0; i < images.size(); i++) {
                auto buff = this->createBuffer(swapchain);
                this->vkBuffers.push_back(buff);
                buff->setPhysicalDevice(swapchain.getDeviceRepository());
                buff->setLogicalDevice();

                buff->setData(vector<BufferDataType>{{data}});
                buff->create();

                this->buffers.push_back(buff->getBuffer());
            }

            //at 0 offset
            this->buffer_info.offset = 0;
            //of the size of a camera data struct
            this->buffer_info.range = sizeof(BufferDataType);

        }

    protected:
        virtual  VkUniformBuffer<BufferDataType>* createBuffer(VkSwapChain &swapchain){
            int choosen = *swapchain
                    .getDeviceRepository()
                    .get()->getPerfectCriteriaMatcher()
                    .begin();


            return new  VkUniformBuffer<BufferDataType>(sizeof(BufferDataType), choosen);
        }

    };

    template<typename BufferDataType = stbi_uc *>
    class TextureBuffer:public GenericBufferHandler<BufferDataType,VkTextureMapper<BufferDataType>>{
        string path;
    public:
        TextureBuffer(string path): GenericBufferHandler<BufferDataType, VkTextureMapper<BufferDataType>>(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER),path(path){
            this->setStage(VK_SHADER_STAGE_FRAGMENT_BIT);
        }

        virtual void createBuffers(VkSwapChain &swapchain){
            auto buff = createBuffer(swapchain);
            buff->setPhysicalDevice(swapchain.getDeviceRepository());
            buff->setLogicalDevice();

            buff->create();
            this->imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            auto texture = buff->getTextureData();

            this->imageInfo.imageView = texture.textureImageView;
            this->imageInfo.sampler = texture.textureSampler;
        }
    protected:
        virtual void createBuffers(VkSwapChain &swapchain,BufferDataType &dataType){


        }

        virtual VkTextureMapper<BufferDataType>* createBuffer(VkSwapChain &swapchain) override {
            int choosen = *swapchain
                    .getDeviceRepository()
                    .get()->getPerfectCriteriaMatcher()
                    .begin();


            return new VkTextureMapper<BufferDataType>(sizeof(BufferDataType), choosen,path);
        }
    };

};
#endif //DB_VULKANBUFFERS_H
