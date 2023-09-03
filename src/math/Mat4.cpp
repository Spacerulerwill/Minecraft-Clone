/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/
#include "Mat4.hpp"
#include <fmt/format.h>
#include <assert.h>

engine::Mat4::Mat4() : m_Data({ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, })
{

}

engine::Mat4::Mat4(float v) : m_Data
({
	v, 0.0f, 0.0f, 0.0f,
	0.0f, v, 0.0f, 0.0f,
	0.0f, 0.0f, v, 0.0f,
	0.0f, 0.0f, 0.0f, v
})
{

}

engine::Mat4::Mat4(std::array<float, 16> data) : m_Data(data)
{

}

engine::Mat4::Mat4(float v_0, float v_1, float v_2, float v_3, float v_4, float v_5, float v_6, float v_7, float v_8, float v_9, float v_10, float v_11, float v_12, float v_13, float v_14, float v_15)
	: m_Data({ v_0, v_1, v_2, v_3, v_4, v_5, v_6, v_7, v_8, v_9, v_10, v_11, v_12, v_13, v_14, v_15 })
{

}

const float* engine::Mat4::GetPointer() const
{
	return &m_Data[0];
}

inline float engine::Mat4::determinant() const
{
	return
		m_Data[0] * (
			m_Data[5] * (m_Data[10] * m_Data[15] - m_Data[14] * m_Data[11])
			- m_Data[6] * (m_Data[9] * m_Data[15] - m_Data[13] * m_Data[11])
			+ m_Data[7] * (m_Data[9] * m_Data[14] - m_Data[13] * m_Data[10])
			) -
		m_Data[1] * (
			m_Data[4] * (m_Data[10] * m_Data[15] - m_Data[14] * m_Data[11])
			- m_Data[6] * (m_Data[8] * m_Data[15] - m_Data[12] * m_Data[11])
			+ m_Data[7] * (m_Data[8] * m_Data[14] - m_Data[12] * m_Data[10])
			) +
		m_Data[2] * (
			m_Data[4] * (m_Data[9] * m_Data[15] - m_Data[13] * m_Data[11])
			- m_Data[5] * (m_Data[8] * m_Data[15] - m_Data[12] * m_Data[11])
			+ m_Data[7] * (m_Data[8] * m_Data[13] - m_Data[12] * m_Data[9])
			) -
		m_Data[3] * (
			m_Data[4] * (m_Data[9] * m_Data[14] - m_Data[13] * m_Data[10])
			- m_Data[5] * (m_Data[8] * m_Data[14] - m_Data[12] * m_Data[10])
			+ m_Data[6] * (m_Data[8] * m_Data[13] - m_Data[12] * m_Data[9])
			);
}

