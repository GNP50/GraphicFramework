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
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string.h>


using namespace std;


template<typename Ret>
class IReturnable {
public:
    virtual ~IReturnable() = default;

    virtual Ret &returnObj() = 0;

};


template<typename Ret, typename StructTp>
class AppConfiguration : public IReturnable<Ret> {
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
    vector<const char *> extension;


    AppConfiguration() {
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

    virtual void setAppMajor(int appMajor = 1) {
        this->appMajor = appMajor;
    }

    virtual void setAppMinor(int appMinor = 0) {
        this->appMinor = appMinor;
    }

    virtual void setAppPatch(int appPatch = 0) {
        this->appPatch = appPatch;
    }

    virtual void setEngineMajor(int engineMajor = 1) {
        this->engineMajor = engineMajor;
    }

    virtual void setEngineMinor(int engineMinor = 0) {
        this->engineMinor = engineMinor;
    }

    virtual void setEnginePatch(int enginePatch = 0) {
        this->enginePatch = enginePatch;
    }

    virtual void setEngineName(string engineName = "No Name") {
        this->engineName = engineName;
    }

    virtual void setAppName(string appName = "No name") {
        this->appName = appName;
    }


    virtual void setApiVersion(int apiVersion = 1) {
        this->apiVersion = apiVersion;
    }

    virtual void setStructType(StructTp structTp) {
        this->structureType = structTp;
    }

    virtual void setRequiredExtension(bool enableValidationLayers) = 0;

    virtual Ret &returnObj() = 0;
};

template<
        typename GraphicPipeLine,
        typename PipelineLayout,
        typename StageCreationInfo,
        typename VertexInputCreateInfo,
        typename InputAssemblyCreateInfo,
        typename RasterizationStateCreateInfo,
        typename MultisampleStateCreateInfo,
        typename ColorBlendAttachmentState,
        typename ColorBlendStateCreateInfo,
        typename LayoutCreateInfo,
        typename DepthStencilStateCreateInfo,
        typename PipelineCreateInfo,
        typename FragShader,
        typename VertexShader,
        typename ViewPort,
        typename Scissor,
        typename SetLayoutBinding,
        typename SetLayoutCreateInfo,
        typename SetLayout>
class GenericGraphicPipeline{
protected:
    StageCreationInfo stageCreationInfo = {};
    FragShader fragmentShader;
    VertexShader vertexShader;
public:
    StageCreationInfo getStageCreationInfo() const {
        return stageCreationInfo;
    }

    FragShader getFragmentShader() const {
        return fragmentShader;
    }

    VertexShader getVertexShader() const {
        return vertexShader;
    }

    VertexInputCreateInfo getVertexInputInfo() const {
        return vertexInputInfo;
    }

    InputAssemblyCreateInfo getInputAssembly() const {
        return inputAssembly;
    }

    ViewPort getViewport() const {
        return viewport;
    }

    Scissor getScissor() const {
        return scissor;
    }

    RasterizationStateCreateInfo getRasterizer() const {
        return rasterizer;
    }

    MultisampleStateCreateInfo getMultisampling() const {
        return multisampling;
    }

    ColorBlendAttachmentState getColorBlendAttachment() const {
        return colorBlendAttachment;
    }

    ColorBlendStateCreateInfo getColorBlending() const {
        return colorBlending;
    }

    LayoutCreateInfo getPipelineLayoutInfo() const {
        return pipelineLayoutInfo;
    }

    DepthStencilStateCreateInfo getDepthStencil() const {
        return depthStencil;
    }

    PipelineCreateInfo getPipelineInfo() const {
        return pipelineInfo;
    }

    SetLayoutBinding getUboLayoutBinding() const {
        return uboLayoutBinding;
    }

    SetLayoutBinding getSamplerLayoutBinding() const {
        return samplerLayoutBinding;
    }

    SetLayoutCreateInfo getLayoutInfo() const {
        return layoutInfo;
    }

    SetLayout getDescriptorSetLayout() const {
        return descriptorSetLayout;
    }

    PipelineLayout getPipelineLayout() const {
        return pipelineLayout;
    }

