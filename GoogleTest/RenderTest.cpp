//
// Created by Pierluigi Carlino on 09/07/22.
//

#include "gtest/gtest.h"
#include <VulkanFramework.h>
#include <vector>
#include "VulkanGeomtric.h"


using namespace std;
using namespace Vulkan;


class RenderTestFixture : public ::testing::Test {

protected:
    virtual void SetUp() {
        enviroment = new Enviroment("triangle test");

        new Triangle(
                vec3(0.5,0.5,0),
                vec3(-0.5,-0.5,0),
                vec3(0.5,-0.5,0),"textures/triangle.png",
                *enviroment
        );


        new Triangle(
                vec3(0.4,0.1,0),
                vec3(-0.4,-0.1,0),
                vec3(0.4,-0.1,0),"textures/triangle.png",
                *enviroment
        );


        enviroment->createRender();
    }

    Enviroment *enviroment;


};

TEST_F(RenderTestFixture,renderSingleTriangleTest){

    enviroment->mainLoop();
    assert(1==1);

}
