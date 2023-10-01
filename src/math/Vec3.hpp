/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef VEC3_H
#define VEC3_H

#include <string>
#include <assert.h>
#include <ostream>
#include <util/Concepts.hpp>
#include <fmt/format.h>

namespace engine {
    template<Arithmetic T>
	struct Vec3 {

		T x;
		T y;
		T z;

		Vec3() : x(T(0)), y(T(0)), z(T(0)) {}

		Vec3(T v) : x(v), y(v), z(v) {}

		Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

		float length() const {
            return sqrt(x * x + y * y + z * z);
        }

	    static T dot(const Vec3& first, const Vec3& second) {
            return first.x * second.x + first.y * second.y + first.z * second.z;
        }

		static Vec3 cross(const Vec3& first, const Vec3& second) {
            return Vec3(
                first.y * second.z - first.z * second.y,
                first.z * second.x - first.x * second.z,
                first.x * second.y - first.y * second.x
            );
        }

		static Vec3 normalised(const Vec3& vec) {
            float len = vec.length();
            assert(len != 0.0f);
            T inv_length = T(1) / len;
            return Vec3(vec.x * inv_length, vec.y * inv_length, vec.z * inv_length);
        }

		void normalise() {
            float len = length();
            assert(len != 0.0f);
            T inv_length = T(1) / len;
            x *= inv_length;
            y *= inv_length;
            z *= inv_length;
        }

        Vec3 abs() {
            return Vec3(
                T(std::abs(x)),
                T(std::abs(y)),
                T(std::abs(z))
            );
        }

		Vec3 operator+(const Vec3& other) const {
            return Vec3(x + other.x, y + other.y, z + other.z);
        }

		void operator+=(const Vec3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
        }

		Vec3 operator-(const Vec3& other) const {
            return Vec3(x - other.x, y - other.y, z - other.z);
        }

		void operator-=(const Vec3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
        }

		Vec3 operator*(T scalar) const {
            return Vec3(x * scalar, y * scalar, z * scalar);
        }

		void operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
        }

		Vec3 operator/(T scalar) {
            assert(scalar != T(0));
            T inverse_scalar = T(1) / scalar;
            return Vec3(x * scalar, y * scalar, z * scalar);
        }

		void operator/=(T scalar) {
            assert(scalar != T(0));
            T inverse_scalar = T(1) / scalar;
            x *= inverse_scalar;
            y *= inverse_scalar;
            z *= inverse_scalar;
        }

		bool operator==(const Vec3& other) const {
            return x == other.x && y == other.y && z == other.z;
        }

		bool operator!=(const Vec3& other) const {
            return !(*this == other);
        }
        
        Vec3 operator%(T num) const requires (std::is_integral_v<T>) {
            return Vec3(
                x % num,
                y % num,
                z % num
            );
        }

        void operator%=(T num) requires (std::is_integral_v<T>) {
           x %= num;
           y %= num;
           z %= num;
        }

		operator std::string() {
            return fmt::format("({}, {}, {})", x, y, z);
        }

		friend std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
            os << fmt::format("({}, {}, {})", vec.x, vec.y, vec.z);
	        return os;
        }
	};
}

#endif // !VEC3_H

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