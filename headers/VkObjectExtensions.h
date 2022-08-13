//
// Created by Pierluigi Carlino on 06/08/22.
//

#ifndef DB_VKOBJECTEXTENSIONS_H
#define DB_VKOBJECTEXTENSIONS_H

#include "VulkanFramework.h"
#include "VulkanGeomtric.h"
#include "VkObject.h"
#include "basicGeometry.hpp"


namespace Vulkan {

    struct PositionBuffer {
        alignas(16) vec4 position;
    };

    class Point : public TexturedRender {
        UniformBufferObject *hostObj;
        vec3  position = {0.0,0.0,0.0};
        float ratio;
        float plainFactor;
        vec3 viewPosition;

    public:

        void setViewPosition(const vec3 &viewPosition){
            this->viewPosition = viewPosition;
            hostObj->mvpMat =  TransfBuff::instance()->addTranslation(viewPosition).doTransformations<glm::mat4>()*ProjectBuffer::instance(1.0,this->n,this->f)->perspectiveProj(this->a)*glm::lookAt(
                    glm::vec3({0.0, 0.0, 0.0}),
                    glm::vec3({0.0, 0.0, -1.0}),
                    glm::vec3({0.0, 1.0, 0.0})
            );

            this->gubo->updateData(*hostObj);
        }


        Point(Enviroment &env, string textPath,float ratio = 5,float plainFactor = 5) :plainFactor(plainFactor),ratio(ratio){
            this->vertexPath = "shaders/PointTrackerVertex.spv";
            this->fragmPath = "shaders/PointTrackerFragment.spv";



            hostObj = new UniformBufferObject();
            hostObj->mMat = TransfBuff::instance()
                    ->addTranslation({0, 0, 1}, 2)
                    .addCenteredScaling(15).doTransformations<glm::mat4>();

            hostObj->nMat = mat4(1.0f);


            auto gubo = std::make_shared<UniformBuffer<UniformBufferObject>>(
                    *(new UniformBuffer<UniformBufferObject>()));
            gubo->createBuffers(*env.getSwapChain(), *hostObj);

            init_texture(env.getRepository(), env.getSwapChain(), textPath, gubo);
            hostObj->mvpMat =  TransfBuff::instance()->addTranslation(viewPosition).doTransformations<glm::mat4>()*ProjectBuffer::instance(1.0,this->n,this->f)->perspectiveProj(this->a)*glm::lookAt(
                    glm::vec3({0.0, 0.0, 0.0}),
                    glm::vec3({0.0, 0.0, -1.0}),
                    glm::vec3({0.0, 1.0, 0.0})
            );
            this->gubo->updateData(*hostObj);

            vector<glm::vec3> vert;
            vector<uint32_t> ind;

            auto sph = SlicedSphere(15, 15, 0.5);
            sph.createVertexList();
            sph.createIndiciesList();

            vert = sph.getVerticies<std::vector<glm::vec3>>();
            ind = sph.getIndiciesList<std::vector<uint32_t>>();

            createObject();

            textCoords.push_back({0.0, 0.0});
            textCoords.push_back({1.0, 0.0});
            textCoords.push_back({1.0, 1.0});

            textCoords.push_back({0.0, 0.0});
            textCoords.push_back({1.0, 0.0});
            textCoords.push_back({0.0, 1.0});

            TexturedRender::setVertexesAndIndicies(vert, ind);
            TexturedRender::createModel();

            createPipeline(*this->swapChain);

            env.addRender(this);
        }

        void setPosition(const vec3& pos){
            this->position = pos;

            float a_cos = glm::dot(vec3 ({1,0,0}),pos)/(glm::length(pos));

            float angle = glm::degrees(acos(a_cos));

            hostObj->mMat = TransfBuff::instance()
                    ->addTranslation(pos*ratio)
                    .addCenteredScaling(plainFactor*ratio)
                    .addZRotation(angle)
                    .addXRotation(80)
                    .addTranslation({0, 0, 1}, 2).doTransformations<glm::mat4>();

            this->gubo->updateData(*hostObj);
        }

    };