void engine::Mat4::inverse()
{
	float det = determinant();
	assert(det != 0);
	float inv_determinant = 1.0f / det;

	float determinant_0 =
		m_Data[5] * (m_Data[10] * m_Data[15] - m_Data[14] * m_Data[11])
		- m_Data[6] * (m_Data[9] * m_Data[15] - m_Data[13] * m_Data[11])
		+ m_Data[7] * (m_Data[9] * m_Data[14] - m_Data[13] * m_Data[10]);

	float determinant_1 =
		m_Data[4] * (m_Data[10] * m_Data[15] - m_Data[14] * m_Data[11])
		- m_Data[6] * (m_Data[8] * m_Data[15] - m_Data[12] * m_Data[11])
		+ m_Data[7] * (m_Data[8] * m_Data[14] - m_Data[12] * m_Data[10]);

	float determinant_2 =
		m_Data[4] * (m_Data[9] * m_Data[15] - m_Data[13] * m_Data[11])
		- m_Data[5] * (m_Data[8] * m_Data[15] - m_Data[12] * m_Data[11])
		+ m_Data[7] * (m_Data[8] * m_Data[13] - m_Data[12] * m_Data[9]);

	float determinant_3 =
		m_Data[4] * (m_Data[9] * m_Data[14] - m_Data[13] * m_Data[10])
		- m_Data[5] * (m_Data[8] * m_Data[14] - m_Data[12] * m_Data[10])
		+ m_Data[6] * (m_Data[8] * m_Data[13] - m_Data[12] * m_Data[9]);

	float determinant_4 =
		m_Data[1] * (m_Data[10] * m_Data[15] - m_Data[14] * m_Data[11])
		- m_Data[2] * (m_Data[9] * m_Data[15] - m_Data[13] * m_Data[11])
		+ m_Data[3] * (m_Data[9] * m_Data[14] - m_Data[13] * m_Data[10]);

	float determinant_5 =
		m_Data[0] * (m_Data[10] * m_Data[15] - m_Data[14] * m_Data[11])
		- m_Data[2] * (m_Data[8] * m_Data[15] - m_Data[12] * m_Data[11])
		+ m_Data[3] * (m_Data[8] * m_Data[14] - m_Data[12] * m_Data[10]);

	float determinant_6 =
		m_Data[0] * (m_Data[9] * m_Data[15] - m_Data[13] * m_Data[11])
		- m_Data[1] * (m_Data[8] * m_Data[15] - m_Data[12] * m_Data[11])
		+ m_Data[3] * (m_Data[8] * m_Data[13] - m_Data[12] * m_Data[9]);

	float determinant_7 =
		m_Data[0] * (m_Data[9] * m_Data[14] - m_Data[13] * m_Data[10])
		- m_Data[1] * (m_Data[8] * m_Data[14] - m_Data[12] * m_Data[10])
		+ m_Data[2] * (m_Data[8] * m_Data[13] - m_Data[12] * m_Data[9]);

	float determinant_8 =
		m_Data[1] * (m_Data[6] * m_Data[15] - m_Data[14] * m_Data[7])
		- m_Data[2] * (m_Data[5] * m_Data[15] - m_Data[13] * m_Data[7])
		+ m_Data[3] * (m_Data[5] * m_Data[14] - m_Data[13] * m_Data[6]);

	float determinant_9 =
		m_Data[0] * (m_Data[6] * m_Data[15] - m_Data[14] * m_Data[7])
		- m_Data[2] * (m_Data[4] * m_Data[15] - m_Data[12] * m_Data[7])
		+ m_Data[3] * (m_Data[4] * m_Data[14] - m_Data[12] * m_Data[6]);

	float determinant_10 =
		m_Data[0] * (m_Data[5] * m_Data[15] - m_Data[13] * m_Data[7])
		- m_Data[1] * (m_Data[4] * m_Data[15] - m_Data[12] * m_Data[7])
		+ m_Data[3] * (m_Data[4] * m_Data[13] - m_Data[12] * m_Data[5]);

	float determinant_11 =
		m_Data[0] * (m_Data[5] * m_Data[14] - m_Data[13] * m_Data[6])
		- m_Data[1] * (m_Data[4] * m_Data[14] - m_Data[12] * m_Data[6])
		+ m_Data[2] * (m_Data[4] * m_Data[13] - m_Data[12] * m_Data[5]);

	float determinant_12 =
		m_Data[1] * (m_Data[6] * m_Data[11] - m_Data[10] * m_Data[7])
		- (m_Data[2] * (m_Data[5] * m_Data[11] - m_Data[9] * m_Data[7]))
		+ m_Data[3] * (m_Data[5] * m_Data[10] - m_Data[9] * m_Data[6]);

	float determinant_13 =
		m_Data[0] * (m_Data[6] * m_Data[11] - m_Data[10] * m_Data[7])
		- m_Data[2] * (m_Data[4] * m_Data[11] - m_Data[8] * m_Data[7])
		+ m_Data[3] * (m_Data[4] * m_Data[10] - m_Data[8] * m_Data[6]);

	float determinant_14 =
		m_Data[0] * (m_Data[5] * m_Data[11] - m_Data[9] * m_Data[7])
		- m_Data[1] * (m_Data[4] * m_Data[11] - m_Data[8] * m_Data[7])
		+ m_Data[3] * (m_Data[4] * m_Data[9] - m_Data[8] * m_Data[5]);

	float determinant_15 =
		m_Data[0] * (m_Data[5] * m_Data[10] - m_Data[9] * m_Data[6])
		- m_Data[1] * (m_Data[4] * m_Data[10] - m_Data[8] * m_Data[6])
		+ m_Data[2] * (m_Data[4] * m_Data[9] - m_Data[8] * m_Data[5]);

	m_Data = {
		inv_determinant * determinant_0, -inv_determinant * determinant_4, inv_determinant * determinant_8, -inv_determinant * determinant_12,
		-inv_determinant * determinant_1, inv_determinant * determinant_5, -inv_determinant * determinant_9, inv_determinant * determinant_13,
		inv_determinant * determinant_2, -inv_determinant * determinant_6, inv_determinant * determinant_10, -inv_determinant * determinant_14,
		-inv_determinant * determinant_3, inv_determinant * determinant_7, -inv_determinant * determinant_11, inv_determinant * determinant_15
	};
}

