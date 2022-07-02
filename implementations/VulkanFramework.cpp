//
// Created by Pierluigi Carlino on 06/03/22.
//

#include "VulkanFramework.h"


using namespace Vulkan;


const vector<glm::vec3> &Object::getVertex() const {
    return vertex;
}

const vector<glm::vec3> &Object::getIndicies() const {
    return indicies;
}

void Object::setVertex(vector<glm::vec3> vertex) {
    this->vertex = vector<glm::vec3>(vertex);
}

void Object::setIndicies(vector<glm::vec3> indicies) {
    this->indicies = vector<glm::vec3>(vertex);
}
