/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include "Vec3.hpp"
#include <assert.h>
#include <fmt/format.h>
#include <cmath>

engine::Vec3::Vec3() : x(1.0f), y(0.0f), z(0.0f)
{

}

engine::Vec3::Vec3(float v): x(v), y(v), z(v)
{
}

engine::Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z)
{

}

float engine::Vec3::length() const
{
	return sqrt(x * x + y * y + z * z);
}

float engine::Vec3::dot(const Vec3& first, const Vec3& second)
{
	return first.x * second.x + first.y * second.y + first.z * second.z;
}

engine::Vec3 engine::Vec3::cross(const Vec3& first, const Vec3& second)
{
	return Vec3(
		first.y * second.z - first.z * second.y,
		first.z * second.x - first.x * second.z,
		first.x * second.y - first.y * second.x
	);
}

engine::Vec3 engine::Vec3::normalised(const Vec3& vec)
{
	float len = vec.length();
	assert(len != 0);
	float inv_length = 1 / len;
	return Vec3(vec.x * inv_length, vec.y * inv_length, vec.z * inv_length);
}

void engine::Vec3::normalise()
{
	float len = length();
	assert(len != 0);
	float inv_length = 1 / len;
	x *= inv_length;
	y *= inv_length;
	z *= inv_length;
}

engine::Vec3 engine::Vec3::operator+(const Vec3& other) const
{
	return Vec3(x + other.x, y + other.y, z + other.z);
}

void engine::Vec3::operator+=(const Vec3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
}

engine::Vec3 engine::Vec3::operator-(const Vec3& other) const
{
	return Vec3(x - other.x, y - other.y, z - other.z);

}

void engine::Vec3::operator-=(const Vec3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
}

engine::Vec3 engine::Vec3::operator*(float scalar) const
{
	return Vec3(x * scalar, y * scalar, z * scalar);
}

void engine::Vec3::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

engine::Vec3 engine::Vec3::operator/(float scalar)
{
	assert(scalar != 0.0f);
	float inverse_scalar = 1.0f / scalar;
	return Vec3(x * scalar, y * scalar, z * scalar);
}

void engine::Vec3::operator/=(float scalar)
{
	assert(scalar != 0.0f);
	float inverse_scalar = 1.0f / scalar;
	x *= inverse_scalar;
	y *= inverse_scalar;
	z *= inverse_scalar;
}

bool engine::Vec3::operator==(const Vec3& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

bool engine::Vec3::operator!=(const Vec3& other) const
{
	return !(*this == other);
}

engine::Vec3::operator std::string() {
	return fmt::format("({}, {}, {})", x, y, z);
}

std::ostream& engine::operator<<(std::ostream& os, const Vec3& vec)
{
	os << fmt::format("({}, {}, {})", vec.x, vec.y, vec.z);
	return os;
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