void engine::Mat4::transpose()
{
	std::swap(m_Data[1], m_Data[4]);
	std::swap(m_Data[8], m_Data[2]);
	std::swap(m_Data[12], m_Data[3]);
	std::swap(m_Data[9], m_Data[6]);
	std::swap(m_Data[13], m_Data[7]);
	std::swap(m_Data[14], m_Data[11]);
}

engine::Mat4 engine::Mat4::operator+(const Mat4& other) const
{
	return Mat4(
		m_Data[0] + other[0], m_Data[1] + other[1], m_Data[2] + other[2], m_Data[3] + other[3],
		m_Data[4] + other[4], m_Data[5] + other[5], m_Data[6] + other[6], m_Data[7] + other[7],
		m_Data[8] + other[8], m_Data[9] + other[9], m_Data[10] + other[10], m_Data[11] + other[11],
		m_Data[12] + other[12], m_Data[13] + other[13], m_Data[14] + other[14], m_Data[15] + other[15]
	);
}

void engine::Mat4::operator+=(const Mat4& other)
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
	m_Data[9] += other[9];
	m_Data[10] += other[10];
	m_Data[11] += other[11];
	m_Data[12] += other[12];
	m_Data[13] += other[13];
	m_Data[14] += other[14];
	m_Data[15] += other[15];
}

engine::Mat4 engine::Mat4::operator-(const Mat4& other) const
{
	return Mat4(
		m_Data[0] - other[0], m_Data[1] - other[1], m_Data[2] - other[2], m_Data[3] - other[3],
		m_Data[4] - other[4], m_Data[5] - other[5], m_Data[6] - other[6], m_Data[7] - other[7],
		m_Data[8] - other[8], m_Data[9] - other[9], m_Data[10] - other[10], m_Data[11] - other[11],
		m_Data[12] - other[12], m_Data[13] - other[13], m_Data[14] - other[14], m_Data[15] - other[15]
	);
}

void engine::Mat4::operator-=(const Mat4& other)
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
	m_Data[9] -= other[9];
	m_Data[10] -= other[10];
	m_Data[11] -= other[11];
	m_Data[12] -= other[12];
	m_Data[13] -= other[13];
	m_Data[14] -= other[14];
	m_Data[15] -= other[15];
}

engine::Mat4 engine::Mat4::operator*(const Mat4& other) const
{
	float v_0 = m_Data[0] * other[0] + m_Data[1] * other[4] + m_Data[2] * other[8] + m_Data[3] * other[12];
	float v_1 = m_Data[0] * other[1] + m_Data[1] * other[5] + m_Data[2] * other[9] + m_Data[3] * other[13];
	float v_2 = m_Data[0] * other[2] + m_Data[1] * other[6] + m_Data[2] * other[10] + m_Data[3] * other[14];
	float v_3 = m_Data[0] * other[3] + m_Data[1] * other[7] + m_Data[2] * other[11] + m_Data[3] * other[15];
	float v_4 = m_Data[4] * other[0] + m_Data[5] * other[4] + m_Data[6] * other[8] + m_Data[7] * other[12];
	float v_5 = m_Data[4] * other[1] + m_Data[5] * other[5] + m_Data[6] * other[9] + m_Data[7] * other[13];
	float v_6 = m_Data[4] * other[2] + m_Data[5] * other[6] + m_Data[6] * other[10] + m_Data[7] * other[14];
	float v_7 = m_Data[4] * other[3] + m_Data[5] * other[7] + m_Data[6] * other[11] + m_Data[7] * other[15];
	float v_8 = m_Data[8] * other[0] + m_Data[9] * other[4] + m_Data[10] * other[8] + m_Data[11] * other[12];
	float v_9 = m_Data[8] * other[1] + m_Data[9] * other[5] + m_Data[10] * other[9] + m_Data[11] * other[13];
	float v_10 = m_Data[8] * other[2] + m_Data[9] * other[6] + m_Data[10] * other[10] + m_Data[11] * other[14];
	float v_11 = m_Data[8] * other[3] + m_Data[9] * other[7] + m_Data[10] * other[11] + m_Data[11] * other[15];
	float v_12 = m_Data[12] * other[0] + m_Data[13] * other[4] + m_Data[14] * other[8] + m_Data[15] * other[12];
	float v_13 = m_Data[12] * other[1] + m_Data[13] * other[5] + m_Data[14] * other[9] + m_Data[15] * other[13];
	float v_14 = m_Data[12] * other[2] + m_Data[13] * other[6] + m_Data[14] * other[10] + m_Data[15] * other[14];
	float v_15 = m_Data[12] * other[3] + m_Data[13] * other[7] + m_Data[14] * other[11] + m_Data[15] * other[15];
	return Mat4(
		v_0, v_1, v_2, v_3,
		v_4, v_5, v_6, v_7,
		v_8, v_9, v_10, v_11,
		v_12, v_13, v_14, v_15
	);
}

