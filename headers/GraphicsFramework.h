//
// Created by Pierluigi Carlino on 05/03/22.
//

#ifndef GRAPHICFRAMEWORK
#define GRAPHICFRAMEWORK

#include <set>
#include <vector>
#include <string>
#include <map>
#include <numeric>
#include <iostream>
#include <Terminal.h>
#include <typeindex>
#include <stdio.h>
#include <string.h>
#include <fstream>

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

        this->setAppName();
        this->setEngineName();
        this->setApiVersion();

    }
    virtual void setAppMajor(int appMajor = 1){
        this->appMajor = appMajor;
    }
    virtual void setAppMinor(int appMinor= 0){
        this->appMinor = appMinor;
    }
    virtual void setAppPatch(int appPatch= 0){
        this->appPatch = appPatch;
    }
    virtual void setEngineMajor(int engineMajor= 1){
        this->engineMajor = engineMajor;
    }
    virtual void setEngineMinor(int engineMinor= 0){
        this->engineMinor = engineMinor;
    }
    virtual void setEnginePatch(int enginePatch= 0){
        this->enginePatch = enginePatch;
    }

    virtual void setEngineName(string engineName = "No Name"){
        this->engineName = engineName;
    }

    virtual void setAppName(string appName = "No name"){
        this->appName = appName;
    }


    virtual void setApiVersion(int apiVersion =1 ){
        this->apiVersion = apiVersion;
    }

    virtual void setStructType(StructTp structTp){
        this->structureType = structTp;
    }

    virtual void setRequiredExtension(bool enableValidationLayers) = 0;
    virtual Ret& returnObj() = 0;
};


template<
        typename Device,
        typename Instance,
        typename SurfaceObj,
        typename SwapChainDetail,
        typename DeviceFeature,
        typename LogicaDevice,
        typename Engine,
        typename Queue,
        typename QueueFlags,
        typename Images,
        typename SwapChain,
        typename SwapChainFormat,
        typename SwapChainExtend
>
class PhysicalDevice{
protected:
    static vector<Device> devices;
    map<const std::type_index,set<int>> supportedDevices;
    std::vector<const char*> deviceExtensions;

    SurfaceObj surface;
    Engine engine;
    Instance instance;
    std::vector<Images> swapChainImages;
    SwapChain swapChain;
    vector<LogicaDevice> logicalDevices;
    SwapChainFormat swapChainImageFormat;
    SwapChainExtend swapChainExtent;

    bool verbose = false;

    virtual void initDevices(Instance instance) = 0;


    virtual bool isFeatureSupported(int offset) = 0;
    virtual DeviceFeature getSupportedExtensions(int offset) = 0;
    virtual bool isSwapChainAdeguate(int offset,SurfaceObj surface) = 0;
    virtual bool isDevicesSuitable(vector<Device> devices) = 0;

    template<typename DeviceExtension>
    bool isExtensionSupported(int offset,DeviceExtension requiredExtensions);

    virtual SwapChain createSwapChain(int off) = 0;

public:
    Queue graphicsQueue,presentQueue;
    std::map<int,vector<pair<QueueFlags,int>>> queueIndicies;
    std::map<int,vector<int>> presentIndicies;

    PhysicalDevice(vector<const char *> deviceExts){
        this->deviceExtensions = deviceExts;
        this->instance = engine.getInstance();
    }


    virtual SwapChainDetail querySwapChainSupport(Device device,SurfaceObj surfaceObj) = 0;
    virtual bool  supportQueues(int  off, vector<QueueFlags> flags) = 0;
    Device getDevice(int off){return devices[off];}
    virtual LogicaDevice createLogicalDevice(int off) = 0;
    virtual vector<string> getAllDevicesName() = 0;
    virtual string getDeviceName(int off) = 0;

    template<typename T>
    T getUniqueIndicies(int off);

    template<typename Criteria>
    void getFirstSuitable(Criteria criteria);


    set<int> getPerfectCriteriaMatcher(){
        auto toRet = set<int>();
        for(auto choosed:supportedDevices){
            auto Update = set<int>();
            if(toRet.empty())
                set_union(toRet.begin(),toRet.end(),choosed.second.begin(),choosed.second.end(), inserter(Update,Update.begin()));
            else
                set_intersection(toRet.begin(),toRet.end(),choosed.second.begin(),choosed.second.end(), inserter(Update,Update.begin()));

            toRet = Update;
        }

        return toRet;
    }

