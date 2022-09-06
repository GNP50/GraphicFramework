//
// Created by Pierluigi Carlino on 06/09/22.
//

#ifndef DB_SKYBOXRENDER_H
#define DB_SKYBOXRENDER_H

#include "commonVulkanIncludes.h"
#include "VulkanGeomtric.h"


namespace Vulkan {
    class GenericSkyBox : public TexturedRender {
    private:
        struct GenericSkyBoxBuffer{
            vec4 cameraPosition;
            vec4 cameraDirection;
            float w;
            float h;
            float n;
            float f;
        };

        static GenericSkyBoxBuffer hostCameraInfoBuffer;
        static UniformBuffer<GenericSkyBoxBuffer> *cameraInfoBuffer;
        static shared_ptr<Enviroment> enviroment;
    public:

        GenericSkyBox(const string textPath,
                      Enviroment &env) : TexturedRender() {
            init_texture(env.getRepository(), env.getSwapChain(), textPath, env.getGubo());


            if(enviroment==nullptr)
                enviroment = std::make_shared<Enviroment>(env);

            if(cameraInfoBuffer == nullptr) {
                cameraInfoBuffer = new UniformBuffer<GenericSkyBoxBuffer>();
                cameraInfoBuffer->createBuffers(*this->swapChain,hostCameraInfoBuffer);
                transferCameraDataToGpu();
            }
        }

        void transferCameraDataToGpu(){
            auto camera = enviroment->getCurrentCamera();
            this->hostCameraInfoBuffer.cameraPosition = vec4(camera->getCamPos(),1.0);
            this->hostCameraInfoBuffer.cameraDirection = vec4(camera->getCameraDir(),1.0);

            this->hostCameraInfoBuffer.f = enviroment->getF();
            this->hostCameraInfoBuffer.n = enviroment->getN();
            this->hostCameraInfoBuffer.w = enviroment->getW();
            this->hostCameraInfoBuffer.h = enviroment->getH();

            this->cameraInfoBuffer->updateData(this->hostCameraInfoBuffer);
        }
        void update(GLFWwindow* window){

        }
    };

    class Sky : public TexturedRender {
        shared_ptr<Enviroment> enviroment;

    public:
        Sky(const string textPath,
            Enviroment &env) : TexturedRender() {
            init_texture(env.getRepository(), env.getSwapChain(), textPath, env.getGubo());

            vector<glm::vec3> app = vector<vec3>();

            app.push_back({1, 1, 0});
            app.push_back({1, -1, 0.0});
            app.push_back({-1, -1, 0.0});
            app.push_back({-1, 1, 0.0});

            vector<uint32_t> ind = {0, 1, 2, 2, 0, 3};

            createObject();
            textCoords.push_back({0.0, 0.0});
            textCoords.push_back({1.0, 0.0});
            textCoords.push_back({1.0, 1.0});
            textCoords.push_back({0.0, 1.0});

            TexturedRender::setVertexesAndIndicies(app, ind);
            TexturedRender::createModel();

            createPipeline(*this->swapChain);


            env.addRender(this);
        }

        virtual void update(GLFWwindow *window) override {
            //get the position of the current camera and reset the sky box
        }
    };

}
#endif //DB_SKYBOXRENDER_H
