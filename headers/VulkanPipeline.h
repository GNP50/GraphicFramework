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
            VkFragmentShader,
            VkVertexShader,
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
        VkPipelineViewportStateCreateInfo viewportState{};
        VkPipelineShaderStageCreateInfo shaderStages[2];

        VkPipeLine(VkFragmentShader& fragShader, VkVertexShader& vertexShader,std::shared_ptr<VkPhyDevice>devicerepo) : GenericGraphicPipeline(
                 fragShader, vertexShader) {
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

        void createDescriptorLayout(VkSwapChain& swapChain) {
            pipelineLayoutInfo.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
            pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

            VkResult  result = vkCreatePipelineLayout(swapChain.getChoosenDevice(), &pipelineLayoutInfo, nullptr,
                                            &pipelineLayout);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create pipeline layout!");
            }
        }

        void inline createShadersInfo(){
            shaderStages[0] = vertexShader.getStageCreateInfo();
            shaderStages[1] = fragmentShader.getStageCreateInfo();



            vertexInputInfo = vertexShader.getVertexInputInfo();


            inputAssembly.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;


            pipelineInfo.pStages = shaderStages;

        }

        void createViewPort(VkSwapChain& swapchain){
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) swapchain.getSwapChainExtent().width;
            viewport.height = (float) swapchain.getSwapChainExtent().height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;


            scissor.offset = {0, 0};
            scissor.extent = swapchain.getSwapChainExtent();

            viewportState.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;
            pipelineInfo.pViewportState = &viewportState;

        }

        void createColorRasterizerMultisampling(bool transp = false,VkPolygonMode polyModel = VK_POLYGON_MODE_FILL,VkCullModeFlags CM = VK_CULL_MODE_NONE){
            rasterizer.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = polyModel;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = CM;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


            multisampling.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_TRUE;
            multisampling.minSampleShading = .2f;
            multisampling.rasterizationSamples = msaaSamples;
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional


            colorBlendAttachment.colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT |
                    VK_COLOR_COMPONENT_G_BIT |
                    VK_COLOR_COMPONENT_B_BIT |
                    VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = transp ? VK_TRUE : VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor =
                    transp ? VK_BLEND_FACTOR_SRC_ALPHA : VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor =
                    transp ? VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA : VK_BLEND_FACTOR_ZERO;
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
        }
        void createDescriptorSetLayout(VkSwapChain& swapChain) {
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr;

            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = 1;
            samplerLayoutBinding.descriptorType =
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            samplerLayoutBinding.pImmutableSamplers = nullptr;

            std::array<VkDescriptorSetLayoutBinding, 2> bindings =
                    {uboLayoutBinding, samplerLayoutBinding};

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            VkResult result = vkCreateDescriptorSetLayout(swapChain.getChoosenDevice(), &layoutInfo,
                                                          nullptr, &descriptorSetLayout);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create descriptor set layout!");
            }


            createDescriptorLayout(swapChain);
        }

        void createRenderPass(VkSwapChain& swapchain ) {
            colorAttachmentResolve.format = swapchain.getSwapChainImageFormat().format;
            colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentResolveRef{};
            colorAttachmentResolveRef.attachment = 2;
            colorAttachmentResolveRef.layout =
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            depthAttachment.format = findDepthFormat();
            depthAttachment.samples = msaaSamples;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout =
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentReference depthAttachmentRef{};
            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout =
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            colorAttachment.format = swapchain.getSwapChainImageFormat().format;
            colorAttachment.samples = msaaSamples;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout =
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
            subpass.pDepthStencilAttachment = &depthAttachmentRef;
            subpass.pResolveAttachments = &colorAttachmentResolveRef;

            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            std::array<VkAttachmentDescription, 3> attachments =
                    {colorAttachment, depthAttachment,
                     colorAttachmentResolve};

            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;

            VkResult result = vkCreateRenderPass(swapchain.getChoosenDevice(), &renderPassInfo, nullptr,
                                                 &renderPass);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create render pass!");
            }
        }


        void createPipeline(VkSwapChain& swapchain, bool transp = false,VkPolygonMode polyModel = VK_POLYGON_MODE_FILL,VkCullModeFlags CM = VK_CULL_MODE_NONE) {

            createColorResources(swapchain);
            createDepthResources(swapchain);
            createRenderPass(swapchain);
            createDescriptorSetLayout(swapchain);
            createShadersInfo();
            createViewPort(swapchain);
            createColorRasterizerMultisampling(transp,polyModel,CM);

            pipelineInfo.sType =
                    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
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

            VkResult result = vkCreateGraphicsPipelines(swapchain.getChoosenDevice(), VK_NULL_HANDLE, 1,
                                               &pipelineInfo, nullptr, &graphicsPipeline);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create graphics pipeline!");
            }

            fragmentShader.destroy(swapchain.getChoosenDevice());
            vertexShader.destroy(swapchain.getChoosenDevice());

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

        void createColorResources(VkSwapChain& swapchain) {
            VkFormat colorFormat = swapchain.getSwapChainImageFormat().format;
            createImage(swapchain.getSwapChainExtent().width, swapchain.getSwapChainExtent().height, 1,
                        msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        colorImage, colorImageMemory);
            colorImageView = createImageView(colorImage, colorFormat,
                                             VK_IMAGE_ASPECT_COLOR_BIT, 1,
                                             VK_IMAGE_VIEW_TYPE_2D, 1);
        }

        void createDepthResources(VkSwapChain& swapChain) {
            VkFormat depthFormat = findDepthFormat();


            auto swapChainExtent = swapChain.getSwapChainExtent();
            auto swapChainImageFormat = swapChain.getSwapChainImageFormat().format;


                createImage(swapChainExtent.width, swapChainExtent.height, 1,
                        msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        depthImage, depthImageMemory);
            depthImageView = createImageView(depthImage, depthFormat,
                                             VK_IMAGE_ASPECT_DEPTH_BIT, 1,
                                             VK_IMAGE_VIEW_TYPE_2D, 1);

            /**transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1);**/
        }

        /**todo understand
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

                if (hasStencilComponent(format)) {
                    barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
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
        }**/


        void createFrameBuffer(VkSwapChain& swapChain){
            auto swapChainImageViews = swapChain.getSwapChainImageViews();
            auto swapChainExtent = swapChain.getSwapChainExtent();

            swapChainFramebuffers.resize(swapChainImageViews.size());
            for (size_t i = 0; i < swapChainImageViews.size(); i++) {
                std::array<VkImageView, 3> attachments = {
                        colorImageView,
                        depthImageView,
                        swapChainImageViews[i]
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

    class VkPhongPipeline:VkPipeLine{
        void createDescriptorLayout(VkSwapChain& swapChain) {
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr;

            VkDescriptorSetLayoutBinding globalUboLayoutBinding{};
            globalUboLayoutBinding.binding = 1;
            globalUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            globalUboLayoutBinding.descriptorCount = 1;
            globalUboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
            globalUboLayoutBinding.pImmutableSamplers = nullptr;

            std::array<VkDescriptorSetLayoutBinding, 2> bindings =
                    {uboLayoutBinding, globalUboLayoutBinding};

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            VkResult result = vkCreateDescriptorSetLayout(swapChain.getChoosenDevice(), &layoutInfo,
                                                          nullptr, &descriptorSetLayout);
            if (result != VK_SUCCESS) {
                Terminal::getInstance().printnl(result);
                throw std::runtime_error("failed to create descriptor set layout!");
            }
        }

    };
}
#endif //DB_VULKANPIPELINE_H
