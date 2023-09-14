/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include "Mat3.hpp"
#include <fmt/format.h>
#include <assert.h>

engine::Mat3::Mat3() : m_Data({ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f })
{

}

engine::Mat3::Mat3(float v): m_Data
({
	v, 0.0f, 0.0f,
	0.0f, v, 0.0f,
	0.0f, 0.0f, v,
})
{

}

engine::Mat3::Mat3(std::array<float, 9> data) : m_Data(data)
{
}

engine::Mat3::Mat3(float v_0, float v_1, float v_2, float v_3, float v_4, float v_5, float v_6, float v_7, float v_8) : m_Data({ v_0, v_1, v_2, v_3, v_4, v_5, v_6, v_7, v_8 })
{

}

float engine::Mat3::determinant() const
{
	return
		m_Data[0] * (m_Data[4] * m_Data[8] - m_Data[7] * m_Data[5])
		- m_Data[1] * (m_Data[3] * m_Data[8] - m_Data[6] * m_Data[5])
		+ m_Data[2] * (m_Data[3] * m_Data[7] - m_Data[6] * m_Data[4]);
}

void engine::Mat3::inverse()
{
	float det = determinant();
	assert(det != 0);
	float inv_determinant = 1.0f / det;
	// Transposition and inv determinant multiplied by cofactor all in operation
	m_Data = {
		inv_determinant * (m_Data[4] * m_Data[8] - m_Data[7] * m_Data[5]), -inv_determinant * (m_Data[1] * m_Data[8] - m_Data[7] * m_Data[2]), inv_determinant* (m_Data[1] * m_Data[5] - m_Data[4] * m_Data[2]),
		-inv_determinant * (m_Data[3] * m_Data[8] - m_Data[6] * m_Data[5]), inv_determinant* (m_Data[0] * m_Data[8] - m_Data[6] * m_Data[2]), -inv_determinant * (m_Data[0] * m_Data[5] - m_Data[3] * m_Data[2]),
		inv_determinant * (m_Data[3] * m_Data[7] - m_Data[6] * m_Data[4]), -inv_determinant * (m_Data[0] * m_Data[7] - m_Data[6] * m_Data[1]), inv_determinant* (m_Data[0] * m_Data[4] - m_Data[3] * m_Data[1])
	};
}

void engine::Mat3::transpose()
{
	std::swap(m_Data[3], m_Data[1]);
	std::swap(m_Data[6], m_Data[2]);
	std::swap(m_Data[7], m_Data[5]);
}

engine::Mat3 engine::Mat3::operator+(const Mat3& other) const
{
	return Mat3(
		m_Data[0] + other[0], m_Data[1] + other[1], m_Data[2] + other[2],
		m_Data[3] + other[3], m_Data[4] + other[4], m_Data[5] + other[5],
		m_Data[6] + other[6], m_Data[7] + other[7], m_Data[8] + other[8]
	);
}

void engine::Mat3::operator+=(const Mat3& other)
{
	m_Data[0] += other[0];
	m_Data[1] += other[1];
	m_Data[2] += other[2];
	m_Data[3] += other[3];
	m_Data[4] += other[4];
	m_Data[5] += other[5];
	m_Data[6] += other[6];
	m_Data[7] += other[7];
	m_Data[8] += other[8];
}

engine::Mat3 engine::Mat3::operator-(const Mat3& other) const
{
	return Mat3(
		m_Data[0] - other[0], m_Data[1] - other[1], m_Data[2] - other[2],
		m_Data[3] - other[3], m_Data[4] - other[4], m_Data[5] - other[5],
		m_Data[6] - other[6], m_Data[7] - other[7], m_Data[8] - other[8]
	);
}

void engine::Mat3::operator-=(const Mat3& other)
{
	m_Data[0] -= other[0];
	m_Data[1] -= other[1];
	m_Data[2] -= other[2];
	m_Data[3] -= other[3];
	m_Data[4] -= other[4];
	m_Data[5] -= other[5];
	m_Data[6] -= other[6];
	m_Data[7] -= other[7];
	m_Data[8] -= other[8];
}

engine::Mat3 engine::Mat3::operator*(const Mat3& other) const
{
	return Mat3(
		m_Data[0] * other[0] + m_Data[1] * other[3] + m_Data[2] * other[6], m_Data[0] * other[1] + m_Data[1] * other[4] + m_Data[2] * other[7], m_Data[0] * other[2] + m_Data[1] * other[5] + m_Data[2] * other[8],
		m_Data[3] * other[0] + m_Data[4] * other[3] + m_Data[5] * other[6], m_Data[3] * other[1] + m_Data[4] * other[4] + m_Data[5] * other[7], m_Data[3] * other[2] + m_Data[4] * other[5] + m_Data[5] * other[8],
		m_Data[6] * other[0] + m_Data[7] * other[3] + m_Data[8] * other[6], m_Data[6] * other[1] + m_Data[7] * other[4] + m_Data[8] * other[7], m_Data[6] * other[2] + m_Data[7] * other[5] + m_Data[8] * other[8]
	);
}

