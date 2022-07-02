//
// Created by Pierluigi Carlino on 02/07/22.
//

#ifndef DB_VULKANPIPELINE_H
#define DB_VULKANPIPELINE_H

#include "commonVulkanIncludes.h"
#include "VulkanShaders.h"
#include "VulkanSwapChain.h"

namespace Vulkan{
    class VkPipeLine:public GenericGraphicPipeline<
            VkPipeline,
            VkPipelineLayout,
            VkPipelineShaderStageCreateInfo,
            VkPipelineVertexInputStateCreateInfo,
            VkPipelineInputAssemblyStateCreateInfo,
            VkPipelineRasterizationStateCreateInfo,
            VkPipelineMultisampleStateCreateInfo,
            VkPipelineColorBlendAttachmentState,
            VkPipelineColorBlendStateCreateInfo,
            VkPipelineLayoutCreateInfo,
            VkPipelineDepthStencilStateCreateInfo,
            VkGraphicsPipelineCreateInfo,
            VkShader,
            VkViewport,
            VkRect2D,
            VkDescriptorSetLayoutBinding,
            VkDescriptorSetLayoutCreateInfo,
            VkDescriptorSetLayout,
            VkAttachmentDescription,
            VkAttachmentReference,
            VkSubpassDescription,
            VkSubpassDependency,
            VkRenderPassCreateInfo,
            VkRenderPass,
            VkFramebuffer>{




    public:

        struct Vertex {
            glm::vec3 pos;
            glm::vec3 norm;
            glm::vec2 texCoord;

            static VkVertexInputBindingDescription getBindingDescription() {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return bindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 3>
            getAttributeDescriptions() {
                std::array<VkVertexInputAttributeDescription, 3>
                        attributeDescriptions{};

                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(Vertex, pos);

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(Vertex, norm);

                attributeDescriptions[2].binding = 0;
                attributeDescriptions[2].location = 2;
                attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

                return attributeDescriptions;
            }
        };

        VkAttachmentDescription colorAttachmentResolve{};
        std::shared_ptr<VkPhyDevice> deviceRepo;
        // Questi dovrebbero essere i render target
        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;

        VkPipeLine(VkShader& fragShader, VkShader& vertexShader,std::shared_ptr<VkPhyDevice>devicerepo) : GenericGraphicPipeline(fragShader, vertexShader) {
            this->deviceRepo = devicerepo;
        }

        VkFormat findDepthFormat() {
            return findSupportedFormat({VK_FORMAT_D32_SFLOAT,
                                        VK_FORMAT_D32_SFLOAT_S8_UINT,
                                        VK_FORMAT_D24_UNORM_S8_UINT},
                                       VK_IMAGE_TILING_OPTIMAL,
                                       VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
        }

        VkFormat findSupportedFormat(const std::vector<VkFormat> candidates,
                                     VkImageTiling tiling, VkFormatFeatureFlags features) {
            for (VkFormat format : candidates) {
                VkFormatProperties props;

                vkGetPhysicalDeviceFormatProperties(deviceRepo->getDevice(
                        *deviceRepo->getPerfectCriteriaMatcher().begin()
                ), format, &props);
                if (tiling == VK_IMAGE_TILING_LINEAR &&
                    (props.linearTilingFeatures & features) == features) {
                    return format;
                } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                           (props.optimalTilingFeatures & features) == features) {
                    return format;
                }
            }

            throw std::runtime_error("failed to find supported format!");
        }

        void createRenderPass(VkSwapChain swapChain){

            colorAttachment.format = swapChain.getSwapChainImageFormat().format;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;


            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = 1;
            renderPassInfo.pAttachments = &colorAttachment;
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;

            VkResult result = vkCreateRenderPass(swapChain.getChoosenDevice(), &renderPassInfo, nullptr,
                                                 &renderPass);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create render pass!");
            }
        }


        void createDescriptorSetLayout(VkSwapChain swapChain) {
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr;


            samplerLayoutBinding.binding = 1;
            samplerLayoutBinding.descriptorType =
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            //

            std::array<VkDescriptorSetLayoutBinding, 2> bindings =
                    {uboLayoutBinding, samplerLayoutBinding};

            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
            layoutInfo.pBindings = bindings.data();

            VkResult result = vkCreateDescriptorSetLayout(swapChain.getChoosenDevice(), &layoutInfo,
                                                          nullptr, &descriptorSetLayout);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create descriptor set layout!");
            }
        }

        void createGraphicsPipeline(VkSwapChain swapChain) {
            createRenderPass(swapChain);
            createDescriptorSetLayout(swapChain);

            createDescriptorSetLayout(swapChain);
            createColorResources(swapChain);

            VkPipelineShaderStageCreateInfo shaderStages[] =
                    {vertexShader.getStageCreateInfo(), fragmentShader.getStageCreateInfo()};

            vertexInputInfo.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            auto attributeDescriptions = Vertex::getAttributeDescriptions();
            auto bindingDescription = Vertex::getBindingDescription();
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount =
                    static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.pVertexAttributeDescriptions =
                    attributeDescriptions.data();

            inputAssembly.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            auto swapChainExtend = swapChain.getSwapChainExtent();

            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) swapChainExtend.width;
            viewport.height = (float) swapChainExtend.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;


