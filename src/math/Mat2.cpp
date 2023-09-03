/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include "Mat2.hpp"
#include <fmt/format.h>
#include <assert.h>

engine::Mat2::Mat2() : m_Data({ 0.0f, 0.0f, 0.0f, 0.0f })
{

}

engine::Mat2::Mat2(float v): m_Data
({
	v, 0.0f,
	0.0f, v
})
{

}

engine::Mat2::Mat2(std::array<float, 4> data) : m_Data(data)
{

}

engine::Mat2::Mat2(float v_0, float v_1, float v_2, float v_3) :m_Data({ v_0, v_1, v_2, v_3 })
{

}

float engine::Mat2::determinant() const
{
	return m_Data[0] * m_Data[3] - m_Data[1] * m_Data[2];
}

void engine::Mat2::inverse()
{
	float det = determinant();
	assert(det != 0);
	float inv_determinant = 1.0f / det;
	m_Data = {
		m_Data[3] * inv_determinant, -m_Data[1] * inv_determinant,
		-m_Data[2] * inv_determinant, m_Data[0] * inv_determinant
	};
}

void engine::Mat2::tranpose()
{
	std::swap(m_Data[1], m_Data[2]);
}

engine::Mat2 engine::Mat2::operator+(const Mat2& other) const
{
	return Mat2(
		m_Data[0] + other.m_Data[0], m_Data[1] + other.m_Data[1],
		m_Data[2] + other.m_Data[2], m_Data[3] + other.m_Data[3]
	);
}

void engine::Mat2::operator+=(const Mat2& other)
{
	m_Data[0] += other.m_Data[0];
	m_Data[1] += other.m_Data[1];
	m_Data[2] += other.m_Data[2];
	m_Data[3] += other.m_Data[3];
}

engine::Mat2 engine::Mat2::operator-(const Mat2& other) const
{
	return Mat2(
		m_Data[0] - other.m_Data[0], m_Data[1] - other.m_Data[1],
		m_Data[2] - other.m_Data[2], m_Data[3] - other.m_Data[3]
	);
}

void engine::Mat2::operator-=(const Mat2& other)
{
	m_Data[0] -= other.m_Data[0];
	m_Data[1] -= other.m_Data[1];
	m_Data[2] -= other.m_Data[2];
	m_Data[3] -= other.m_Data[3];
}

engine::Mat2 engine::Mat2::operator*(const Mat2& other) const
{
	return Mat2(
		m_Data[0] * other.m_Data[0] + m_Data[1] * other.m_Data[2], m_Data[0] * other.m_Data[1] + m_Data[1] * other.m_Data[3],
		m_Data[2] * other.m_Data[0] + m_Data[3] * other.m_Data[2], m_Data[2] * other.m_Data[1] + m_Data[3] * other.m_Data[3]
	);
}

void engine::Mat2::operator*=(const Mat2& other)
{
	m_Data[0] = m_Data[0] * other.m_Data[0] + m_Data[1] * other.m_Data[2];
	m_Data[1] = m_Data[0] * other.m_Data[1] + m_Data[1] * other.m_Data[3];
	m_Data[2] = m_Data[2] * other.m_Data[0] + m_Data[3] * other.m_Data[2];
	m_Data[3] = m_Data[2] * other.m_Data[1] + m_Data[3] * other.m_Data[3];
}

engine::Mat2 engine::Mat2::operator*(float scalar) const
{
	return Mat2(
		m_Data[0] * scalar, m_Data[1] * scalar,
		m_Data[2] * scalar, m_Data[3] * scalar
	);
}

void engine::Mat2::operator*=(float scalar)
{
	m_Data[0] *= scalar;
	m_Data[1] *= scalar;
	m_Data[2] *= scalar;
	m_Data[3] *= scalar;
}

engine::Mat2 engine::Mat2::operator/(float scalar) const
{
	float inv_scalar = 1.0f / scalar;
	return Mat2(
		m_Data[0] * inv_scalar, m_Data[1] * inv_scalar,
		m_Data[2] * inv_scalar, m_Data[3] * inv_scalar
	);
}

void engine::Mat2::operator/=(float scalar)
{
	float inv_scalar = 1.0f / scalar;
	m_Data[0] *= inv_scalar;
	m_Data[1] *= inv_scalar;
	m_Data[2] *= inv_scalar;
	m_Data[3] *= inv_scalar;
}

float& engine::Mat2::operator[](size_t idx)
{
	return m_Data[idx];
}

float engine::Mat2::operator[](size_t idx) const
{
	return m_Data[idx];
}

engine::Mat2::operator std::string() {
	return fmt::format("|{} {}|\n|{} {}|", m_Data[0], m_Data[1], m_Data[2], m_Data[3]);
}

std::ostream& engine::operator<<(std::ostream& os, const Mat2& mat)
{
	os << fmt::format("|{} {}|\n|{} {}|", mat[0], mat[1], mat[2], mat[3]);
	return os;
}

engine::Mat2 engine::Mat2::inversed(const Mat2& matrix)
{
	float det = matrix.determinant();
	assert(det != 0);
	float inv_determinant = 1.0f / det;

	return Mat2(
		matrix[3] * inv_determinant, -matrix[1] * inv_determinant,
		-matrix[2] * inv_determinant, matrix[0] * inv_determinant
	);
}

engine::Mat2 engine::Mat2::transposed(const Mat2& mat2)
{
	return Mat2(
		mat2[0], mat2[2],
		mat2[1], mat2[3]
	);
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