    class VKPositionMap : public TexturedRender {
        UniformBufferObject *hostObj;
        vec3 position;
        Point* point;
        float factor;

        vec3 viewPosition;



        virtual void update(GLFWwindow *window) {

        }

    public:
        void setDs(const vec3 &ds) {
            position = ds;
            position *=2;
            position +=glm::vec3 ({-1.0,-1.0,0.0});
            position.y = -position.y;

            point->setPosition(position);

            float a_cos = glm::dot(vec3 ({1,0,0}),position)/(glm::length(position));

            float angle = glm::degrees(acos(a_cos));

            hostObj->mMat = TransfBuff::instance()
                    ->addCenteredScaling(factor)
                    .addZRotation(angle)
                    .addXRotation(80)
                    .addTranslation({0,0,1},4).doTransformations<glm::mat4>();

            this->gubo->updateData(*hostObj);
        }

        VKPositionMap(Enviroment &env, string textPath,float factor = 5.0) : TexturedRender(),factor(factor) {

            this->vertexPath = "shaders/PositionTrackerVertex.spv";
            this->fragmPath = "shaders/PositionTrackerFragment.spv";

            point = new Point(env,"textures/triangle.png",factor/2,factor);

            viewPosition = {1-sqrt(5)/factor,-1+sqrt(2)/factor,0.0};
            point->setViewPosition(viewPosition);

            hostObj = new UniformBufferObject();
            hostObj->nMat = mat4(1.0f);
            hostObj->mvpMat = mat4(1.0f);
            hostObj->mMat = mat4(1.0f);
            hostObj->mMat = TransfBuff::instance()
                    ->addCenteredScaling(factor)
                    .addXRotation(80)
                    .addTranslation({0,0,1},4)
                    .doTransformations<glm::mat4>();


            auto gubo = std::make_shared<UniformBuffer<UniformBufferObject>>(
                    *(new UniformBuffer<UniformBufferObject>()));
            gubo->createBuffers(*env.getSwapChain(), *hostObj);

            init_texture(env.getRepository(), env.getSwapChain(), textPath, gubo);
            hostObj->mvpMat =TransfBuff::instance()->addTranslation(viewPosition).doTransformations<glm::mat4>()*ProjectBuffer::instance(1.0,this->n,this->f)->perspectiveProj(this->a)*glm::lookAt(
                    glm::vec3({0.0, 0.0, 0.0}),
                    glm::vec3({0.0, 0.0, -1.0}),
                    glm::vec3({0.0, 1.0, 0.0})
            );

            this->gubo->updateData(*hostObj);


            vector<glm::vec3> vert;
            vector<uint32_t> ind;

            vert.push_back(glm::vec3(-1, 1, 0));
            vert.push_back(glm::vec3(1, 1, 0));
            vert.push_back(glm::vec3(1, -1, 0));

            vert.push_back(glm::vec3(-1, -1, 0));
            vert.push_back(glm::vec3(-1, 1, 0));
            vert.push_back(glm::vec3(1, -1, 0));


            ind.push_back(0);
            ind.push_back(1);
            ind.push_back(2);

            ind.push_back(3);
            ind.push_back(4);
            ind.push_back(5);

            createObject();

            textCoords.push_back({0.0, 1.0});
            textCoords.push_back({1.0, 1.0});
            textCoords.push_back({1.0, 0.0});

            textCoords.push_back({0.0, 0.0});
            textCoords.push_back({0.0, 1.0});
            textCoords.push_back({1.0, 0.0});

            TexturedRender::setVertexesAndIndicies(vert, ind);
            TexturedRender::createModel();

            createPipeline(*this->swapChain);

            env.addRender(this);


        }


    };
}
#endif //DB_VKOBJECTEXTENSIONS_H
