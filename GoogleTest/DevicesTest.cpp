//
// Created by Pierluigi Carlino on 13/03/22.
//

#ifndef DB_DEVICESTEST_H
#define DB_DEVICESTEST_H


#include "gtest/gtest.h"
#include <VulkanFramework.h>


class DeviceTestFixture : public ::testing::Test {

protected:
    virtual void SetUp()
    {
        deviceRepository = new VkPhyDevice(nullptr, vector<const char *>());
    }

    virtual void TearDown() {
        delete deviceRepository;
    }


    VkPhyDevice* deviceRepository;
};

TEST_F(DeviceTestFixture, PrintAllDevicesName){ // 1/1/1 -> 1/3/1

}


TEST_F(DeviceTestFixture, PrintAllDevicesName){ // 1/1/1 -> 1/3/1

}

#endif //DB_DEVICESTEST_H
