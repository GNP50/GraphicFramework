//
// Created by Pierluigi Carlino on 05/03/22.
//

#ifndef ASS0_GEOMETRIC_H
#define ASS0_GEOMETRIC_H

#include <stdlib.h>
#include <glm/glm.hpp>
#include <string>

using namespace  std;

namespace GraphicLibrary {
    class IStringable
    {
    public:
        virtual ~IStringable() {}
        virtual constexpr string toString() = 0;

    };


    class IGeometric
    {
    public:
        virtual ~IGeometric() {}
        virtual constexpr string variables() = 0;
        virtual constexpr string transformations() = 0;
        virtual constexpr string vertexBuffer() = 0;

    };


    class Vertex: public IStringable{
    public:
        string toString();
    };


    class Geometric:public IGeometric,IStringable{
    private:

    public:
        Geometric();

        void translate(float x, float y, float z = 0.0);

        void dilatate(float x = 1.0, float y = 1.0, float z = 1.0);

        virtual void rotate(float x = 1.0, float y = 1.0, float z = 1.0);
        virtual void rotate(glm::vec3 rotateAxis);

        string toString();
    };

}

#endif //ASS0_GEOMETRIC_H
