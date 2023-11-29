/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef VEC2_H
#define VEC2_H

#include <string>
#include <assert.h>
#include <ostream>
#include <util/Concepts.hpp>
#include <fmt/format.h>

namespace engine {
    template<Arithmetic T>
    struct Vec2 {

        T x;
        T y;

        Vec2() : x(T(1)), y(T(1)) {}

        Vec2(T v) : x(v), y(v) {}

        Vec2(T x, T y) : x(x), y(y) {}

        template <typename Type2> Vec2(const Vec2<Type2>& other) : x(other.x), y(other.y) {}

        float length() const {
            return sqrt(x * x + y * y);
        };

        static T dot(const Vec2& first, const Vec2& second) {
            return first.x * second.x + first.y * second.y;
        }

        static Vec2 normalised(const Vec2& vec) {
            float len = vec.length();
            assert(len != 0.0f);
            T inv_length = T(1) / len;
            return Vec2(vec.x * inv_length, vec.y * inv_length);
        }

        void normalise() {
            float len = length();
            assert(len != 0.0f);
            T inv_length = T(1) / len;
            x *= inv_length;
            y *= inv_length;
        }

        Vec2 operator + (const Vec2& other) const {
            return Vec2(x + other.x, y + other.y);
        }

        void operator += (const Vec2& other) {
            x += other.x;
            y += other.y;
        }

        Vec2 operator - (const Vec2& other) const {
            return Vec2(x - other.x, y - other.y);
        }

        void operator -= (const Vec2& other) {
            x -= other.x;
            y -= other.y;
        }

        Vec2 operator * (T scalar) const {
            return Vec2(x * scalar, y * scalar);
        }

        void operator *= (T scalar) {
            x *= scalar;
            y *= scalar;
        }

        Vec2 operator / (T scalar) const {
            assert(scalar != T(0));
            T inverse_scalar = T(1) / scalar;
            return Vec2(x * inverse_scalar, y * inverse_scalar);
        }

        void operator /= (T scalar) {
            assert(scalar != T(0));
            T inverse_scalar = T(1) / scalar;
            x *= inverse_scalar;
            y *= inverse_scalar;
        }

        bool operator == (const Vec2& other) const {
            return x == other.x && y == other.y;
        }

        bool operator != (const Vec2& other) const {
            return !(*this == other);
        }

        operator std::string() const {
            return fmt::format("({}, {})", x, y);
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec2& vec) {
            os << fmt::format("({}, {})", vec.x, vec.y);
            return os;
        }
    };
};


namespace std {
    template<> struct hash<engine::Vec2<int>> {
        std::size_t operator()(engine::Vec2<int>const& vec) const noexcept {
            size_t h = (size_t(vec.x) << 32) + size_t(vec.y);
            h *= 1231231557ull; // "random" uneven integer 
            h ^= (h >> 32);
            return h;
        }
    };
}

#endif // !VEC2_H

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