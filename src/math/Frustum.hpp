/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glad/glad.h>
#include <math/Vector.hpp>

struct Plane
{
    Vec3 normal = { 0.f, 1.f, 0.f }; // unit vector
    GLfloat     distance = 0.f;        // Distance with origin

    Plane() = default;

    Plane(const Vec3& p1, const Vec3& norm)
        : normal(norm.normalized()),
        distance(normal.dot(p1))
    {}

    float getSignedDistanceToPlane(const Vec3& point) const
    {
        return (normal.dot(point)) - distance;
    }
};

struct Frustum
{
    Plane topFace;
    Plane bottomFace;

    Plane rightFace;
    Plane leftFace;

    Plane farFace;
    Plane nearFace;
};

struct BoundingVolume
{
    virtual bool isOnFrustum(const Frustum& camFrustum, const Mat4& transform) const = 0;

    virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;

    bool isOnFrustum(const Frustum& camFrustum) const
    {
        return (isOnOrForwardPlane(camFrustum.leftFace) &&
            isOnOrForwardPlane(camFrustum.rightFace) &&
            isOnOrForwardPlane(camFrustum.topFace) &&
            isOnOrForwardPlane(camFrustum.bottomFace) &&
            isOnOrForwardPlane(camFrustum.nearFace) &&
            isOnOrForwardPlane(camFrustum.farFace));
    };
};

struct Sphere : public BoundingVolume
{
    Sphere() = default;
    Vec3 center{ 0.f, 0.f, 0.f };
    GLfloat radius{ 0.f };

    Sphere(const Vec3& inCenter, float inRadius)
        : BoundingVolume{}, center{ inCenter }, radius{ inRadius }
    {}

    bool isOnOrForwardPlane(const Plane& plane) const final
    {
        return plane.getSignedDistanceToPlane(center) > -radius;
    }

    bool isOnFrustum(const Frustum& camFrustum, const Mat4& transform) const final
    {
        //Check Firstly the result that have the most chance to failure to avoid to call all functions.
        return (isOnOrForwardPlane(camFrustum.leftFace) &&
            isOnOrForwardPlane(camFrustum.rightFace) &&
            isOnOrForwardPlane(camFrustum.farFace) &&
            isOnOrForwardPlane(camFrustum.nearFace) &&
            isOnOrForwardPlane(camFrustum.topFace) &&
            isOnOrForwardPlane(camFrustum.bottomFace));
    };
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