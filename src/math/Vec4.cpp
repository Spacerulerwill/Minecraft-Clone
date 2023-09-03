/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include "Vec4.hpp"
#include <assert.h>
#include <fmt/format.h>
#include <cmath>

engine::Vec4::Vec4(): x(1.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

engine::Vec4::Vec4(float v): x(v), y(v), z(v), w(v)
{
}

engine::Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{

}

float engine::Vec4::length() const
{
	return sqrt(x * x + y * y + z * z + w * w);
}

float engine::Vec4::dot(const Vec4& first, const Vec4& second)
{
	return first.x * second.x + first.y * second.y + first.z * second.z + first.w * second.w;
}

inline engine::Vec4 engine::Vec4::normalised(const Vec4& vec)
{
	float len = vec.length();
	assert(len != 0);
	float inv_length = 1 / len;

	return Vec4(vec.x * inv_length, vec.y * inv_length, vec.z * inv_length, vec.z * inv_length);
}

void engine::Vec4::normalise()
{
	float len = length();
	assert(len != 0);
	float inv_length = 1 / len;
	x *= inv_length;
	y *= inv_length;
	z *= inv_length;
	w *= inv_length;
}

engine::Vec4 engine::Vec4::operator+(const Vec4& other) const
{
	return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);

}

void engine::Vec4::operator+=(const Vec4& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
}

engine::Vec4 engine::Vec4::operator-(const Vec4& other) const
{
	return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
}

void engine::Vec4::operator-=(const Vec4& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
}

engine::Vec4 engine::Vec4::operator*(float scalar) const
{
	return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);

}

void engine::Vec4::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
}

engine::Vec4 engine::Vec4::operator/(float scalar) const
{
	assert(scalar != 0.0f);
	float inverse_scalar = 1.0f / scalar;
	return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
}

void engine::Vec4::operator/=(float scalar)
{
	assert(scalar != 0.0f);
	float inverse_scalar = 1.0f / scalar;
	x *= inverse_scalar;
	y *= inverse_scalar;
	z *= inverse_scalar;
	w *= inverse_scalar;
}

bool engine::Vec4::operator==(const Vec4& other) const
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool engine::Vec4::operator!=(const Vec4& other) const
{
	return !(*this == other);
}

engine::Vec4::operator std::string() {
	return fmt::format("({}, {}, {}, {})", x, y, z, w);
}

std::ostream& engine::operator<<(std::ostream& os, const engine::Vec4& vec)
{
	os << fmt::format("({}, {}, {}, {})", vec.x, vec.y, vec.z, vec.w);
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