//
// Created by Pierluigi Carlino on 10/07/22.
//

#include "VkObject.h"

using namespace Vulkan;

const vector<glm::vec3> &Object::getVertex() const {
    return vertexes;
}

const vector<uint32_t> &Object::getIndicies() const {
    return indicies;
}

const vector<glm::vec2> &Object::getTextCoords() const{
    return textCoords;
};


void Object::setVertex(vector<glm::vec3> vertex) {
    this->vertexes = vector<glm::vec3>(vertex);
}

void Object::setIndicies(vector<uint32_t> indicies) {
    this->indicies = vector<uint32_t>(indicies);
}

void Object::setTextCoords(vector<glm::vec2> coords) {
    this->textCoords = vector<glm::vec2>(coords);
}