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
using namespace Vulkan;


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

    auto type = vector<VkPhysicalDeviceType>  {{VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU}};


    auto queueFlags = vector<VkQueueFlagBits>{{VK_QUEUE_GRAPHICS_BIT}};

    deviceRepository->getFirstSuitable(type);
    deviceRepository->getFirstSuitable(queueFlags);

    int choosen = *deviceRepository->getPerfectCriteriaMatcher().begin();
    deviceRepository->createLogicalDevice(choosen);

    auto buffer = new VkUniformBuffer<float>(100,choosen);
    buffer->setLogicalDevice(deviceRepository->getLogicalDevice(choosen));

    buffer->setPhysicalDevice(deviceRepository);


    buffer->createCommandPool();
    buffer->createBuffer();
    assert(choosen==0);
}

TEST_F(DeviceTestFixture, Assignment14){ // 1/1/1 -> 1/3/1

    auto type = vector<VkPhysicalDeviceType>  {{VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU}};


    auto queueFlags = vector<VkQueueFlagBits>{{VK_QUEUE_GRAPHICS_BIT}};

    deviceRepository->getFirstSuitable(type);
    deviceRepository->getFirstSuitable(queueFlags);

    int choosen = *deviceRepository->getPerfectCriteriaMatcher().begin();
    deviceRepository->createLogicalDevice(choosen);

    auto buffer = new VkUniformBuffer<float>(100,choosen);
    buffer->setLogicalDevice(deviceRepository->getLogicalDevice(choosen));

    buffer->setPhysicalDevice(deviceRepository);


    buffer->createCommandPool();
    buffer->createBuffer();

    VkSwapChain swapChain;
    swapChain.setDeviceRepository(deviceRepository);

    auto cap = VkSurfaceCapabilitiesKHR();
    cap.maxImageCount = 1;
    cap.minImageCount = 1;
    cap.maxImageArrayLayers = 1;

    auto presentMode = vector<VkPresentModeKHR>{VK_PRESENT_MODE_FIFO_KHR,VK_PRESENT_MODE_MAILBOX_KHR};
    auto formats = vector<VkSurfaceFormatKHR>{
            {
                    VK_FORMAT_B8G8R8A8_UNORM,
                    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
            },
            {
                    VK_FORMAT_R16G16B16A16_SFLOAT,
                    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
            },
    };

    swapChain.getFirstSuitable(formats);
    swapChain.getFirstSuitable(presentMode);
    swapChain.getFirstSuitable(cap);
    assert(swapChain.getPerfectCriteriaMatcher().contains(0));

    int off = 0;
    swapChain.createSwapChain(off,formats[0],presentMode[0],VkExtent2D{100,100});
    swapChain.updateSwapImages();
    swapChain.createImageView();
    assert(choosen==0);
}

//VK_KHR_SWAPCHAIN_EXTENSION_NAME


#endif //DB_DEVICESTEST_H
