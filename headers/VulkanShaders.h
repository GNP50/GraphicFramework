//
// Created by Pierluigi Carlino on 02/07/22.
//

#ifndef DB_VULKANSHADERS_H
#define DB_VULKANSHADERS_H
#include "commonVulkanIncludes.h"

namespace Vulkan{


    class VkShader : public Shader<VkPipelineShaderStageCreateInfo> {
    protected:
        VkShaderModuleCreateInfo createInfo = {};
        VkShaderModule shaderModule = {};

    public:
        VkShader(string filename) : Shader(filename) {
            createInfo = {};
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

            stageInfo.module = shaderModule;

            //TODO think about a better way to handle this
            stageInfo.pName = "main";

        };

        VkShader(VkShader& shader):Shader(shader){
            this->shaderModule = shader.shaderModule;
            this->createInfo = shader.createInfo;
        }

        template<typename... T>
        void create(T...);

        template<typename... T>
        void destroy(T...);


        template<>
        void create(VkDevice device) {
            if (this->data.size() == 0)
                this->readFile();

            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = this->data.size();
            createInfo.pCode = reinterpret_cast<const uint32_t *>(this->data.data());

            if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shader module!");
            }

            stageInfo.module = shaderModule;
        }

        template<>
        void destroy(VkDevice device) {
            vkDestroyShaderModule(device, shaderModule, nullptr);
        }

    };

    class VkFragmentShader : public VkShader {
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        VkPipelineColorBlendStateCreateInfo colorBlending = {};

    public:
        VkFragmentShader(string filename,
                         VkPolygonMode polyMode = VK_POLYGON_MODE_FILL
        ) : VkShader(filename) {
            stageInfo = {};
            stageInfo.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            stageInfo.pName = "main";

            rasterizer = {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;

            rasterizer.polygonMode = polyMode;

            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


            colorBlendAttachment = {};
            colorBlendAttachment.colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                    VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

            colorBlending = {};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional



        }
    };

    class VkVertexShader : public VkShader {
    protected:
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};

    public:
        VkVertexShader(
                string filename,
                VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
        ) : VkShader(filename) {
            stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

            vertexInputInfo = {};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

            inputAssembly = {};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;
        }
    };

}
#endif //DB_VULKANSHADERS_H
