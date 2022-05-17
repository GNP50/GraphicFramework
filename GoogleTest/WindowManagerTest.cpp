//
// Created by Pierluigi Carlino on 13/03/22.
//

#ifndef DB_WINDOWSTEST_H
#define DB_WINDOWSTEST_H


#include "gtest/gtest.h"
#include <VulkanFramework.h>
#include <chrono>
#include <thread>

class WindowTestFixture : public ::testing::Test {

protected:
    virtual void SetUp()
    {
        engine = new VkEngine();
        configuration = VkAppConfiguration();
        configuration.setAppName("Assignment 12");
        engine->setAppConfig(configuration);

    }

    virtual void TearDown() {
        delete engine;

    }


    VkEngine* engine;
    VkAppConfiguration configuration;
};

TEST_F(WindowTestFixture, CreateInstance){ // 1/1/1 -> 1/3/1
    Terminal::getInstance().printnl("Starting creation objects ");
    engine->initWindow(640,480);
    engine->createSurface();

    char escape;
    scanf("%s",&escape);

    while(escape!='q'){
        std::this_thread::sleep_for(500ms);
        scanf("%s",&escape);
    }
}

#endif //DB_WINDOWSTESTS_H
