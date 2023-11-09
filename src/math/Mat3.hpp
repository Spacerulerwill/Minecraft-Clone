/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef MAT3_H
#define MAT3_H

#include <array>
#include <ostream>
#include <util/Concepts.hpp>

namespace engine {
    template<std::floating_point T>
	class Mat3 {
	private:
		std::array<T, 9> m_Data;
	public:
		Mat3() : m_Data({
            T(0), T(0), T(0), 
            T(0), T(0), T(0),
            T(0), T(0), T(0)
        }) {}

		Mat3(T v) : m_Data({
            v, T(0), T(0),
            T(0), v, T(0),
            T(0), T(0), v
        }) {}

		Mat3(const std::array<T, 9>& data) : m_Data(data) {}

		Mat3(T v_0, T v_1, T v_2, T v_3, T v_4, T v_5, T v_6, T v_7, T v_8) : m_Data({
            v_0, v_1, v_2,
            v_3, v_4, v_5,
            v_6, v_7, v_8
        }) {}

        int determinant() const {
            return
                m_Data[0] * (m_Data[4] * m_Data[8] - m_Data[7] * m_Data[5])
                - m_Data[1] * (m_Data[3] * m_Data[8] - m_Data[6] * m_Data[5])
                + m_Data[2] * (m_Data[3] * m_Data[7] - m_Data[6] * m_Data[4]);
        }

		void inverse() {
            int det = determinant();
            assert(det != 0);
            T inv_determinant = T(1) / det;
            m_Data = {
                inv_determinant * (m_Data[4] * m_Data[8] - m_Data[7] * m_Data[5]), -inv_determinant * (m_Data[1] * m_Data[8] - m_Data[7] * m_Data[2]), inv_determinant* (m_Data[1] * m_Data[5] - m_Data[4] * m_Data[2]),
                -inv_determinant * (m_Data[3] * m_Data[8] - m_Data[6] * m_Data[5]), inv_determinant* (m_Data[0] * m_Data[8] - m_Data[6] * m_Data[2]), -inv_determinant * (m_Data[0] * m_Data[5] - m_Data[3] * m_Data[2]),
                inv_determinant * (m_Data[3] * m_Data[7] - m_Data[6] * m_Data[4]), -inv_determinant * (m_Data[0] * m_Data[7] - m_Data[6] * m_Data[1]), inv_determinant* (m_Data[0] * m_Data[4] - m_Data[3] * m_Data[1])
            };
        }

		void transpose() {
            std::swap(m_Data[3], m_Data[1]);
            std::swap(m_Data[6], m_Data[2]);
            std::swap(m_Data[7], m_Data[5]);
        }

        static Mat3 inversed(const Mat3& mat3) {
            int det = mat3.determinant();
            assert(det != 0);
            T inv_determinant = T(1) / det;
            Mat3 transpose = transposed(mat3);
            
            return Mat3(
                inv_determinant * (transpose[4] * transpose[8] - transpose[7] * transpose[5]), -inv_determinant * (transpose[3] * transpose[8] - transpose[6] * transpose[5]), inv_determinant * (transpose[3] * transpose[7] - transpose[6] * transpose[4]),
                -inv_determinant * (transpose[1] * transpose[8] - transpose[7] * transpose[2]), inv_determinant * (transpose[0] * transpose[8] - transpose[6] * transpose[2]), -inv_determinant * (transpose[0] * transpose[7] - transpose[6] * transpose[1]),
                inv_determinant * (transpose[1] * transpose[5] - transpose[4] * transpose[2]), -inv_determinant * (transpose[0] * transpose[5] - transpose[3] * transpose[2]), inv_determinant * (transpose[0] * transpose[4] - transpose[3] * transpose[1])
            );
        }

		static Mat3 transposed(const Mat3& mat3) {
            return Mat3(
                mat3[0], mat3[3], mat3[6],
                mat3[1], mat3[4], mat3[7],
                mat3[2], mat3[5], mat3[8]
            );
        }

		Mat3 operator+(const Mat3& other) const {
            return Mat3(
                m_Data[0] + other[0], m_Data[1] + other[1], m_Data[2] + other[2],
                m_Data[3] + other[3], m_Data[4] + other[4], m_Data[5] + other[5],
                m_Data[6] + other[6], m_Data[7] + other[7], m_Data[8] + other[8]
            );
        }

		void operator+=(const Mat3& other) {
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

		Mat3 operator-(const Mat3& other) const {
            return Mat3(
                m_Data[0] - other[0], m_Data[1] - other[1], m_Data[2] - other[2],
                m_Data[3] - other[3], m_Data[4] - other[4], m_Data[5] - other[5],
                m_Data[6] - other[6], m_Data[7] - other[7], m_Data[8] - other[8]
            );
        }

		void operator-=(const Mat3& other) {
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

		Mat3 operator*(const Mat3& other) const {
            return Mat3(
                m_Data[0] * other[0] + m_Data[1] * other[3] + m_Data[2] * other[6], m_Data[0] * other[1] + m_Data[1] * other[4] + m_Data[2] * other[7], m_Data[0] * other[2] + m_Data[1] * other[5] + m_Data[2] * other[8],
                m_Data[3] * other[0] + m_Data[4] * other[3] + m_Data[5] * other[6], m_Data[3] * other[1] + m_Data[4] * other[4] + m_Data[5] * other[7], m_Data[3] * other[2] + m_Data[4] * other[5] + m_Data[5] * other[8],
                m_Data[6] * other[0] + m_Data[7] * other[3] + m_Data[8] * other[6], m_Data[6] * other[1] + m_Data[7] * other[4] + m_Data[8] * other[7], m_Data[6] * other[2] + m_Data[7] * other[5] + m_Data[8] * other[8]
            );
        }

		void operator*=(const Mat3& other) {
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

		Mat3 operator*(T scalar) const {
            return Mat3(
                m_Data[0] * scalar, m_Data[1] * scalar, m_Data[2] * scalar,
                m_Data[3] * scalar, m_Data[4] * scalar, m_Data[5] * scalar,
                m_Data[6] * scalar, m_Data[7] * scalar, m_Data[8] * scalar
            );
        }

		void operator*=(T scalar) {
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

		Mat3 operator/(T scalar) const {
            T inv_scalar = T(1) / scalar;
            return Mat3(
                m_Data[0] * inv_scalar, m_Data[1] * inv_scalar, m_Data[2] * inv_scalar,
                m_Data[3] * inv_scalar, m_Data[4] * inv_scalar, m_Data[5] * inv_scalar,
                m_Data[6] * inv_scalar, m_Data[7] * inv_scalar, m_Data[8] * inv_scalar
            );
        }

		void operator/=(T scalar) {
            T inv_scalar = T(1) / scalar;
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

        const T* GetPointer() const {
            return &m_Data[0];
        }

		T& operator[](size_t idx) {
            return m_Data[idx];
        }   

		T operator[](size_t idx) const {
            return m_Data[idx];
        }

		operator std::string() {
	        return fmt::format("|{} {} {}|\n|{} {} {}|\n|{} {} {}|", m_Data[0], m_Data[1], m_Data[2], m_Data[3], m_Data[4], m_Data[5], m_Data[6], m_Data[7], m_Data[8]);
        }

		friend std::ostream& operator<<(std::ostream& os, const Mat3& mat) {
            os << fmt::format("|{} {} {}|\n|{} {} {}|\n|{} {} {}|", mat[0], mat[1], mat[2], mat[3], mat[4], mat[5], mat[6], mat[7], mat[8]);
            return os;
        }
	};
}
#endif // !MAT3_H

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