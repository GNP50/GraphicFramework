//
// Created by Pierluigi Carlino on 13/03/22.
//

#ifndef DB_DEVICESTEST_H
#define DB_DEVICESTEST_H


#include "gtest/gtest.h"
#include <VulkanFramework.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class DeviceTestFixture : public ::testing::Test {

protected:
    virtual void SetUp()
    {
        deviceRepository = new VkPhyDevice();
    }

    virtual void TearDown() {
        delete deviceRepository;
    }


    VkPhyDevice* deviceRepository;

};

TEST_F(DeviceTestFixture, PrintAllDevicesName){ // 1/1/1 -> 1/3/1
    vector<string> names = deviceRepository->getAllDevicesName();
    for(auto name = begin(names);name != end(names);name++) {
        cout << name->data() << endl;
    }

}

TEST_F(DeviceTestFixture,FindQueueFamilyTests){
    QueueFamilyIndices indices = deviceRepository->findQueueFamilies(0);
    auto graphicsQueue = deviceRepository->graphicsQueue;
    auto presetQueue = deviceRepository->presentQueue;

    return;
}

TEST_F(DeviceTestFixture,CreateLogicalDeviceTest){
    deviceRepository->createLogicalDevice(0);
}



#endif //DB_DEVICESTEST_H