    GraphicPipeLine getGraphicsPipeline() const {
        return graphicsPipeline;
    }




protected:
    VertexInputCreateInfo vertexInputInfo = {};
    InputAssemblyCreateInfo inputAssembly = {};
    ViewPort viewport = {};
    Scissor scissor = {};
    RasterizationStateCreateInfo rasterizer = {};
    MultisampleStateCreateInfo multisampling = {};
    ColorBlendAttachmentState colorBlendAttachment = {};
    ColorBlendStateCreateInfo colorBlending = {};
    LayoutCreateInfo pipelineLayoutInfo = {};
    DepthStencilStateCreateInfo depthStencil = {};
    PipelineCreateInfo pipelineInfo = {};
    SetLayoutBinding uboLayoutBinding = {};
    SetLayoutBinding samplerLayoutBinding = {};
    SetLayoutCreateInfo layoutInfo = {};
    SetLayout descriptorSetLayout = {};
    PipelineLayout pipelineLayout = {};
    GraphicPipeLine graphicsPipeline = {};


public:
    GenericGraphicPipeline(FragShader& fragShader, VertexShader& vertexShader):
        fragmentShader(fragShader),
        vertexShader(vertexShader){

    };

    GenericGraphicPipeline()  = delete;


};
template<
        typename SwapChain,
        typename SwapChainFormat,
        typename SwapChainDetail,
        typename SwapChainExtend,
        typename Images,
        typename ImageView
>
class GenericSwapChain {
protected:
    SwapChainFormat swapChainImageFormat = {};
    SwapChainExtend swapChainExtent = {};
    SwapChain swapChain = {};

    std::vector<Images> swapChainImages;
    std::vector<ImageView> swapChainImageViews;

    map<const std::type_index, set<int>> supportedCapabilities;
    bool verbose = true;
    bool isCreated = false;
public:
    SwapChainFormat const getSwapChainImageFormat(){return swapChainImageFormat;};
    SwapChainExtend const getSwapChainExtent(){return swapChainExtent;};
    SwapChain const getSwapChain(){return swapChain;};
    std::vector<Images> const getSwapChainImages(){return swapChainImages;};
    std::vector<ImageView> const getSwapChainImageViews(){return swapChainImageViews;};




};


template<
        typename Device,
        typename Instance,
        typename SurfaceObj,
        typename DeviceFeature,
        typename LogicaDevice,
        typename Engine,
        typename Queue,
        typename QueueFlags
>
class PhysicalDevice {
protected:
    static vector<Device> devices;
    map<const std::type_index, set<int>> supportedDevices;
    std::vector<const char *> deviceExtensions;

    SurfaceObj surface;
    Engine engine;
    Instance instance;
    vector<LogicaDevice> logicalDevices;


    bool verbose = false;

    virtual void initDevices(Instance instance) = 0;


    virtual bool isFeatureSupported(int offset) = 0;

    virtual DeviceFeature getSupportedExtensions(int offset) = 0;

    virtual bool isDevicesSuitable(vector<Device> devices) = 0;

    template<typename DeviceExtension>
    bool isExtensionSupported(int offset, DeviceExtension requiredExtensions);


public:
    Queue graphicsQueue, presentQueue;
    std::map<int, vector<pair<QueueFlags, int>>> queueIndicies;
    std::map<int, vector<int>> presentIndicies;

    PhysicalDevice(vector<const char *> deviceExts,Engine engine) {
        this->engine = engine;
        this->deviceExtensions = deviceExts;
        this->instance = engine.getInstance();
    }


    virtual bool supportQueues(int off, vector<QueueFlags> flags) = 0;

    Device getDevice(int off) { return devices[off]; }

    virtual LogicaDevice createLogicalDevice(int off) = 0;

    virtual vector<string> getAllDevicesName() = 0;

    virtual string getDeviceName(int off) = 0;

    template<typename T>
    T getUniqueIndicies(int off);

    template<typename Criteria>
    void getFirstSuitable(Criteria criteria);


    set<int> getPerfectCriteriaMatcher() {
        auto toRet = set<int>();
        for (auto choosed: supportedDevices) {
            auto Update = set<int>();
            if (toRet.empty())
                set_union(toRet.begin(), toRet.end(), choosed.second.begin(), choosed.second.end(),
                          inserter(Update, Update.begin()));
            else
                set_intersection(toRet.begin(), toRet.end(), choosed.second.begin(), choosed.second.end(),
                                 inserter(Update, Update.begin()));

            toRet = Update;
        }

        return toRet;
    }

    void setVerbosity(bool verbose) {
        this->verbose = verbose;
    }

    LogicaDevice getLogicalDevice(int off) {
        return this->logicalDevices[off];
    }

    SurfaceObj getSurface() { return surface; };
    SurfaceObj setSurface(SurfaceObj surface) { this->surface =  surface; };