void engine::Mat4::operator*=(const Mat4& other)
{
	m_Data[0] = m_Data[0] * other[0] + m_Data[1] * other[4] + m_Data[2] * other[8] + m_Data[3] * other[12];
	m_Data[1] = m_Data[0] * other[1] + m_Data[1] * other[5] + m_Data[2] * other[9] + m_Data[3] * other[13];
	m_Data[2] = m_Data[0] * other[2] + m_Data[1] * other[6] + m_Data[2] * other[10] + m_Data[3] * other[14];
	m_Data[3] = m_Data[0] * other[3] + m_Data[1] * other[7] + m_Data[2] * other[11] + m_Data[3] * other[15];
	m_Data[4] = m_Data[4] * other[0] + m_Data[5] * other[4] + m_Data[6] * other[8] + m_Data[7] * other[12];
	m_Data[5] = m_Data[4] * other[1] + m_Data[5] * other[5] + m_Data[6] * other[9] + m_Data[7] * other[13];
	m_Data[6] = m_Data[4] * other[2] + m_Data[5] * other[6] + m_Data[6] * other[10] + m_Data[7] * other[14];
	m_Data[7] = m_Data[4] * other[3] + m_Data[5] * other[7] + m_Data[6] * other[11] + m_Data[7] * other[15];
	m_Data[8] = m_Data[8] * other[0] + m_Data[9] * other[4] + m_Data[10] * other[8] + m_Data[11] * other[12];
	m_Data[9] = m_Data[8] * other[1] + m_Data[9] * other[5] + m_Data[10] * other[9] + m_Data[11] * other[13];
	m_Data[10] = m_Data[8] * other[2] + m_Data[9] * other[6] + m_Data[10] * other[10] + m_Data[11] * other[14];
	m_Data[11] = m_Data[8] * other[3] + m_Data[9] * other[7] + m_Data[10] * other[11] + m_Data[11] * other[15];
	m_Data[12] = m_Data[12] * other[0] + m_Data[13] * other[4] + m_Data[14] * other[8] + m_Data[15] * other[12];
	m_Data[13] = m_Data[12] * other[1] + m_Data[13] * other[5] + m_Data[14] * other[9] + m_Data[15] * other[13];
	m_Data[14] = m_Data[12] * other[2] + m_Data[13] * other[6] + m_Data[14] * other[10] + m_Data[15] * other[14];
	m_Data[15] = m_Data[12] * other[3] + m_Data[13] * other[7] + m_Data[14] * other[11] + m_Data[15] * other[15];
}

engine::Vec4 engine::Mat4::operator*(const Vec4& other)
{
	return Vec4(
		m_Data[0] * other.x + m_Data[1] * other.y * m_Data[2] * other.z + m_Data[3] * other.w,
		m_Data[4] * other.x + m_Data[5] * other.y * m_Data[6] * other.z + m_Data[7] * other.w,
		m_Data[8] * other.x + m_Data[9] * other.y * m_Data[10] * other.z + m_Data[11] * other.w,
		m_Data[12] * other.x + m_Data[13] * other.y * m_Data[14] * other.z + m_Data[15] * other.w

	);
}