            scissor.offset = {0, 0};
            scissor.extent = swapChainExtend;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

            rasterizer.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


            multisampling.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional

            colorBlendAttachment.colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT |
                    VK_COLOR_COMPONENT_G_BIT |
                    VK_COLOR_COMPONENT_B_BIT |
                    VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor =
                    VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstColorBlendFactor =
                    VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.colorBlendOp =
                    VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.srcAlphaBlendFactor =
                    VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstAlphaBlendFactor =
                    VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp =
                    VK_BLEND_OP_ADD; // Optional

            colorBlending.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional

            pipelineLayoutInfo.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
            pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

            VkResult result = vkCreatePipelineLayout(swapChain.getChoosenDevice(), &pipelineLayoutInfo, nullptr,
                                                     &pipelineLayout);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create pipeline layout!");
            }

            depthStencil.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.minDepthBounds = 0.0f; // Optional
            depthStencil.maxDepthBounds = 1.0f; // Optional
            depthStencil.stencilTestEnable = VK_FALSE;
            depthStencil.front = {}; // Optional
            depthStencil.back = {}; // Optional

            pipelineInfo.sType =
                    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = nullptr; // Optional
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional

            result = vkCreateGraphicsPipelines(swapChain.getChoosenDevice(), VK_NULL_HANDLE, 1,
                                               &pipelineInfo, nullptr, &graphicsPipeline);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create graphics pipeline!");
            }

        }

        uint32_t findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) {
            VkPhysicalDeviceMemoryProperties memProperties;
            auto device = deviceRepo->getDevice(*deviceRepo->getPerfectCriteriaMatcher().begin());
            vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) &&
                    (memProperties.memoryTypes[i].propertyFlags & properties) ==
                    properties) {
                    return i;
                }
            }

            throw std::runtime_error("failed to find suitable memory type!");
        }

        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                         VkSampleCountFlagBits numSamples, VkFormat format,
                         VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties, VkImage& image,
                         VkDeviceMemory& imageMemory) {
            VkImageCreateInfo imageInfo{};
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

            auto device = deviceRepo->getLogicalDevice(
                    *deviceRepo->getPerfectCriteriaMatcher().begin()
            );

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

        VkImageView createImageView(VkImage image, VkFormat format,
                                    VkImageAspectFlags aspectFlags,
                                    uint32_t mipLevels, VkImageViewType type, int layerCount) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = image;
            viewInfo.viewType = type;
            viewInfo.format = format;
            viewInfo.subresourceRange.aspectMask = aspectFlags;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = mipLevels;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = layerCount;
            VkImageView imageView;

            auto device = deviceRepo->getLogicalDevice(
                    *deviceRepo->getPerfectCriteriaMatcher().begin()
            );

            VkResult result = vkCreateImageView(device, &viewInfo, nullptr,
                                                &imageView);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create image view!");
            }
            return imageView;
        }

        void createColorResources(VkSwapChain swapChain) {
            auto swapChainExtent = swapChain.getSwapChainExtent();
            auto swapChainImageFormat = swapChain.getSwapChainImageFormat().format;

            VkFormat colorFormat = swapChainImageFormat;
            createImage(swapChainExtent.width, swapChainExtent.height, 1,
                        VK_SAMPLE_COUNT_1_BIT, colorFormat, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        colorImage, colorImageMemory);
            colorImageView = createImageView(colorImage, colorFormat,
                                             VK_IMAGE_ASPECT_COLOR_BIT, 1,
                                             VK_IMAGE_VIEW_TYPE_2D, 1);
        }
        void createDepthResources(VkSwapChain swapChain) {
            auto swapChainExtent = swapChain.getSwapChainExtent();
            auto swapChainImageFormat = swapChain.getSwapChainImageFormat().format;


            VkFormat depthFormat = findDepthFormat();

            createImage(swapChainExtent.width, swapChainExtent.height, 1,
                        VK_SAMPLE_COUNT_1_BIT, depthFormat, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        depthImage, depthImageMemory);
            depthImageView = createImageView(depthImage, depthFormat,
                                             VK_IMAGE_ASPECT_DEPTH_BIT, 1,
                                             VK_IMAGE_VIEW_TYPE_2D, 1);

        }


        void createFrameBuffer(VkSwapChain swapChain){
            auto swapChainImageViews = swapChain.getSwapChainImageViews();
            auto swapChainExtent = swapChain.getSwapChainExtent();

            swapChainFramebuffers.resize(swapChainImageViews.size());
            for (size_t i = 0; i < swapChainImageViews.size(); i++) {
                std::array<VkImageView, 1> attachments = {
                        swapChainImageViews[i],
                };

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType =
                        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = renderPass;
                framebufferInfo.attachmentCount =
                        static_cast<uint32_t>(attachments.size());;
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = swapChainExtent.width;
                framebufferInfo.height = swapChainExtent.height;
                framebufferInfo.layers = 1;

                VkResult result = vkCreateFramebuffer(swapChain.getChoosenDevice(), &framebufferInfo, nullptr,
                                                      &swapChainFramebuffers[i]);
                if (result != VK_SUCCESS) {
                    Terminal::getInstance().printnl(result);
                    throw std::runtime_error("failed to create framebuffer!");
                }
            }
        }
    };

}
#endif //DB_VULKANPIPELINE_H