    Engine getEngine(){return engine;}
};

template<
        typename Device,
        typename Instance,
        typename SurfaceObj,
        typename DeviceFeature,
        typename LogicaDevice,
        typename Engine,
        typename Queue,
        typename QueueFlags>
vector<Device>
        PhysicalDevice<
                Device,
                Instance,
                SurfaceObj,
                DeviceFeature,
                LogicaDevice,
                Engine,
                Queue,
                QueueFlags
        >::devices;


template<
        typename BufferType,
        typename ConfigurationFlags,
        typename PropertyFlags,
        typename DeviceMemory,
        typename Device,
        typename PhyDevice,
        typename BufferDataType
>
class Buffer {
protected:
    //todo write setters for them
    uint64_t size;
    uint64_t bufferSize;

    ConfigurationFlags usage;
    BufferType buffer;
    PropertyFlags properties;
    DeviceMemory bufferMemory;
    Device device;
    std::shared_ptr<PhyDevice> physicalDevice;

    void *dataPointer;

    virtual void createBuffer() = 0;

    virtual uint32_t findMemoryType(uint32_t typeFilter, PropertyFlags properties) = 0;

    vector<BufferDataType> data;
public:
    Buffer(int size = 0) {
        data = vector<BufferDataType>(size);


        this->size = size;
        this->bufferSize = sizeof(BufferDataType) * this->size;
    }

    Buffer(const Buffer &toCopy) {
        this->usage = toCopy.usage;
        this->properties = toCopy.properties;
        this->bufferMemory = toCopy.bufferMemory;
        this->device = toCopy.device;
        this->physicalDevice = toCopy.physicalDevice;
    }

    void setData(vector<BufferDataType> data){
        this->data = vector<BufferDataType>(data);
        this->size = data.size();
        this->bufferSize = sizeof(BufferDataType) * this->size;
    };


    void setUsage(const ConfigurationFlags toSet) {
        this->usage = toSet;
    }

    void setPhysicalDevice(std::shared_ptr<PhyDevice> phydevice) {
        this->physicalDevice = phydevice;
    }

    void setLogicalDevice(Device device) {
        this->device = device;
    }

    BufferType getBuffer(){return buffer;}


    virtual void create() = 0;
};


template<
        typename Instance,
        typename Config,
        typename ErroCode,
        typename WindowObj,
        typename SurfaceObj
>
class GraphicEngine {
protected:
    Instance instance;
    Config appConfiguration;
    WindowObj *window;
    SurfaceObj surface;



    map<ErroCode, string> errorStorer;

    void (*framebufferResizeCallback)(WindowObj, int, int);

public:
    bool enableValidationLayers = true;
    std::vector<const char *> validationLayers;

    virtual bool checkValidationLayerSupport() = 0;


    virtual void setAppConfig(Config config = Config()) { appConfiguration = config; };

    virtual void setInstance() = 0;

    Instance getInstance() { return this->instance; }

    virtual void createSurface() = 0;

    virtual void setValidationLayers(vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    }) {
        this->validationLayers = validationLayers;
    }


    void printError(ErroCode errCode) {
        Terminal::getInstance().printnl(
                errorStorer.at(errCode), BackGroudColor::red, ForeGroudColor::yellow);
    }

    //window handlers
    virtual void initWindow(int w, int h) = 0;

    virtual void defaultWindowResizeHandler(WindowObj *window, int width, int height) = 0;

    virtual void setResizeWindowCallBack(void *newFunc) = 0;

    virtual bool mainLoop() = 0;

    //debug helper
    virtual void setupDebugMessenger(void (*callBack)()) = 0;

    virtual void setupDebugCallback() = 0;


    //cleanup methods
    virtual void cleanup() = 0;

    virtual std::vector<const char *> getRequiredInstanceExtensions() = 0;

    WindowObj *getWindow() { return window; }

    SurfaceObj getSurface() { return surface; }

};

template<typename StageCreateInfo>
class Shader{
protected:
    string filename;
    vector<char> data;
    ifstream file;
    StageCreateInfo stageInfo = {} ;

public:
    Shader(string filename) : filename(filename) {

    }

    Shader(Shader& shader){
        this->filename = shader.filename;
        this->stageInfo = shader.stageInfo;
        this->data =  vector<char>(shader.data);
    }

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

    StageCreateInfo getStageCreateInfo(){return stageInfo;}
};


#endif //GRAPHICFRAMEWORK
