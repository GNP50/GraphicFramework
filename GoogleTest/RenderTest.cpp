//
// Created by Pierluigi Carlino on 09/07/22.
//

#include "gtest/gtest.h"
#include <VulkanFramework.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace Vulkan;


class DeviceTestFixture : public ::testing::Test {

protected:
    virtual void SetUp() {

        configuration = VkAppConfiguration();
        configuration.setAppName("Assignment 12");
        engine = new VkEngine(w, h, configuration);

        deviceRepository = std::make_shared<VkPhyDevice>(*engine);


    }


    void PrintAllDevicesName() {
        vector<string> names = deviceRepository->getAllDevicesName();
        for (auto name = begin(names); name != end(names); name++) {
            cout << name->data() << endl;
        }
    }

    void ChooseADevice() {
        auto type = vector<VkPhysicalDeviceType>{
                {VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU}};


        auto queueFlags = vector<VkQueueFlagBits>{{VK_QUEUE_GRAPHICS_BIT}};

        deviceRepository->getFirstSuitable(type);
        deviceRepository->getFirstSuitable(queueFlags);

        int choosen = *deviceRepository->getPerfectCriteriaMatcher().begin();
        deviceRepository->createLogicalDevice(choosen);
        choosenDevice = choosen;

    }

    void createUniformBuffer() {
        Object *wheel = new BlenderObj("objs/wheel.obj");

        wheel->create();

        m = new Model();

        m->setDeviceRepository(deviceRepository);

        m->addObject(*wheel);
        m->createBuffers();


    }

    void setAndCreateSwapChain() {
        swapChain.setDeviceRepository(deviceRepository);

        auto cap = VkSurfaceCapabilitiesKHR();
        cap.maxImageCount = 1;
        cap.minImageCount = 1;
        cap.maxImageArrayLayers = 1;

        auto presentMode = vector<VkPresentModeKHR>{VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_MAILBOX_KHR};
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
        auto extend = VkExtent2D{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};

        swapChain.createSwapChain(off, formats[0], presentMode[0], extend);
        swapChain.updateSwapImages();
        swapChain.createImageView();
    }

    void createPipeline() {
        setAndCreateSwapChain();
        assert(choosenDeviceSwapChain != -1);

        fragShader = new VkFragmentShader("shaders/Fragment.spv");
        vertexShader = new VkVertexShader("shaders/Vertex.spv");

        fragShader->create(deviceRepository->getLogicalDevice(choosenDevice));
        vertexShader->create(deviceRepository->getLogicalDevice(choosenDevice));


        pipeline = new VkPipeLine(*fragShader, *vertexShader, deviceRepository);
        pipeline->createPipelineWithPolyMode(swapChain, VK_POLYGON_MODE_POINT);

        pipeline->createFrameBuffer(swapChain);
    }


    std::shared_ptr<VkPhyDevice> deviceRepository;
    VkSwapChain swapChain;
    VkPipeLine *pipeline;
    VkFragmentShader *fragShader;
    VkVertexShader *vertexShader;
    Model *m;


    VkEngine *engine;
    VkAppConfiguration configuration;

    int choosenDevice = -1;
    int choosenDeviceSwapChain = -1;

    const int w = 640, h = 480;

}
#include "RenderTest.h"