engine::Mat4 engine::Mat4::operator*(float scalar) const
{
	return Mat4(
		m_Data[0] * scalar, m_Data[1] * scalar, m_Data[2] * scalar, m_Data[3] * scalar,
		m_Data[4] * scalar, m_Data[5] * scalar, m_Data[6] * scalar, m_Data[7] * scalar,
		m_Data[8] * scalar, m_Data[9] * scalar, m_Data[10] * scalar, m_Data[11] * scalar,
		m_Data[12] * scalar, m_Data[13] * scalar, m_Data[14] * scalar, m_Data[15] * scalar
	);
}

void engine::Mat4::operator*=(float scalar)
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
	m_Data[9] *= scalar;
	m_Data[10] *= scalar;
	m_Data[11] *= scalar;
	m_Data[12] *= scalar;
	m_Data[13] *= scalar;
	m_Data[14] *= scalar;
	m_Data[15] *= scalar;
}

engine::Mat4 engine::Mat4::operator/(float scalar) const
{
	float inv_scalar = 1.0f / scalar;
	return Mat4(
		m_Data[0] * inv_scalar, m_Data[1] * inv_scalar, m_Data[2] * inv_scalar, m_Data[3] * inv_scalar,
		m_Data[4] * inv_scalar, m_Data[5] * inv_scalar, m_Data[6] * inv_scalar, m_Data[7] * inv_scalar,
		m_Data[8] * inv_scalar, m_Data[9] * inv_scalar, m_Data[10] * inv_scalar, m_Data[11] * inv_scalar,
		m_Data[12] * inv_scalar, m_Data[13] * inv_scalar, m_Data[14] * inv_scalar, m_Data[15] * inv_scalar
	);
}

void engine::Mat4::operator/=(float scalar)
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
	m_Data[9] *= inv_scalar;
	m_Data[10] *= inv_scalar;
	m_Data[11] *= inv_scalar;
	m_Data[12] *= inv_scalar;
	m_Data[13] *= inv_scalar;
	m_Data[14] *= inv_scalar;
	m_Data[15] *= inv_scalar;
}

float& engine::Mat4::operator[](size_t idx)
{
	return m_Data[idx];
}

float engine::Mat4::operator[](size_t idx) const
{
	return m_Data[idx];
}

