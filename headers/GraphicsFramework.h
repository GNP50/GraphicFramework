//
// Created by Pierluigi Carlino on 05/03/22.
//

#ifndef GRAPHICFRAMEWORK
#define GRAPHICFRAMEWORK

#include <set>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <Terminal.h>

using namespace std;

template<typename Result>
struct ErrCodeStruct {
    Result resultCode;
    std::string meaning;
};


template<typename Ret>
class IReturnable
{
public:
    virtual ~IReturnable() {}
    virtual Ret& returnObj() = 0;

};


template<typename Ret,typename StructTp>
class AppConfiguration:public IReturnable<Ret>{
protected:
    int appMajor;
    int appMinor;
    int appPatch;
    int engineMajor;
    int engineMinor;
    int enginePatch;

    string engineName;
    int apiVersion;
    StructTp structureType;

    bool enableValidationLayers;
    Ret appRappresentation;
public:
    string appName;
    vector<const char*> extension;


    AppConfiguration(){
        this->setAppMajor();
        this->setAppMinor();
        this->setAppPatch();

        this->setEngineMajor();
        this->setEngineMinor();
        this->setEnginePatch();

        this->setEngineName();
        this->setApiVersion();

    }
    virtual void setAppMajor(int appMajor){
        this->appMajor = appMajor;
    }
    virtual void setAppMinor(int appMinor){
        this->appMinor = appMinor;
    }
    virtual void setAppPatch(int appPatch){
        this->appPatch = appPatch;
    }
    virtual void setEngineMajor(int engineMajor){
        this->engineMajor = engineMajor;
    }
    virtual void setEngineMinor(int engineMinor){
        this->engineMinor = engineMinor;
    }
    virtual void setEnginePatch(int enginePatch){
        this->enginePatch = enginePatch;
    }

    virtual void setEngineName(string engineName){
        this->engineName = engineName;
    }

    virtual void setAppName(string appName){
        this->engineName = engineName;
    }


    virtual void setApiVersion(int apiVersion){
        this->apiVersion = apiVersion;
    }

    virtual void setStructType(StructTp structTp){
        this->structureType = structTp;
    }

    virtual void setEnableValidationLayers(bool value){enableValidationLayers = value;};
    virtual void setRequiredExtension() = 0;
    virtual Ret& returnObj() = 0;
};





template<
        typename Instance,
        typename Config,
        typename ErroCode,
        typename WindowObj,
        typename SurfaceObj
>
class GraphicEngine{
protected:
    Instance instance;
    Config appConfiguration;
    WindowObj* window;
    SurfaceObj surface;
    std::vector<const char*> validationLayers;

    bool enableValidationLayers;

    static map<ErroCode,string> errorStorer;
    void (*framebufferResizeCallback)(WindowObj, int, int);

public:
    virtual bool checkValidationLayerSupport() =0;


    virtual void setAppConfig(Config config){instance = config;};
    virtual void setInstance() = 0;

    virtual void setValidationLayers(vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    }){
        this->validationLayers = validationLayers;
    }


    void printError(ErroCode errCode){
        Terminal::getInstance().printnl(
                errorStorer.at(errCode), BackGroudColor::red, ForeGroudColor::yellow);
    }

    //window handlers
    virtual void initWindow(int w,int h);
    virtual void defaultWindowResizeHandler(WindowObj* window, int width, int height);
    virtual void setResizeWindowCallBack(void* newFunc);

    //debug helper
    virtual void setupDebugMessenger(void (*debugCallback)()) = 0;

};


template<
        typename Device,
        typename Instance,
        typename SurfaceObj,
        typename SwapChainDetail,
        typename DeviceFeature,
        typename DeviceExtension
        >
class PhysicalDevice{
protected:
    vector<Device> devices;
    std::vector<const char*> deviceExtensions;
    SurfaceObj surface;

    virtual void initDevices(Instance instance) = 0;

    template<typename Criteria>
    void getFirstSuitable(Criteria criteria);

    virtual bool isFeatureSupported(int offset) = 0;
    virtual DeviceFeature getSupportedExtensions(int offset) = 0;
    virtual bool isSwapChainAdeguate(int offset,SurfaceObj surface) = 0;
    virtual bool isDevicesSuitable(vector<Device> devices) = 0;
    virtual bool isDeviceSuitable(vector<Device> devices) = 0;
    virtual bool isExtensionSupported(int offset,vector<DeviceExtension> requiredExtensions);

    virtual SwapChainDetail queryChainSupport(Device device,SurfaceObj surfaceObj) = 0;

    //lather when other library will be added try to make this general(or if not leave it as is)
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() &&
                   presentFamily.has_value();
        }
    };

    virtual QueueFamilyIndices  findQueueFamilies(Device device) = 0;

public:
    PhysicalDevice(vector<const char*> deviceExts){
        deviceExtensions = deviceExts;
    }


};
#endif //GRAPHICFRAMEWORK
