/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include "Vec2.hpp"
#include <assert.h>
#include <fmt/format.h>
#include <cmath>

engine::Vec2::Vec2() : x(1.0f), y(0.0f)
{
}

engine::Vec2::Vec2(float v) :x(v), y(v)
{

}

engine::Vec2::Vec2(float x, float y) : x(x), y(y)
{
}

float engine::Vec2::length() const
{
	return sqrt(x * x + y * y);
}

float engine::Vec2::dot(const Vec2& first, const Vec2& second)
{
	return first.x * second.x + first.y * second.y;
}

engine::Vec2 engine::Vec2::normalised(const Vec2& vec)
{
	float len = vec.length();
	assert(len != 0);
	float inv_length = 1 / vec.length();
	return Vec2(vec.x * inv_length, vec.y * inv_length);
}

void engine::Vec2::normalise()
{
	float len = length();
	assert(len != 0);
	float inv_length = 1 / len;
	x *= inv_length;
	y *= inv_length;
}

engine::Vec2 engine::Vec2::operator+(const Vec2& other) const
{
	return Vec2(x + other.x, y + other.y);
}

void engine::Vec2::operator+=(const Vec2& other)
{
	x += other.x;
	y += other.y;
}

engine::Vec2 engine::Vec2::operator-(const Vec2& other) const
{
	return Vec2(x - other.x, y - other.y);
}

void engine::Vec2::operator-=(const Vec2& other)
{
	x -= other.x;
	y -= other.y;
}

engine::Vec2 engine::Vec2::operator*(float scalar) const
{
	return Vec2(x * scalar, y * scalar);
}

void engine::Vec2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
}

engine::Vec2 engine::Vec2::operator/(float scalar) const
{
	assert(scalar != 0.0f);
	float inverse_scalar = 1.0f / scalar;
	return Vec2(x * scalar, y * scalar);
}

void engine::Vec2::operator/=(float scalar)
{
	assert(scalar != 0.0f);
	float inverse_scalar = 1.0f / scalar;
	x *= inverse_scalar;
	y *= inverse_scalar;
}

bool engine::Vec2::operator==(const Vec2& other) const
{
	return x == other.x && y == other.y;
}

bool engine::Vec2::operator!=(const Vec2& other) const
{
	return !(*this == other);
}

engine::Vec2::operator std::string() {
	return fmt::format("({}, {})", x, y);
}

std::ostream& engine::operator<<(std::ostream& os, const Vec2& vec)
{
	os << fmt::format("({}, {})", vec.x, vec.y);
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