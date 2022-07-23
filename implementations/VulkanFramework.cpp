//
// Created by Pierluigi Carlino on 06/03/22.
//

#include "VulkanFramework.h"


using namespace Vulkan;





void Enviroment::SetUp() {

    configuration = VkAppConfiguration();
    configuration.setAppName(name);
    engine = new VkEngine(w, h, configuration);

    deviceRepository = std::make_shared<VkPhyDevice>(*engine);
    swapChain = std::make_shared<VkSwapChain>(*(new VkSwapChain()));

}
void Enviroment::setAndCreateSwapChain() {
    swapChain->setDeviceRepository(deviceRepository);

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

    swapChain->getFirstSuitable(formats);
    swapChain->getFirstSuitable(presentMode);
    swapChain->getFirstSuitable(cap);
    choosenDeviceSwapChain = *swapChain->getPerfectCriteriaMatcher().begin();


    int off = choosenDeviceSwapChain;
    auto extend = VkExtent2D{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};

    swapChain->createSwapChain(off, formats[0], presentMode[0], extend);
    swapChain->updateSwapImages();
    swapChain->createImageView();
}
void Enviroment::ChooseADevice(vector<VkPhysicalDeviceType> type) {



    auto queueFlags = vector<VkQueueFlagBits>{{VK_QUEUE_GRAPHICS_BIT}};

    deviceRepository->getFirstSuitable(type);
    deviceRepository->getFirstSuitable(queueFlags);

    choosen = *deviceRepository->getPerfectCriteriaMatcher().begin();
    deviceRepository->createLogicalDevice(choosen);
}
void Enviroment::createUniformBuffer() {
    for(auto render:renders) {
        render.createBuffer(*this->deviceRepository);
    }


}
void Enviroment::createSyncObjects() {

    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChain->getSwapChainImages().size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result1 = vkCreateSemaphore(swapChain->getChoosenDevice(), &semaphoreInfo, nullptr,
                                             &imageAvailableSemaphores[i]);
        VkResult result2 = vkCreateSemaphore(swapChain->getChoosenDevice(), &semaphoreInfo, nullptr,
                                             &renderFinishedSemaphores[i]);
        VkResult result3 = vkCreateFence(swapChain->getChoosenDevice(), &fenceInfo, nullptr,
                                         &inFlightFences[i]);
        if (result1 != VK_SUCCESS ||
            result2 != VK_SUCCESS ||
            result3 != VK_SUCCESS) {
            Terminal::getInstance().printnl(result1);
            Terminal::getInstance().printnl(result2);
            Terminal::getInstance().printnl(result3);
            throw std::runtime_error("failed to create synchronization objects for a frame!!");
        }
    }
}
void Enviroment::drawFrame() {
    vkWaitForFences(swapChain->getChoosenDevice(), 1, &inFlightFences[currentFrame],
                    VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;

    VkResult result = vkAcquireNextImageKHR(swapChain->getChoosenDevice(), swapChain->getSwapChain(), UINT64_MAX,
                                            imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //todo decide how to put
        //recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(swapChain->getChoosenDevice(), 1, &imagesInFlight[imageIndex],
                        VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    //todo decide how to put
    //updateUniformBuffer(imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] =
            {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(swapChain->getChoosenDevice(), 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(deviceRepository->graphicsQueue, 1, &submitInfo,
                      inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;


    VkSwapchainKHR swapChains[] = {swapChain->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(deviceRepository->presentQueue, &presentInfo);

    /** todo decide how to handle
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        framebufferResized) {
        framebufferResized = false;
        recreateSwapChain();
    } else **/if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
        return;
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


