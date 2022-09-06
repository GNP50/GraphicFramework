//
// Created by Pierluigi Carlino on 09/07/22.
//

#include "gtest/gtest.h"
#include <VulkanFramework.h>
#include <vector>
#include "VulkanGeomtric.h"
#include "VkObjectExtensions.h"
#include "SkyBoxRender.h"

using namespace std;
using namespace Vulkan;


class RenderTestFixture : public ::testing::Test {

protected:
    virtual void SetUp() {
        auto camera = new Camera(0.1,0.1);
        auto camera2 = new Camera(0.1,0.1);

        camera2->setCamPos(vec3(0.0,3.0,0.0));
        camera2->setCamAng(vec3(0.0,-1.4,0.0));

        enviroment = new Enviroment("ass 12");
        enviroment->addCamera(camera);
        enviroment->addCamera(camera2);
    }

    Enviroment *enviroment;

    void startRender(){
        enviroment->createRender();
        enviroment->mainLoop();
    }

};

TEST_F(RenderTestFixture,SkyBoxRenderTest){
    new Sky("textures/triangle.png",*enviroment);
    startRender();
}

TEST_F(RenderTestFixture,BlenderRenderTest){
    new BlenderRender("objs/wheel.obj","objs/landscape.png",*enviroment);
    startRender();
}


TEST_F(RenderTestFixture,TriangleRenderTest){
    new Triangle(
            vec3(-1,-1,0),|
            vec3(1,-1,0),
            vec3(1,1,0),"textures/triangle.png",
            *enviroment
    );
    startRender();
}
