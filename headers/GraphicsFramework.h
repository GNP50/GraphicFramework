//
// Created by Pierluigi Carlino on 05/03/22.
//

#ifndef GRAPHICFRAMEWORK
#define GRAPHICFRAMEWORK

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
}


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

    string appName;
    string engineName;
    int apiVersion;
    StructTp structureType;

    bool enableValidationLayers;
    Ret appRappresentation;

public:
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

    virtual void setEnableValidationLayers(bool value){enableValidationLayers = value};
    virtual void setRequiredExtension() = 0;
    virtual Ret& returnObj() = 0;
};





template<
        typename Instance,
        typename Config,
        typename ErroCode
>
class GraphicEngine{
protected:
    Instance instance;
    Config appConfiguration;

    static map<ErroCode,string> errorStorer;

public:


    virtual void setAppConfig(Config config){instance = config;};
    virtual void setInstance() = 0;

    void printError(ErroCode errCode){
        Terminal::getInstance().print(
                errorStorer.at(errCode),BackGroudColor::red,ForeGroudColor::yellow);
    }

    //window handlers
    virtual void initWindow(int w,int h);
    virtual void defaultWindowResizeHandler();
    virtual void setResizeWindowCallBack():

};



#endif //GRAPHICFRAMEWORK
