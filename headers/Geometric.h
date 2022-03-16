//
// Created by Pierluigi Carlino on 05/03/22.
//

#ifndef ASS0_GEOMETRIC_H
#define ASS0_GEOMETRIC_H

#include <stdlib.h>
#include <glm/glm.hpp>
#include <string>
#include <vulkan/vulkan.hpp>

using namespace  std;

namespace GraphicLibrary {
    class IStringable
    {
    public:
        virtual ~IStringable() {}
        virtual constexpr string toString() = 0;

    };


    class IGeometric
    {
    public:
        virtual ~IGeometric() {}
        virtual constexpr string variables() = 0;
        virtual constexpr string transformations() = 0;
        virtual constexpr string vertexBuffer() = 0;

    };


    template<
            typename BindingDescription,
            typename AttributeDescription>
    class Vertex: public IStringable{
    public:
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 texCoord;

    public:
        string toString();
        bool operator==(const Vertex& other) const {
            return pos == other.pos && normal == other.normal &&
                   texCoord == other.texCoord;
        }

        BindingDescription getBindingDescription();

        std::array<AttributeDescription, 3>
        getAttributeDescriptions();

    };


    class VkVertex:public Vertex<VkVertexInputBindingDescription.VkVertexInputAttributeDescription>{
        VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VkVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 3>
    getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3>
                attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VkVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(this, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(this, texCoord);

        return attributeDescriptions;
    }
    };
    template<
            typename VertexBuffer,
            typename IndexBuffer>
    class Geometric:public IGeometric,IStringable{
    protected:
        VertexBuffer vertexBuffer;
        IndexBuffer indexBuffer;

    public:
        Geometric();

        void translate(float x, float y, float z = 0.0);

        void dilatate(float x = 1.0, float y = 1.0, float z = 1.0);

        virtual void rotate(float x = 1.0, float y = 1.0, float z = 1.0);
        virtual void rotate(glm::vec3 rotateAxis);

        string toString();
    };

}

#endif //ASS0_GEOMETRIC_H