void engine::Mat3::operator*=(const Mat3& other)
{
	m_Data[0] = m_Data[0] * other[0] + m_Data[1] * other[3] + m_Data[2] * other[6];
	m_Data[1] = m_Data[0] * other[1] + m_Data[1] * other[4] + m_Data[2] * other[7];
	m_Data[2] = m_Data[0] * other[2] + m_Data[1] * other[5] + m_Data[2] * other[8];
	m_Data[3] = m_Data[3] * other[0] + m_Data[4] * other[3] + m_Data[5] * other[6];
	m_Data[4] = m_Data[3] * other[1] + m_Data[4] * other[4] + m_Data[5] * other[7];
	m_Data[5] = m_Data[3] * other[2] + m_Data[4] * other[5] + m_Data[5] * other[8];
	m_Data[6] = m_Data[6] * other[0] + m_Data[7] * other[3] + m_Data[8] * other[6];
	m_Data[7] = m_Data[6] * other[1] + m_Data[7] * other[4] + m_Data[8] * other[7];
	m_Data[8] = m_Data[6] * other[2] + m_Data[7] * other[5] + m_Data[8] * other[8];
}

engine::Mat3 engine::Mat3::operator*(float scalar) const
{
	return Mat3(
		m_Data[0] * scalar, m_Data[1] * scalar, m_Data[2] * scalar,
		m_Data[3] * scalar, m_Data[4] * scalar, m_Data[5] * scalar,
		m_Data[6] * scalar, m_Data[7] * scalar, m_Data[8] * scalar
	);
}

void engine::Mat3::operator*=(float scalar)
{
	m_Data[0] *= scalar;
	m_Data[1] *= scalar;
	m_Data[2] *= scalar;
	m_Data[3] *= scalar;
	m_Data[4] *= scalar;
	m_Data[5] *= scalar;
	m_Data[6] *= scalar;
	m_Data[7] *= scalar;
	m_Data[8] *= scalar;
}

engine::Mat3 engine::Mat3::operator/(float scalar) const
{
	float inv_scalar = 1.0f / scalar;
	return Mat3(
		m_Data[0] * inv_scalar, m_Data[1] * inv_scalar, m_Data[2] * inv_scalar,
		m_Data[3] * inv_scalar, m_Data[4] * inv_scalar, m_Data[5] * inv_scalar,
		m_Data[6] * inv_scalar, m_Data[7] * inv_scalar, m_Data[8] * inv_scalar
	);
}

void engine::Mat3::operator/=(float scalar)
{
	float inv_scalar = 1.0f / scalar;
	m_Data[0] *= inv_scalar;
	m_Data[1] *= inv_scalar;
	m_Data[2] *= inv_scalar;
	m_Data[3] *= inv_scalar;
	m_Data[4] *= inv_scalar;
	m_Data[5] *= inv_scalar;
	m_Data[6] *= inv_scalar;
	m_Data[7] *= inv_scalar;
	m_Data[8] *= inv_scalar;
}

engine::Mat3::operator std::string() {
	return fmt::format("|{} {} {}|\n|{} {} {}|\n|{} {} {}|", m_Data[0], m_Data[1], m_Data[2], m_Data[3], m_Data[4], m_Data[5], m_Data[6], m_Data[7], m_Data[8]);
}

float& engine::Mat3::operator[](size_t idx)
{
	return m_Data[idx];
}

float engine::Mat3::operator[](size_t idx) const
{
	return m_Data[idx];

}

std::ostream& engine::operator<<(std::ostream& os, const Mat3& mat)
{
	os << fmt::format("|{} {} {}|\n|{} {} {}|\n|{} {} {}|", mat[0], mat[1], mat[2], mat[3], mat[4], mat[5], mat[6], mat[7], mat[8]);
	return os;
}

engine::Mat3 engine::Mat3::inversed(const Mat3& mat3)
{
	float det = mat3.determinant();
	assert(det != 0);
	float inv_determinant = 1.0f / det;
	Mat3 transpose = transposed(mat3);
	
	return Mat3(
		inv_determinant * (transpose[4] * transpose[8] - transpose[7] * transpose[5]), -inv_determinant * (transpose[3] * transpose[8] - transpose[6] * transpose[5]), inv_determinant * (transpose[3] * transpose[7] - transpose[6] * transpose[4]),
		-inv_determinant * (transpose[1] * transpose[8] - transpose[7] * transpose[2]), inv_determinant * (transpose[0] * transpose[8] - transpose[6] * transpose[2]), -inv_determinant * (transpose[0] * transpose[7] - transpose[6] * transpose[1]),
		inv_determinant * (transpose[1] * transpose[5] - transpose[4] * transpose[2]), -inv_determinant * (transpose[0] * transpose[5] - transpose[3] * transpose[2]), inv_determinant * (transpose[0] * transpose[4] - transpose[3] * transpose[1])
	);
	
}

engine::Mat3 engine::Mat3::transposed(const Mat3& mat3)
{
	return Mat3(
		mat3[0], mat3[3], mat3[6],
		mat3[1], mat3[4], mat3[7],
		mat3[2], mat3[5], mat3[8]
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