engine::Mat4 engine::Mat4::inversed(const Mat4& matrix)
{
	float det = matrix.determinant();
	assert(det != 0);
	float inv_determinant = 1.0f / det;

	float determinant_0 =
		matrix[5] * (matrix[10] * matrix[15] - matrix[14] * matrix[11])
		- matrix[6] * (matrix[9] * matrix[15] - matrix[13] * matrix[11])
		+ matrix[7] * (matrix[9] * matrix[14] - matrix[13] * matrix[10]);

	float determinant_1 =
		matrix[4] * (matrix[10] * matrix[15] - matrix[14] * matrix[11])
		- matrix[6] * (matrix[8] * matrix[15] - matrix[12] * matrix[11])
		+ matrix[7] * (matrix[8] * matrix[14] - matrix[12] * matrix[10]);

	float determinant_2 =
		matrix[4] * (matrix[9] * matrix[15] - matrix[13] * matrix[11])
		- matrix[5] * (matrix[8] * matrix[15] - matrix[12] * matrix[11])
		+ matrix[7] * (matrix[8] * matrix[13] - matrix[12] * matrix[9]);

	float determinant_3 =
		matrix[4] * (matrix[9] * matrix[14] - matrix[13] * matrix[10])
		- matrix[5] * (matrix[8] * matrix[14] - matrix[12] * matrix[10])
		+ matrix[6] * (matrix[8] * matrix[13] - matrix[12] * matrix[9]);

	float determinant_4 =
		matrix[1] * (matrix[10] * matrix[15] - matrix[14] * matrix[11])
		- matrix[2] * (matrix[9] * matrix[15] - matrix[13] * matrix[11])
		+ matrix[3] * (matrix[9] * matrix[14] - matrix[13] * matrix[10]);

	float determinant_5 =
		matrix[0] * (matrix[10] * matrix[15] - matrix[14] * matrix[11])
		- matrix[2] * (matrix[8] * matrix[15] - matrix[12] * matrix[11])
		+ matrix[3] * (matrix[8] * matrix[14] - matrix[12] * matrix[10]);

	float determinant_6 =
		matrix[0] * (matrix[9] * matrix[15] - matrix[13] * matrix[11])
		- matrix[1] * (matrix[8] * matrix[15] - matrix[12] * matrix[11])
		+ matrix[3] * (matrix[8] * matrix[13] - matrix[12] * matrix[9]);

	float determinant_7 =
		matrix[0] * (matrix[9] * matrix[14] - matrix[13] * matrix[10])
		- matrix[1] * (matrix[8] * matrix[14] - matrix[12] * matrix[10])
		+ matrix[2] * (matrix[8] * matrix[13] - matrix[12] * matrix[9]);

	float determinant_8 =
		matrix[1] * (matrix[6] * matrix[15] - matrix[14] * matrix[7])
		- matrix[2] * (matrix[5] * matrix[15] - matrix[13] * matrix[7])
		+ matrix[3] * (matrix[5] * matrix[14] - matrix[13] * matrix[6]);

	float determinant_9 =
		matrix[0] * (matrix[6] * matrix[15] - matrix[14] * matrix[7])
		- matrix[2] * (matrix[4] * matrix[15] - matrix[12] * matrix[7])
		+ matrix[3] * (matrix[4] * matrix[14] - matrix[12] * matrix[6]);

	float determinant_10 =
		matrix[0] * (matrix[5] * matrix[15] - matrix[13] * matrix[7])
		- matrix[1] * (matrix[4] * matrix[15] - matrix[12] * matrix[7])
		+ matrix[3] * (matrix[4] * matrix[13] - matrix[12] * matrix[5]);

	float determinant_11 =
		matrix[0] * (matrix[5] * matrix[14] - matrix[13] * matrix[6])
		- matrix[1] * (matrix[4] * matrix[14] - matrix[12] * matrix[6])
		+ matrix[2] * (matrix[4] * matrix[13] - matrix[12] * matrix[5]);

	float determinant_12 =
		matrix[1] * (matrix[6] * matrix[11] - matrix[10] * matrix[7])
		- (matrix[2] * (matrix[5] * matrix[11] - matrix[9] * matrix[7]))
		+ matrix[3] * (matrix[5] * matrix[10] - matrix[9] * matrix[6]);

	float determinant_13 =
		matrix[0] * (matrix[6] * matrix[11] - matrix[10] * matrix[7])
		- matrix[2] * (matrix[4] * matrix[11] - matrix[8] * matrix[7])
		+ matrix[3] * (matrix[4] * matrix[10] - matrix[8] * matrix[6]);

	float determinant_14 =
		matrix[0] * (matrix[5] * matrix[11] - matrix[9] * matrix[7])
		- matrix[1] * (matrix[4] * matrix[11] - matrix[8] * matrix[7])
		+ matrix[3] * (matrix[4] * matrix[9] - matrix[8] * matrix[5]);

	float determinant_15 =
		matrix[0] * (matrix[5] * matrix[10] - matrix[9] * matrix[6])
		- matrix[1] * (matrix[4] * matrix[10] - matrix[8] * matrix[6])
		+ matrix[2] * (matrix[4] * matrix[9] - matrix[8] * matrix[5]);

	return Mat4(
		inv_determinant * determinant_0, -inv_determinant * determinant_4, inv_determinant * determinant_8, -inv_determinant * determinant_12,
		-inv_determinant * determinant_1, inv_determinant * determinant_5, -inv_determinant * determinant_9, inv_determinant * determinant_13,
		inv_determinant * determinant_2, -inv_determinant * determinant_6, inv_determinant * determinant_10, -inv_determinant * determinant_14,
		-inv_determinant * determinant_3, inv_determinant * determinant_7, -inv_determinant * determinant_11, inv_determinant * determinant_15
	);
}

engine::Mat4 engine::Mat4::transposed(const Mat4& mat4)
{
	return Mat4(
		mat4[0], mat4[4], mat4[8], mat4[12],
		mat4[1], mat4[5], mat4[9], mat4[13],
		mat4[2], mat4[6], mat4[10], mat4[14],
		mat4[3], mat4[7], mat4[11], mat4[15]
	);
}

