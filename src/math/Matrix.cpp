/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <math/Matrix.hpp>
#include <math/Vector.hpp>
#include <util/Concepts.hpp>
#include <sstream>
#include <cmath>

/*
=====================
Arithmetic operations
=====================
*/

template<Arithmetic T, std::size_t size>
Matrix<T, size> Matrix<T, size>::identity()
{
    Matrix<T, size> result{};
    for (std::size_t i = 0; i < size; i++) {
        result[i * size + i] = T(1);
    }
    return result;
}

template<Arithmetic T, std::size_t size>
Matrix<T, size> Matrix<T, size>::operator*(const Matrix<T, size>& other) const
{
    Matrix<T, size> result;
    for (std::size_t i = 0; i < size; i++) {
        for (std::size_t j = 0; j < size; j++) {
            T sum = 0;
            for (std::size_t k = 0; k < size; k++) {
                sum = sum + elems[i * size + k] * other[k * size + j];
            }
            result[i * size + j] = sum;
        }
    }
    return result;
}

template<Arithmetic T, std::size_t size>
void Matrix<T, size>::operator*=(const Matrix<T, size>& other)
{
    *this = *this * other;
}

/*
===========
Data access
===========
*/

template<Arithmetic T, std::size_t size>
T& Matrix<T, size>::operator[](std::size_t idx) {
    return elems[idx];
}

template<Arithmetic T, std::size_t size>
T Matrix<T, size>::operator[](std::size_t idx) const {
    return elems[idx];
}

template<Arithmetic T, std::size_t size>
const T* Matrix<T, size>::GetPointer() const
{
    return &elems[0];
}

/*
=================
String conversion
=================
*/

template<Arithmetic T, std::size_t size>
Matrix<T, size>::operator std::string()
{
    std::ostringstream oss;
    for (size_t i = 0; i < size; ++i) {
        std::copy(elems.begin() + i * size, elems.begin() + (i + 1) * size, std::ostream_iterator<T>(oss, " "));
        if (i < size - 1) {
            oss << '\n';
        }
    }
    return oss.str();
}

/*
================================
Explicit template instantiations
================================
*/
template struct Matrix<float, 2>;
template struct Matrix<float, 3>;
template struct Matrix<float, 4>;

/*
======================
Matrix transformations
======================
*/
Mat4 translationRemoved(const Mat4& mat)
{
    return Mat4{
        mat[0], mat[1], mat[2], 0.0f,
        mat[4], mat[5], mat[6], 0.0f,
        mat[8], mat[9], mat[10], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

Mat4 translate(const Vec3& vec)
{
    return Mat4{
        1.0f, 0.0f, 0.0f, vec[0],
        0.0f, 1.0f, 0.0f, vec[1],
        0.0f, 0.0f, 1.0f, vec[2],
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

Mat4 scale(const Vec3& vec)
{
    return Mat4{
        vec[0], 0.0f, 0.0f, 0.0f,
        0.0f, vec[1], 0.0f, 0.0f,
        0.0f, 0.0f, vec[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

Mat4 rotate(const Vec3& axis, float theta)
{
    float cos_theta = std::cos(theta);
    float sin_theta = std::sin(theta);

    return Mat4{
        cos_theta + axis[0] * axis[0] * (1 - cos_theta), axis[0] * axis[1] * (1 - cos_theta) - axis[2] * sin_theta, axis[0] * axis[2] * (1 - cos_theta) + axis[1] * sin_theta, 0.0f,
        axis[1] * axis[0] * (1 - cos_theta) + axis[2] * sin_theta, cos_theta + axis[1] * axis[1] * (1 - cos_theta), axis[1] * axis[2] * (1 - cos_theta) - axis[0] * sin_theta, 0.0f,
        axis[2] * axis[0] * (1 - cos_theta) - axis[1] * sin_theta, axis[2] * axis[1] * (1 - cos_theta) + axis[0] * sin_theta, cos_theta + axis[2] * axis[2] * (1 - cos_theta), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

Mat4 perspective(float fov, float aspect, float near_dist, float far_dist)
{
    const float f = 1.0f / tan(fov / 2.0f);

    return Mat4{
        f / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, f, 0.0f, 0.0f,
        0.0f, 0.0f, (far_dist + near_dist) / (near_dist - far_dist), (2 * far_dist * near_dist) / (near_dist - far_dist),
        0.0f, 0.0f, -1.0f, 0.0f
    };
}

Mat4 orthographic(float bottom, float top, float left, float right, float _near, float _far)
{
    return Mat4{
        2.0f / (right - left), 0.0f, 0.0f, -((right + left) / (right - left)),
        0.0f, 2.0f / (top - bottom), 0.0f, -((top + bottom) / (top - bottom)),
        0.0f, 0.0f, -2.0f / (_far - _near), -((_far + _near) / (_far - _near)),
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
{
    Vec3 f = (center - eye).normalized();
    Vec3 u = up.normalized();
    Vec3 s = (f.cross(u)).normalized();
    u = s.cross(f);

    return Mat4{
        s[0], s[1], s[2], -s.dot(eye),
        u[0], u[1], u[2], -u.dot(eye),
        -f[0], -f[1], -f[2], f.dot(eye),
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

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