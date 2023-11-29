/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef VEC4_H
#define VEC4_H

#include <string>
#include <assert.h>
#include <ostream>
#include <util/Concepts.hpp>
#include <fmt/format.h>

namespace engine {
    template<Arithmetic T>
    struct Vec4 {

        T x;
        T y;
        T z;
        T w;

        Vec4() : x(T(1)), y(T(1)), z(T(1)), w(T(1)) {}

        Vec4(T v) : x(v), y(v), z(v), w(v) {}

        Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

        template <typename Type2> Vec4(const Vec4<Type2>& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

        float length() const {
            return sqrt(x * x + y * y + z * z + w * w);
        }

        static T dot(const Vec4& first, const Vec4& second) {
            return first.x * second.x + first.y * second.y + first.z * second.z + first.w * second.w;
        }

        static Vec4 normalised(const Vec4& vec) {
            float len = vec.length();
            assert(len != 0.0f);
            T inv_length = T(1) / len;

            return Vec4(vec.x * inv_length, vec.y * inv_length, vec.z * inv_length, vec.z * inv_length);
        }

        void normalise() {
            float len = length();
            assert(len != 0.0f);
            T inv_length = T(1) / len;
            x *= inv_length;
            y *= inv_length;
            z *= inv_length;
            w *= inv_length;
        }

        Vec4 operator+(const Vec4& other) const {
            return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        void operator+=(const Vec4& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
        }

        Vec4 operator-(const Vec4& other) const {
            return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
        }

        void operator-=(const Vec4& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
        }

        Vec4 operator*(T scalar) const {
            return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
        }

        void operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
        }

        Vec4 operator/(T scalar) const {
            assert(scalar != T(0));
            T inverse_scalar = T(1) / scalar;
            return Vec4(x * inverse_scalar, y * inverse_scalar, z * inverse_scalar, w * inverse_scalar);
        }

        void operator/=(T scalar) {
            assert(scalar != T(0));
            T inverse_scalar = T(1) / scalar;
            x *= inverse_scalar;
            y *= inverse_scalar;
            z *= inverse_scalar;
            w *= inverse_scalar;
        }

        bool operator==(const Vec4& other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!=(const Vec4& other) const {
            return !(*this == other);
        }

        operator std::string() const {
            return fmt::format("({}, {}, {}, {})", x, y, z, w);
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec4& vec) {
            os << fmt::format("({}, {}, {}, {})", vec.x, vec.y, vec.z, vec.w);
            return os;
        }
    };
};

#endif // !VEC4_H

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