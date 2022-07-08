//
// Created by Pierluigi Carlino on 06/03/22.
//

#include "VulkanFramework.h"


using namespace Vulkan;


const vector<glm::vec3> &Object::getVertex() const {
    return vertexes;
}

const vector<int> &Object::getIndicies() const {
    return indicies;
}

void Object::setVertex(vector<glm::vec3> vertex) {
    this->vertexes = vector<glm::vec3>(vertex);
}

void Object::setIndicies(vector<int> indicies) {
    this->indicies = vector<int>(indicies);
}
