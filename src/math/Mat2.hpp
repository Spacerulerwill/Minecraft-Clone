/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef MAT2_H
#define MAT2_H

#include <array>
#include <ostream>
#include <math/Math.hpp>
#include <util/Concepts.hpp>

namespace engine {
    template<std::floating_point T>
    class Mat2 {
    private:
        std::array<T, 4> m_Data;
    public:
        Mat2() : m_Data({
                  T(0), T(0),
                  T(0), T(0)
            }) {}

        Mat2(T v) : m_Data({
                  v, T(0),
                  T(0), v
            }) {}

        Mat2(const std::array<T, 4>& data) : m_Data(data) {}

        Mat2(T v_0, T v_1, T v_2, T v_3) : m_Data({
                  v_0, v_1,
                  v_2, v_3
            }) {}

        int determinant() const {
            return m_Data[0] * m_Data[3] - m_Data[1] * m_Data[2];
        }

        void inverse() {
            int det = determinant();
            assert(det != 0);
            float inv_determinant = T(1) / det;
            m_Data = {
                m_Data[3] * inv_determinant, -m_Data[1] * inv_determinant,
                -m_Data[2] * inv_determinant, m_Data[0] * inv_determinant
            };
        }

        void tranpose() {
            std::swap(m_Data[1], m_Data[2]);
        }

        static Mat2 inversed(const Mat2& matrix) {
            int det = matrix.determinant();
            assert(det != 0);
            float inv_determinant = T(1) / det;

            return Mat2(
                matrix[3] * inv_determinant, -matrix[1] * inv_determinant,
                -matrix[2] * inv_determinant, matrix[0] * inv_determinant
            );
        }

        static Mat2 transposed(const Mat2& mat2) {
            return Mat2(
                mat2[0], mat2[2],
                mat2[1], mat2[3]
            );
        }

        Mat2 operator+(const Mat2& other) const {
            return Mat2(
                m_Data[0] + other.m_Data[0], m_Data[1] + other.m_Data[1],
                m_Data[2] + other.m_Data[2], m_Data[3] + other.m_Data[3]
            );
        }

        void operator+=(const Mat2& other) {
            m_Data[0] += other.m_Data[0];
            m_Data[1] += other.m_Data[1];
            m_Data[2] += other.m_Data[2];
            m_Data[3] += other.m_Data[3];
        }

        Mat2 operator-(const Mat2& other) const {
            return Mat2(
                m_Data[0] - other.m_Data[0], m_Data[1] - other.m_Data[1],
                m_Data[2] - other.m_Data[2], m_Data[3] - other.m_Data[3]
            );
        }

        void operator-=(const Mat2& other) {
            m_Data[0] -= other.m_Data[0];
            m_Data[1] -= other.m_Data[1];
            m_Data[2] -= other.m_Data[2];
            m_Data[3] -= other.m_Data[3];
        }

        Mat2 operator*(const Mat2& other) const {
            return Mat2(
                m_Data[0] * other.m_Data[0] + m_Data[1] * other.m_Data[2], m_Data[0] * other.m_Data[1] + m_Data[1] * other.m_Data[3],
                m_Data[2] * other.m_Data[0] + m_Data[3] * other.m_Data[2], m_Data[2] * other.m_Data[1] + m_Data[3] * other.m_Data[3]
            );
        }

        void operator*=(const Mat2& other) {
            m_Data[0] = m_Data[0] * other.m_Data[0] + m_Data[1] * other.m_Data[2];
            m_Data[1] = m_Data[0] * other.m_Data[1] + m_Data[1] * other.m_Data[3];
            m_Data[2] = m_Data[2] * other.m_Data[0] + m_Data[3] * other.m_Data[2];
            m_Data[3] = m_Data[2] * other.m_Data[1] + m_Data[3] * other.m_Data[3];
        }

        Mat2 operator*(T scalar) const {
            return Mat2(
                m_Data[0] * scalar, m_Data[1] * scalar,
                m_Data[2] * scalar, m_Data[3] * scalar
            );
        }

        void operator*=(T scalar) {
            m_Data[0] *= scalar;
            m_Data[1] *= scalar;
            m_Data[2] *= scalar;
            m_Data[3] *= scalar;
        }

        Mat2 operator/(T scalar) const {
            T inv_scalar = T(1) / scalar;
            return Mat2(
                m_Data[0] * inv_scalar, m_Data[1] * inv_scalar,
                m_Data[2] * inv_scalar, m_Data[3] * inv_scalar
            );
        }

        void operator/=(T scalar) {
            T inv_scalar = T(1) / scalar;
            m_Data[0] *= inv_scalar;
            m_Data[1] *= inv_scalar;
            m_Data[2] *= inv_scalar;
            m_Data[3] *= inv_scalar;
        }

        const T* GetPointer() const {
            return &m_Data[0];
        }

        T& operator [](size_t idx) {
            return m_Data[idx];
        }

        T operator [](size_t idx) const {
            return m_Data[idx];
        }

        operator std::string() {
            return fmt::format("|{} {}|\n|{} {}|", m_Data[0], m_Data[1], m_Data[2], m_Data[3]);
        }

        friend std::ostream& operator<<(std::ostream& os, const Mat2& mat) {
            os << fmt::format("|{} {}|\n|{} {}|", mat[0], mat[1], mat[2], mat[3]);
            return os;
        }
    };
}
#endif // !MAT2_H

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