//
// Created by Pierluigi Carlino on 06/09/22.
//

#include "commonVulkanIncludes.h"
#include "SkyBoxRender.h"

using namespace Vulkan;

shared_ptr<Enviroment> GenericSkyBox::enviroment = nullptr;
UniformBuffer<GenericSkyBox::GenericSkyBoxBuffer> *GenericSkyBox::cameraInfoBuffer = nullptr;
GenericSkyBox::GenericSkyBoxBuffer GenericSkyBox::hostCameraInfoBuffer = {};

