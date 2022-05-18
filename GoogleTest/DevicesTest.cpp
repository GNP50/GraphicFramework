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

TEST_F(DeviceTestFixture, Assignment13){ // 1/1/1 -> 1/3/1

    VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU

}




#endif //DB_DEVICESTEST_H
