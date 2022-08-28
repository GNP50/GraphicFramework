//
// Created by Pierluigi Carlino on 09/07/22.
//

#include "gtest/gtest.h"
#include <VulkanFramework.h>
#include <vector>
#include "VulkanGeomtric.h"
#include "VkObjectExtensions.h"

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

        //new Sky("textures/triangle.png",*enviroment);
        /**
        new BlenderRender("objs/wheel.obj","objs/landscape.png",*enviroment);
        new BlenderRender("objs/wheel.obj","objs/landscape.png",*enviroment);**/

            new Triangle(
                      vec3(-1,-1,0),
                      vec3(1,-1,0),
                      vec3(1,1,0),"textures/triangle.png",
                      *enviroment
              );


        enviroment->createRender();
    }

    Enviroment *enviroment;


};

TEST_F(RenderTestFixture,extensionsFeature){


    enviroment->mainLoop();
    assert(1==1);

}


TEST_F(RenderTestFixture,renderSingleTriangleTest){

    enviroment->mainLoop();
    assert(1==1);

}
