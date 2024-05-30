/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glad/glad.h>
#include "glm/glm.hpp"
#include <array>

struct Plane
{
    glm::vec3 normal = glm::vec3( 0.f, 1.f, 0.f );   // unit vector
    GLfloat     distance = 0.f;        // Distance with origin

    Plane() = default;

    Plane(const glm::vec3& p1, const glm::vec3& norm)
        : normal(glm::normalize(norm)),
        distance(glm::dot(normal, p1))
    {}

    float getSignedDistanceToPlane(const glm::vec3& point) const
    {
        return glm::dot(normal, point) - distance;
    }
};

enum class FrustumPlane {
    TOP,
    BOTTOM,
    RIGHT,
    LEFT,
    FAR,
    NEAR
};

using Frustum = std::array<Plane, 6>;

struct Sphere {
    Sphere() = default;
    glm::vec3 center = glm::vec3(0.0f);
    GLfloat radius{ 0.0f };

    Sphere(const glm::vec3& inCenter, float inRadius)
        : center{ inCenter }, radius{ inRadius }
    {}

    bool IsOnFrustum(const Frustum& frustum);
};

#endif // !FRUSTUM_H

/*
MIT License

Copyright (c) 2023 William Redding

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/