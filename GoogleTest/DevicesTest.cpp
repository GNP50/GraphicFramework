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


    void PrintAllDevicesName(){
        vector<string> names = deviceRepository->getAllDevicesName();
        for(auto name = begin(names);name != end(names);name++) {
            cout << name->data() << endl;
        }
    }

    void ChooseADevice(){
        auto type = vector<VkPhysicalDeviceType>  {{VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU}};


        auto queueFlags = vector<VkQueueFlagBits>{{VK_QUEUE_GRAPHICS_BIT}};

        deviceRepository->getFirstSuitable(type);
        deviceRepository->getFirstSuitable(queueFlags);

        int choosen = *deviceRepository->getPerfectCriteriaMatcher().begin();
        deviceRepository->createLogicalDevice(choosen);
        choosenDevice = choosen;

    }

    void createUniformBuffer(){
        auto buffer = new VkUniformBuffer<float>(100,choosenDevice);
        buffer->setLogicalDevice(deviceRepository->getLogicalDevice(choosenDevice));

        buffer->setPhysicalDevice(deviceRepository);


        buffer->createCommandPool();
        buffer->createBuffer();
    }

    void setAndCreateSwapChain(){
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
        choosenDeviceSwapChain = *swapChain.getPerfectCriteriaMatcher().begin();


        int off = choosenDeviceSwapChain;
        auto extend = VkExtent2D{100,100};

        swapChain.createSwapChain(off,formats[0],presentMode[0], extend);
        swapChain.updateSwapImages();
        swapChain.createImageView();
    }

    VkPhyDevice* deviceRepository;
    VkSwapChain swapChain;
    int choosenDevice = -1;
    int choosenDeviceSwapChain = -1;

};

TEST_F(DeviceTestFixture, PrintAllDevicesName){ // 1/1/1 -> 1/3/1
    PrintAllDevicesName();
}

TEST_F(DeviceTestFixture, Assignment13){ // 1/1/1 -> 1/3/1

    ChooseADevice();
    createUniformBuffer();
    assert(choosenDevice==0);
}

TEST_F(DeviceTestFixture, Assignment14){ // 1/1/1 -> 1/3/1
    ChooseADevice();
    createUniformBuffer();
    assert(choosenDevice==0);
    setAndCreateSwapChain();
    assert(choosenDeviceSwapChain!=-1);
}

TEST_F(DeviceTestFixture, PipeLineCreation){
    ChooseADevice();
    createUniformBuffer();
    assert(choosenDevice==0);
    setAndCreateSwapChain();
    assert(choosenDeviceSwapChain!=-1);

    VkFragmentShader fragShader = VkFragmentShader("shaders/Fragment.spv");
    VkVertexShader vertexShader = VkVertexShader("shaders/Fragment.spv");

    fragShader.create(deviceRepository->getLogicalDevice(choosenDevice));
    vertexShader.create(deviceRepository->getLogicalDevice(choosenDevice));


    VkPipeLine pipeline = VkPipeLine(fragShader,vertexShader);
    pipeline.createGraphicsPipeline(swapChain);

    assert(1==1);
}


#endif //DB_DEVICESTEST_H