engine::Mat4::operator std::string() const {
	return fmt::format("|{} {} {} {}|\n|{} {} {} {}|\n|{} {} {} {}|\n|{} {} {} {}|",
		m_Data[0], m_Data[1], m_Data[2], m_Data[3],
		m_Data[4], m_Data[5], m_Data[6], m_Data[7],
		m_Data[8], m_Data[9], m_Data[10], m_Data[11],
		m_Data[12], m_Data[13], m_Data[14], m_Data[15]
	);
}

std::ostream& engine::operator<<(std::ostream& os, const Mat4& mat)
{
	os << fmt::format("|{} {} {} {}|\n|{} {} {} {}|\n|{} {} {} {}|\n|{} {} {} {}|",
		mat[0], mat[1], mat[2], mat[3],
		mat[4], mat[5], mat[6], mat[7],
		mat[8], mat[9], mat[10], mat[11],
		mat[12], mat[13], mat[14], mat[15]
	);
	return os;
}

engine::Mat4 engine::translationRemoved(const Mat4& mat)
{
	return Mat4(
		mat[0], mat[1], mat[2], 0.0f,
		mat[4], mat[5], mat[6], 0.0f,
		mat[8], mat[9], mat[10], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

engine::Mat4 engine::translate(const Vec3& vec)
{
	return Mat4(
		1.0f, 0.0f, 0.0f, vec.x,
		0.0f, 1.0f, 0.0f, vec.y,
		0.0f, 0.0f, 1.0f, vec.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

engine::Mat4 engine::scale(const Vec3& vec)
{
	return Mat4(
		vec.x, 0.0f, 0.0f, 0.0f,
		0.0f, vec.y, 0.0f, 0.0f,
		0.0f, 0.0f, vec.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

engine::Mat4 engine::rotate(const Vec3& axis, float theta)
{
	return Mat4(
		cos(theta) + axis.x * axis.x * (1.0f - cos(theta)), axis.x * axis.y * (1.0f - cos(theta)) - axis.z * sin(theta), axis.x * axis.z * (1.0f - cos(theta)) + axis.y * sin(theta), 0.0f,
		axis.y * axis.x * (1.0f - cos(theta)) + axis.z * sin(theta), cos(theta) + axis.y * axis.y * (1.0f - cos(theta)), axis.y * axis.z * (1.0f - cos(theta)) - axis.x * sin(theta), 0.0f,
		axis.z * axis.x * (1.0f - cos(theta)) - axis.y * sin(theta), axis.z * axis.y * (1.0f - cos(theta)) + axis.x * sin(theta), cos(theta) + axis.z * axis.z * (1.0f - cos(theta)), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

engine::Mat4 engine::perspective(float fov, float aspect, float near_dist, float far_dist)
{
	const float f = 1.0f / tan(fov / 2.0f);

	return Mat4(
		f / aspect, 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, (far_dist + near_dist) / (near_dist - far_dist), (2 * far_dist * near_dist) / (near_dist - far_dist),
		0.0f, 0.0f, -1.0f, 0.0f
	);
}

engine::Mat4 engine::orthographic(float bottom, float top, float left, float right, float _near, float _far)
{
	return Mat4(
		2.0f / (right - left), 0.0f, 0.0f, -((right + left) / (right - left)),
		0.0f, 2.0f / (top - bottom), 0.0f, -((top + bottom) / (top - bottom)),
		0.0f, 0.0f, -2.0f/(_far - _near), -((_far+_near)/(_far-_near)),
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

engine::Mat4 engine::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
{
	Vec3  f = engine::Vec3::normalised(center - eye);
	Vec3  u = engine::Vec3::normalised(up);
	Vec3  s = engine::Vec3::normalised(engine::Vec3::cross(f, u));
	u = engine::Vec3::cross(s, f);

	return Mat4(
		s.x, s.y, s.z, -engine::Vec3::dot(s, eye),
		u.x, u.y, u.z, -engine::Vec3::dot(u, eye),
		-f.x, -f.y, -f.z, engine::Vec3::dot(f, eye),
		0.0f, 0.0f, 0.0f, 1.0f
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