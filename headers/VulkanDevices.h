//
// Created by Pierluigi Carlino on 02/07/22.
//

#ifndef DB_VULKANDEVICES_H
#define DB_VULKANDEVICES_H

#include "commonVulkanIncludes.h"
#include "VulkanEngine.h"

namespace Vulkan{
    class VkPhyDevice : public PhysicalDevice<
            VkPhysicalDevice,
            VkInstance,
            VkSurfaceKHR,
            VkPhysicalDeviceFeatures,
            VkDevice,
            VkEngine,
            VkQueue,
            VkQueueFlagBits
    > {
    protected:
        virtual bool isDevicesSuitable(vector<VkPhysicalDevice> devices) override;

        virtual bool isFeatureSupported(int offset) override;


        virtual VkPhysicalDeviceFeatures getSupportedExtensions(int offset) override;


        virtual void initDevices(VkInstance instance) override;


        template<typename DeviceExt>
        bool isExtensionSupported(int offset, DeviceExt requiredExtensions);


        template<>
        bool isExtensionSupported(int offset, VkPhysicalDeviceType requiredExtensions) {
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceProperties(this->devices[offset], &deviceProperties);
            vkGetPhysicalDeviceFeatures(this->devices[offset], &deviceFeatures);


            return deviceProperties.deviceType == requiredExtensions;
        };

        template<>
        bool isExtensionSupported(int offset, vector<VkQueueFlagBits> requiredExtensions) {
            if (this->supportQueues(offset, requiredExtensions))
                return true;
            return false;
        };

        template<>
        bool isExtensionSupported(int offset, vector<VkExtensionProperties> requiredExtensions) {

            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(devices[offset], nullptr,
                                                 &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(devices[offset], nullptr,
                                                 &extensionCount, availableExtensions.data());

            for (const auto extension: availableExtensions) {
                bool found = false;
                string first(extension.extensionName);
                for (const auto reqExtension: requiredExtensions) {
                    string second(reqExtension.extensionName);
                    if (first == second && reqExtension.specVersion == extension.specVersion) {
                        found = true;
                        break;
                    } else {

                    }
                }
                if (!found)
                    return false;
            }

            return false;
        };


    public:
        VkPhyDevice(
                VkEngine engine,
                vector<const char *> deviceExts = vector<const char *>({
                                                                               VK_KHR_SWAPCHAIN_EXTENSION_NAME
                                                                       })

        ) : PhysicalDevice(deviceExts,engine) {
            initDevices(instance);
            this->setSurface(engine.getSurface());
        }

        ~VkPhyDevice(){
            for(int i=0;i<logicalDevices.size();i++) {
                if(logicalDevices[i] != nullptr)
                    vkDestroyDevice(this->getLogicalDevice(i), nullptr);
            }
        }
        virtual bool supportQueues(int off, vector<VkQueueFlagBits> flags) override;

        virtual VkDevice createLogicalDevice(int off) override;

        //swaps function valutate if i have to make generic
        virtual VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR> &availableFormats);

        virtual VkPresentModeKHR chooseSwapPresentMode(
                const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        virtual vector<string> getAllDevicesName() override;

        virtual string getDeviceName(int off) override;

        template<typename Criteria>
        void getFirstSuitable(Criteria criteria);

        template<>
        void getFirstSuitable<vector<VkExtensionProperties>>(vector<VkExtensionProperties> criteria) {
            //insert the type id if not present
            if (!this->supportedDevices.contains(typeid(VkExtensionProperties))) {
                auto toAdd = set<int>();


                this->supportedDevices.insert(
                        {
                                typeid(VkExtensionProperties),
                                toAdd
                        }
                );
            }

            for (int i = 0; i < this->devices.size(); i++) {
                if (this->isExtensionSupported(i, criteria))
                    this->supportedDevices[typeid(VkExtensionProperties)].insert(i);
                if (this->verbose) {
                    Terminal::getInstance().printnl("Found suitable:\t" + this->getDeviceName(i));
                }
            }

        };

        template<>
        void getFirstSuitable<vector<VkPhysicalDeviceType>>(vector<VkPhysicalDeviceType> criteria) {
            //insert the type id if not present
            if (!this->supportedDevices.contains(typeid(VkPhysicalDeviceType))) {
                auto toAdd = set<int>();

                this->supportedDevices.insert(
                        {
                                typeid(VkPhysicalDeviceType),
                                toAdd
                        }
                );
            }


            for (int i = 0; i < this->devices.size(); i++) {
                for (auto s_criteria: criteria) {
                    if (this->isExtensionSupported<VkPhysicalDeviceType>(i, s_criteria))
                        this->supportedDevices[typeid(VkPhysicalDeviceType)].insert(i);
                    if (this->verbose) {
                        Terminal::getInstance().printnl("Found suitable:\t" + this->getDeviceName(i));
                    }
                }
            }
        };

        template<>
        void getFirstSuitable(vector<VkQueueFlagBits> criteria) {
            //insert the type id if not present
            if (!this->supportedDevices.contains(typeid(VkQueueFlagBits))) {
                auto toAdd = set<int>();


                this->supportedDevices.insert(
                        {
                                typeid(VkQueueFlagBits),
                                toAdd
                        }
                );
            }


            for (int i = 0; i < this->devices.size(); i++) {

                if (this->isExtensionSupported<>(i, criteria))
                    this->supportedDevices[typeid(VkQueueFlagBits)].insert(i);
                if (this->verbose) {
                    Terminal::getInstance().printnl("Found suitable:\t" + this->getDeviceName(i));
                }
            }
        };

        template<typename T>
        T getUniqueIndicies(int off) {
            auto graphFamilyIndVec = this->queueIndicies[off];
            auto presentFamilyInd = this->presentIndicies[off];

            uint32_t queueFamilyIndices[] = {0, 0};

            for (auto gF: graphFamilyIndVec) {
                queueFamilyIndices[0] = gF.second;
                for (auto i: presentIndicies[off]) {
                    queueFamilyIndices[1] = i;

                    if (queueFamilyIndices[0] == queueFamilyIndices[1]) break;
                }
                if (queueFamilyIndices[0] == queueFamilyIndices[1]) break;
            }

            if constexpr(std::same_as<T, uint32_t *>) {
                return queueFamilyIndices;
            } else if constexpr(std::same_as<T, vector<uint32_t>>) {
                return vector<uint32_t>{{
                                                queueFamilyIndices[0], queueFamilyIndices[1]
                                        }};
            }
        };


        VkSampleCountFlagBits getMaxUsableSampleCount(int off) {

            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(this->devices[off], &physicalDeviceProperties);

            VkSampleCountFlags counts =
                    physicalDeviceProperties.limits.framebufferColorSampleCounts &
                    physicalDeviceProperties.limits.framebufferDepthSampleCounts;

            if (counts >= VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
            if (counts >= VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
            if (counts >= VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
            if (counts >= VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
            if (counts >= VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
            if (counts >= VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

            return VK_SAMPLE_COUNT_1_BIT;
        }

    };

}
#endif //DB_VULKANDEVICES_H
