/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef VECTOR_H
#define VECTOR_H

#include <glad/glad.h>
#include <array>
#include <string>
#include <concepts>
#include <util/Concepts.hpp>
#include <cmath>

template<Arithmetic T, std::size_t size>
struct Vector {
    using value_type = T;
    std::array<T, size> elems{};

    // Vector operations
    float length() const requires (std::integral<T>);
    T length() const requires (std::floating_point<T>);
    T dot(Vector<T, size> other) const;
    Vector<T, 3> cross(const Vector<T, 3> other) const requires (size == 3);
    void normalize() requires (std::floating_point<T>);
    Vector<T, size> normalized() const requires (std::floating_point<T>);

    // Arithmetic operations
    Vector<T, size> operator+(const Vector<T, size>& other) const;
    Vector<T, size> operator-(const Vector<T, size>& other) const;
    Vector<T, size> operator*(T scalar) const;
    Vector<T, size> operator/(T scalar) const;
    void operator+=(const Vector<T, size>& other);
    void operator-=(const Vector<T, size>& other);
    void operator*=(T scalar);
    void operator/=(T scalar);
    void operator%=(T divisor) requires (std::integral<T>);
    Vector<T, size> operator%(T divisor) const requires (std::integral<T>);

    // Comparison operators
    bool operator==(const Vector<T, size>& other) const;
    bool operator!=(const Vector<T, size>& other) const;

    // Data access
    T& operator[](std::size_t idx);
    T operator[](std::size_t idx) const;
    const T* GetPointer() const;

    // Type conversion
    operator std::string() const;
    template<Arithmetic NewT>
    operator Vector<NewT, size>() const {
        Vector<NewT, size> result;
        for (std::size_t i = 0; i < size; i++) {
            result[i] = static_cast<NewT>(elems[i]);
        }
        return result;
    }
};

using Vec2 = Vector<GLfloat, 2>;
using Vec3 = Vector<GLfloat, 3>;
using Vec4 = Vector<GLfloat, 4>;

using iVec2 = Vector<GLint, 2>;
using iVec3 = Vector<GLint, 3>;
using iVec4 = Vector<GLint, 4>;

namespace std {
    template<> struct hash<iVec2> {
        std::size_t operator()(iVec2 const& vec) const noexcept {
            size_t h = (size_t(vec[0]) << 32) + size_t(vec[1]);
            h *= 1231231557ull;
            h ^= (h >> 32);
            return h;
        }
    };
}

#endif //!VECTOR_H

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
