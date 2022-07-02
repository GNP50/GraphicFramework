//
// Created by Pierluigi Carlino on 02/07/22.
//


#ifndef DB_VULKANAPPCONFIGURATION_H
#define DB_VULKANAPPCONFIGURATION_H

#include "commonVulkanIncludes.h"

namespace Vulkan{
    class VkAppConfiguration : public AppConfiguration<VkApplicationInfo, VkStructureType> {
    public:

        virtual void setAppMajor(int appMajor) override;

        virtual void setAppMinor(int appMinor) override;

        virtual void setAppPatch(int appPatch) override;

        virtual void setEngineMajor(int engineMajor) override;

        virtual void setEngineMinor(int engineMinor) override;

        virtual void setEnginePatch(int enginePatch) override;

        virtual void setAppName(string appName) override;

        virtual void setEngineName(string engineName) override;

        virtual void setApiVersion(int apiVersion) override;

        virtual void setStructType(VkStructureType structureType) override;

        virtual void setRequiredExtension(bool enableValidationLayers) override;

        virtual VkApplicationInfo &returnObj() {


            appRappresentation.sType = this->structureType;
            appRappresentation.pApplicationName = this->appName.c_str();
            appRappresentation.applicationVersion = VK_MAKE_VERSION(
                    this->appMajor,
                    this->appMinor,
                    this->appPatch);


            appRappresentation.pEngineName = this->engineName.c_str();
            appRappresentation.engineVersion = VK_MAKE_VERSION(
                    this->engineMajor,
                    this->engineMinor,
                    this->enginePatch);

            appRappresentation.apiVersion = VK_API_VERSION_1_0;
            appRappresentation.pNext = NULL;
            return appRappresentation;
        };
    };

}
#endif //DB_VULKANAPPCONFIGURATION_H