    void setVerbosity(bool verbose){
        this->verbose = verbose;
    }

    LogicaDevice getLogicalDevice(int off){
        return this->logicalDevices[off];
    }

};
template<
        typename Device,
        typename Instance,
        typename SurfaceObj,
        typename SwapChainDetail,
        typename DeviceFeature,
        typename LogicaDevice,
        typename Engine,
        typename Queue,
        typename QueueFlags,
        typename Images,
        typename SwapChain,
        typename SwapChainFormat,
        typename SwapChainExtend
>
vector<Device>
PhysicalDevice<
        Device,
        Instance,
        SurfaceObj,
        SwapChainDetail,
        DeviceFeature,
        LogicaDevice,
        Engine,
        Queue,
        QueueFlags,
        Images,
        SwapChain,
        SwapChainFormat,
        SwapChainExtend
>::devices;


template <
        typename BufferType,
        typename ConfigurationFlags,
        typename PropertyFlags,
        typename DeviceMemory,
        typename Device,
        typename PhyDevice,
        typename BufferDataType
        >
class Buffer{
protected:
    //todo write setters for them
    uint64_t size;
    uint64_t bufferSize;
    vector<BufferDataType> data;

    ConfigurationFlags usage;
    BufferType buffer;
    PropertyFlags properties;
    DeviceMemory bufferMemory;
    Device device;
    PhyDevice physicalDevice;

    void* dataPointer;

    virtual void createBuffer() = 0;
    virtual uint32_t findMemoryType(uint32_t typeFilter,PropertyFlags properties) = 0;
public:
    Buffer(int size = 100){
        data =  vector<float>(size);


        this->size = size;
        this->bufferSize = sizeof(BufferDataType)*this->size;
    }
    Buffer(const Buffer& toCopy){
        this->usage = toCopy.usage;
        this->properties = toCopy.properties;
        this->bufferMemory = toCopy.bufferMemory;
        this->device = toCopy.device;
        this->physicalDevice = toCopy.physicalDevice;
    }

    void setUsage(const ConfigurationFlags toSet){
        this->usage = toSet;
    }

    void setPhysicalDevice(PhyDevice phydevice){
        this->physicalDevice = phydevice;
    }

    void setLogicalDevice(Device device){
        this->device = device;
    }

    virtual void create() = 0;
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


    map<ErroCode,string> errorStorer;
    void (*framebufferResizeCallback)(WindowObj, int, int);

public:
    bool enableValidationLayers = true;
    std::vector<const char*> validationLayers;
    virtual bool checkValidationLayerSupport() =0;


    virtual void setAppConfig(Config config = Config()){appConfiguration = config;};
    virtual void setInstance() = 0;
    Instance getInstance(){return this->instance;}
    virtual void createSurface() = 0;

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
    virtual void initWindow(int w,int h) = 0;
    virtual void defaultWindowResizeHandler(WindowObj* window, int width, int height) = 0;
    virtual void setResizeWindowCallBack(void* newFunc) = 0;
    virtual void mainLoop() = 0;

    //debug helper
    virtual void setupDebugMessenger(void (*callBack)()) = 0;
    virtual void setupDebugCallback() = 0;


    //cleanup methods
    virtual void cleanup() = 0;

    virtual std::vector<const char*> getRequiredInstanceExtensions() = 0;

    WindowObj* getWindow(){return window;}
    SurfaceObj getSurface(){return surface;}

};


class Shader{
protected:
    string filename;
    vector<char> data;
    ifstream  file;

public:
    Shader(string filename):filename(filename){

    }

    template<typename... T>
    void create(T...);

    template<typename... T>
    void destroy(T...);

    std::vector<char> readFile() {
        file = std::ifstream(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        data = vector<char>(buffer);
        return buffer;
    }
};

class Pipeline{
    vector<Shader> shaders;

public:
    Pipeline(){

    }

    template<typename... T>
    void create(T...);

    void addShader(Shader& shader){this->shaders.push_back(shader);};
};

#endif //GRAPHICFRAMEWORK
