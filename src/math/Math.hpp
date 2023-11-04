/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <math/Vec3.hpp>
#include <math/Mat4.hpp>

namespace engine {
	constexpr float PI = 3.14159265358979323846f;
	constexpr float PI_OVER_180 = PI / 180.0f;
	constexpr float ONE_EIGHTY_OVER_PI = 180.0f / PI;

	inline float radians(float theta) {
		return theta * PI_OVER_180;
	}

	inline float degrees(float theta) {
		return theta * ONE_EIGHTY_OVER_PI;
	}

    inline Mat4<float> translationRemoved(const Mat4<float>& mat)
    {
        return Mat4(
            mat[0], mat[1], mat[2], 0.0f,
            mat[4], mat[5], mat[6], 0.0f,
            mat[8], mat[9], mat[10], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4<float> translate(const Vec3<float>& vec)
    {
        return Mat4(
            1.0f, 0.0f, 0.0f, vec.x,
            0.0f, 1.0f, 0.0f, vec.y,
            0.0f, 0.0f, 1.0f, vec.z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4<float> scale(const Vec3<float>& vec)
    {
        return Mat4(
            vec.x, 0.0f, 0.0f, 0.0f,
            0.0f, vec.y, 0.0f, 0.0f,
            0.0f, 0.0f, vec.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4<float> rotate(const Vec3<float>& axis, float theta)
    {  
        
        return Mat4<float>(
            cos(theta) + axis.x * axis.x * (1 - cos(theta)), axis.x * axis.y * (1 - cos(theta)) - axis.z * sin(theta), axis.x * axis.z * (1 - cos(theta)) + axis.y * sin(theta), 0,
            axis.y * axis.x * (1 - cos(theta)) + axis.z * sin(theta), cos(theta) + axis.y * axis.y * (1 - cos(theta)), axis.y * axis.z * (1 - cos(theta)) - axis.x * sin(theta), 0,
            axis.z * axis.x * (1 - cos(theta)) - axis.y * sin(theta), axis.z * axis.y * (1 - cos(theta)) + axis.x * sin(theta), cos(theta) + axis.z * axis.z * (1 - cos(theta)), 0,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4<float> perspective(float fov, float aspect, float near_dist, float far_dist)
    {
        const float f = 1.0f / tan(fov / 2.0f);

        return Mat4(
            f / aspect, 0.0f, 0.0f, 0.0f,
            0.0f, f, 0.0f, 0.0f,
            0.0f, 0.0f, (far_dist + near_dist) / (near_dist - far_dist), (2 * far_dist * near_dist) / (near_dist - far_dist),
            0.0f, 0.0f, -1.0f, 0.0f
        );
    }

    inline Mat4<float> orthographic(float bottom, float top, float left, float right, float _near, float _far)
    {
        return Mat4(
            2.0f / (right - left), 0.0f, 0.0f, -((right + left) / (right - left)),
            0.0f, 2.0f / (top - bottom), 0.0f, -((top + bottom) / (top - bottom)),
            0.0f, 0.0f, -2.0f/(_far - _near), -((_far+_near)/(_far-_near)),
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4<float> lookAt(const Vec3<float>& eye, const Vec3<float>& center, const Vec3<float>& up)
    {
        Vec3<float>  f = Vec3<float>::normalised(center - eye);
        Vec3<float> u = Vec3<float>::normalised(up);
        Vec3<float>  s = Vec3<float>::normalised(Vec3<float>::cross(f, u));
        u = Vec3<float>::cross(s, f);

        return Mat4(
            s.x, s.y, s.z, -Vec3<float>::dot(s, eye),
            u.x, u.y, u.z, -Vec3<float>::dot(u, eye),
            -f.x, -f.y, -f.z, Vec3<float>::dot(f, eye),
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

}

#endif // !MATH_H

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