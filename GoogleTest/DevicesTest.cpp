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

        configuration = VkAppConfiguration();
        configuration.setAppName("Assignment 12");
        engine = new VkEngine(w,h,configuration);

        deviceRepository = new VkPhyDevice(*engine);





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
        Object triangle = Object();

        auto vertex = vector<glm::vec3>{
                glm::vec3 (0.0f,0.1f,-0.1f)
        };
        auto index = vector<glm::vec3>{
                glm::vec3 (1,2,3)
        };

        triangle.setVertex(vertex);
        triangle.setIndicies(index);

        m = new Model();

        m->setDeviceRepository(deviceRepository);

        m->addObject(triangle);
        m->createBuffers();



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
        auto extend = VkExtent2D{static_cast<uint32_t>(w),static_cast<uint32_t>(h)};

        swapChain.createSwapChain(off,formats[0],presentMode[0], extend);
        swapChain.updateSwapImages();
        swapChain.createImageView();
    }

    void createPipeline(){
        setAndCreateSwapChain();
        assert(choosenDeviceSwapChain!=-1);

        fragShader = new VkFragmentShader("shaders/Fragment.spv");
        vertexShader = new VkVertexShader("shaders/Vertex.spv");

        fragShader->create(deviceRepository->getLogicalDevice(choosenDevice));
        vertexShader->create(deviceRepository->getLogicalDevice(choosenDevice));


        pipeline = new VkPipeLine(*fragShader,*vertexShader,deviceRepository);
        pipeline->createGraphicsPipeline(swapChain);

        pipeline->createFrameBuffer(swapChain);
    }


    VkPhyDevice* deviceRepository;
    VkSwapChain swapChain;
    VkPipeLine *pipeline;
    VkShader *fragShader;
    VkShader *vertexShader;
    Model *m;


    VkEngine* engine;
    VkAppConfiguration configuration;

    int choosenDevice = -1;
    int choosenDeviceSwapChain = -1;

    const int w = 640,h = 480;
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
    createPipeline();
    assert(1==1);
}

TEST_F(DeviceTestFixture, RendingTest){
    ChooseADevice();
    createUniformBuffer();
    assert(choosenDevice==0);
    createPipeline();



    Render r = Render(
            deviceRepository,
            pipeline,
            &swapChain,
            m
    );

    r.createCommandPool();
    r.createCommandBuffers();
    r.createSyncObjects();

    r.mainLoop();
    assert(1==1);
}



#endif //DB_DEVICESTEST_H
