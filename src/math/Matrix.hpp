/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef MATRIX_H
#define MATRIX_H

#include <glad/glad.h>
#include <array>
#include <util/Concepts.hpp>
#include <math/Vector.hpp>

template <Arithmetic T, std::size_t size>
struct Matrix {
    std::array<T, size* size> elems{};

    // Matrix operations
    static Matrix<T, size> identity();

    // Arithmetic operations
    Matrix<T, size> operator*(const Matrix<T, size>& other) const;
    void operator*=(const Matrix<T, size>& other);

    // Data access
    T& operator[](std::size_t idx);
    T operator[](std::size_t idx) const;

    const T* GetPointer() const;
    operator std::string();
};

using Mat2 = Matrix<GLfloat, 2>;
using Mat3 = Matrix<GLfloat, 3>;
using Mat4 = Matrix<GLfloat, 4>;

// Matrix transfomrations
Mat4 translationRemoved(const Mat4& mat);
Mat4 translate(const Vec3& vec);
Mat4 scale(const Vec3& vec);
Mat4 rotate(const Vec3& axis, float theta);
Mat4 perspective(float fov, float aspect, float near_dist, float far_dist);
Mat4 orthographic(float bottom, float top, float left, float right, float _near, float _far);
Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);

#endif // !MATRIX_H

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