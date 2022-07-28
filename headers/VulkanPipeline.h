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
            VkDescriptorSetLayout
            >{




    public:


        std::shared_ptr<VkPhyDevice> deviceRepo;


        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineViewportStateCreateInfo viewportState{};
        VkPipelineShaderStageCreateInfo shaderStages[2];

        VkPipeLine(VkFragmentShader& fragShader, VkVertexShader& vertexShader,std::shared_ptr<VkPhyDevice>devicerepo) : GenericGraphicPipeline(
                 fragShader, vertexShader) {
            this->deviceRepo = devicerepo;
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

            createDescriptorLayout(swapChain);
        }



        void createPipelineWithPolyMode(VkSwapChain& swapchain,  VkPolygonMode polyModel){
            this->createPipeline(swapchain,polyModel,VK_COMPARE_OP_LESS,VK_POLYGON_MODE_FILL,VK_CULL_MODE_NONE);
        }
        void createPipeline(VkSwapChain& swapchain, bool transp = false,VkCompareOp compareOp = VK_COMPARE_OP_LESS,VkPolygonMode polyModel = VK_POLYGON_MODE_FILL,VkCullModeFlags CM = VK_CULL_MODE_NONE) {
            msaaSamples = swapchain.getMaxUsableSampleCount();

            createDescriptorSetLayout(swapchain);
            createShadersInfo();
            createViewPort(swapchain);
            createColorRasterizerMultisampling(transp,polyModel,CM);

            depthStencil.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = compareOp;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.minDepthBounds = 0.0f; // Optional
            depthStencil.maxDepthBounds = 1.0f; // Optional
            depthStencil.stencilTestEnable = VK_FALSE;
            depthStencil.front = {}; // Optional
            depthStencil.back = {}; // Optional

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
            pipelineInfo.renderPass = swapchain.getRenderPass();
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional

            VkResult result = vkCreateGraphicsPipelines(swapchain.getChoosenDevice(), VK_NULL_HANDLE, 1,
                                               &pipelineInfo, nullptr, &graphicsPipeline);

            Terminal::getInstance().printOnCondition("to create the pipeline",result!=VK_SUCCESS,result);
            fragmentShader.destroy(swapchain.getChoosenDevice());
            vertexShader.destroy(swapchain.getChoosenDevice());

        }

        void destroy(VkSwapChain &swapchain){
            vkDestroyPipelineLayout(swapchain.getChoosenDevice(),pipelineLayout, nullptr);
            Terminal::getInstance().printDestroy(" destroy pipeline layout ");

            vkDestroyPipeline(swapchain.getChoosenDevice(),graphicsPipeline, nullptr);
            Terminal::getInstance().printDestroy(" destroy pipeline  ");

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
