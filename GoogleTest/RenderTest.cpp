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
        auto camera = Camera(0.1,0.1);
        auto camera2 = Camera(0.1,0.1);

        camera2.setCamPos(vec3(0.0,3.0,0.0));
        camera2.setCamAng(vec3(0.0,-1.4,0.0));

        enviroment = new Enviroment("triangle test");
        enviroment->addCamera(camera);
        enviroment->addCamera(camera2);

        new BlenderRender("objs/landscape.obj","objs/landscape.png",*enviroment);
        //new Sky("textures/triangle.png",*enviroment);
        //new BlenderRender("objs/legofriends.obj","objs/paesaggio.png",*enviroment);
/**

            new Triangle(
                      vec3(-1,-1,0),
                      vec3(1,-1,0),
                      vec3(1,1,0),"textures/triangle.png",
                      *enviroment
              );
              new Triangle(
                      vec3(0.4,0.1,0),
                      vec3(-0.4,-0.1,0),
                      vec3(0.4,-0.1,0),"textures/triangle.png",
                      *enviroment
              );**/


        enviroment->createRender();
    }

    Enviroment *enviroment;


};

TEST_F(RenderTestFixture,renderSingleTriangleTest){

    enviroment->mainLoop();
    assert(1==1);

}
