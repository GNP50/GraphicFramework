//
// Created by Pierluigi Carlino on 13/03/22.
//

#ifndef DB_ENGINETESTS_H
#define DB_ENGINETESTS_H


#include "gtest/gtest.h"
#include <VulkanFramework.h>

using namespace Vulkan;

class EngineTestFixture : public ::testing::Test {

protected:
    virtual void SetUp()
    {
        engine = new VkEngine();
    }

    virtual void TearDown() {
        delete engine;
    }


    VkEngine* engine;


};

TEST_F(EngineTestFixture, CreateInstance){ // 1/1/1 -> 1/3/1
    Terminal::getInstance().printnl("Starting creation objects ");
}

#endif //DB_ENGINETESTS_H
