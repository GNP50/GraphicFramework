//
// Created by Pierluigi Carlino on 13/03/22.
//

#ifndef DB_WINDOWSTEST_H
#define DB_WINDOWSTEST_H


#include "gtest/gtest.h"
#include <VulkanFramework.h>
#include <chrono>
#include <thread>

using namespace Vulkan;

class WindowTestFixture : public ::testing::Test {

protected:
    virtual void SetUp()
    {
        configuration = VkAppConfiguration();
        configuration.setAppName("Assignment 12");

        engine = new VkEngine(640,480,configuration);


    }

    virtual void TearDown() {
        delete engine;

    }


    VkEngine* engine;
    VkAppConfiguration configuration;
};

TEST_F(WindowTestFixture, Assignment12){ // 1/1/1 -> 1/3/1
    Terminal::getInstance().printnl("Starting creation objects ");
    configuration.setRequiredExtension(true);

    for(auto ext:configuration.extension)
        cout<<ext<<"\n";

}

#endif //DB_WINDOWSTESTS_H
