/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef MAT4_H
#define MAT4_H

#include <array>
#include <ostream>
#include <math/Vec4.hpp>
#include <math/Vec3.hpp>

namespace engine {
    template<std::floating_point T>
    class Mat4 {
    private:
        std::array<T, 16> m_Data;
    public:
        Mat4() : m_Data({
                  T(0), T(0), T(0), T(0),
                  T(0), T(0), T(0), T(0),
                  T(0), T(0), T(0), T(0),
                  T(0), T(0), T(0), T(0)
            }) {}

        Mat4(T v) : m_Data({
                  v, T(0), T(0), T(0),
                  T(0), v, T(0), T(0),
                  T(0), T(0), v, T(0),
                  T(0), T(0), T(0), v
            }) {}

        Mat4(const std::array<T, 16>& data) : m_Data(data) {}
        ;
        Mat4(T v_0, T v_1, T v_2, T v_3, T v_4, T v_5, T v_6, T v_7, T v_8, T v_9, T v_10, T v_11, T v_12, T v_13, T v_14, T v_15) : m_Data({
                  v_0, v_1, v_2, v_3,
                  v_4, v_5, v_6, v_7,
                  v_8, v_9, v_10, v_11,
                  v_12, v_13, v_14, v_15
            }) {};

        // Return a pointer to the first element in the matrix
        int determinant() const {
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

        void inverse() {
            int det = determinant();
            assert(det != 0);
            T inv_determinant = T(1) / det;

            int determinant_0 =
                m_Data[5] * (m_Data[10] * m_Data[15] - m_Data[14] * m_Data[11])
                - m_Data[6] * (m_Data[9] * m_Data[15] - m_Data[13] * m_Data[11])
                + m_Data[7] * (m_Data[9] * m_Data[14] - m_Data[13] * m_Data[10]);

            int determinant_1 =
                m_Data[4] * (m_Data[10] * m_Data[15] - m_Data[14] * m_Data[11])
                - m_Data[6] * (m_Data[8] * m_Data[15] - m_Data[12] * m_Data[11])
                + m_Data[7] * (m_Data[8] * m_Data[14] - m_Data[12] * m_Data[10]);

            int determinant_2 =
                m_Data[4] * (m_Data[9] * m_Data[15] - m_Data[13] * m_Data[11])
                - m_Data[5] * (m_Data[8] * m_Data[15] - m_Data[12] * m_Data[11])
                + m_Data[7] * (m_Data[8] * m_Data[13] - m_Data[12] * m_Data[9]);

            int determinant_3 =
                m_Data[4] * (m_Data[9] * m_Data[14] - m_Data[13] * m_Data[10])
                - m_Data[5] * (m_Data[8] * m_Data[14] - m_Data[12] * m_Data[10])
                + m_Data[6] * (m_Data[8] * m_Data[13] - m_Data[12] * m_Data[9]);

            int determinant_4 =
                m_Data[1] * (m_Data[10] * m_Data[15] - m_Data[14] * m_Data[11])
                - m_Data[2] * (m_Data[9] * m_Data[15] - m_Data[13] * m_Data[11])
                + m_Data[3] * (m_Data[9] * m_Data[14] - m_Data[13] * m_Data[10]);

            int determinant_5 =
                m_Data[0] * (m_Data[10] * m_Data[15] - m_Data[14] * m_Data[11])
                - m_Data[2] * (m_Data[8] * m_Data[15] - m_Data[12] * m_Data[11])
                + m_Data[3] * (m_Data[8] * m_Data[14] - m_Data[12] * m_Data[10]);

            int determinant_6 =
                m_Data[0] * (m_Data[9] * m_Data[15] - m_Data[13] * m_Data[11])
                - m_Data[1] * (m_Data[8] * m_Data[15] - m_Data[12] * m_Data[11])
                + m_Data[3] * (m_Data[8] * m_Data[13] - m_Data[12] * m_Data[9]);

            int determinant_7 =
                m_Data[0] * (m_Data[9] * m_Data[14] - m_Data[13] * m_Data[10])
                - m_Data[1] * (m_Data[8] * m_Data[14] - m_Data[12] * m_Data[10])
                + m_Data[2] * (m_Data[8] * m_Data[13] - m_Data[12] * m_Data[9]);

            int determinant_8 =
                m_Data[1] * (m_Data[6] * m_Data[15] - m_Data[14] * m_Data[7])
                - m_Data[2] * (m_Data[5] * m_Data[15] - m_Data[13] * m_Data[7])
                + m_Data[3] * (m_Data[5] * m_Data[14] - m_Data[13] * m_Data[6]);

            int determinant_9 =
                m_Data[0] * (m_Data[6] * m_Data[15] - m_Data[14] * m_Data[7])
                - m_Data[2] * (m_Data[4] * m_Data[15] - m_Data[12] * m_Data[7])
                + m_Data[3] * (m_Data[4] * m_Data[14] - m_Data[12] * m_Data[6]);

            int determinant_10 =
                m_Data[0] * (m_Data[5] * m_Data[15] - m_Data[13] * m_Data[7])
                - m_Data[1] * (m_Data[4] * m_Data[15] - m_Data[12] * m_Data[7])
                + m_Data[3] * (m_Data[4] * m_Data[13] - m_Data[12] * m_Data[5]);

            int determinant_11 =
                m_Data[0] * (m_Data[5] * m_Data[14] - m_Data[13] * m_Data[6])
                - m_Data[1] * (m_Data[4] * m_Data[14] - m_Data[12] * m_Data[6])
                + m_Data[2] * (m_Data[4] * m_Data[13] - m_Data[12] * m_Data[5]);

            int determinant_12 =
                m_Data[1] * (m_Data[6] * m_Data[11] - m_Data[10] * m_Data[7])
                - (m_Data[2] * (m_Data[5] * m_Data[11] - m_Data[9] * m_Data[7]))
                + m_Data[3] * (m_Data[5] * m_Data[10] - m_Data[9] * m_Data[6]);

            int determinant_13 =
                m_Data[0] * (m_Data[6] * m_Data[11] - m_Data[10] * m_Data[7])
                - m_Data[2] * (m_Data[4] * m_Data[11] - m_Data[8] * m_Data[7])
                + m_Data[3] * (m_Data[4] * m_Data[10] - m_Data[8] * m_Data[6]);

            int determinant_14 =
                m_Data[0] * (m_Data[5] * m_Data[11] - m_Data[9] * m_Data[7])
                - m_Data[1] * (m_Data[4] * m_Data[11] - m_Data[8] * m_Data[7])
                + m_Data[3] * (m_Data[4] * m_Data[9] - m_Data[8] * m_Data[5]);

            int determinant_15 =
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

        void transpose() {
            std::swap(m_Data[1], m_Data[4]);
            std::swap(m_Data[8], m_Data[2]);
            std::swap(m_Data[12], m_Data[3]);
            std::swap(m_Data[9], m_Data[6]);
            std::swap(m_Data[13], m_Data[7]);
            std::swap(m_Data[14], m_Data[11]);
        }

        static Mat4 inversed(const Mat4& matrix) {
            int det = matrix.determinant();
            assert(det != 0);
            T inv_determinant = T(1) / det;

            int determinant_0 =
                matrix[5] * (matrix[10] * matrix[15] - matrix[14] * matrix[11])
                - matrix[6] * (matrix[9] * matrix[15] - matrix[13] * matrix[11])
                + matrix[7] * (matrix[9] * matrix[14] - matrix[13] * matrix[10]);

            int determinant_1 =
                matrix[4] * (matrix[10] * matrix[15] - matrix[14] * matrix[11])
                - matrix[6] * (matrix[8] * matrix[15] - matrix[12] * matrix[11])
                + matrix[7] * (matrix[8] * matrix[14] - matrix[12] * matrix[10]);

            int determinant_2 =
                matrix[4] * (matrix[9] * matrix[15] - matrix[13] * matrix[11])
                - matrix[5] * (matrix[8] * matrix[15] - matrix[12] * matrix[11])
                + matrix[7] * (matrix[8] * matrix[13] - matrix[12] * matrix[9]);

            int determinant_3 =
                matrix[4] * (matrix[9] * matrix[14] - matrix[13] * matrix[10])
                - matrix[5] * (matrix[8] * matrix[14] - matrix[12] * matrix[10])
                + matrix[6] * (matrix[8] * matrix[13] - matrix[12] * matrix[9]);

            int determinant_4 =
                matrix[1] * (matrix[10] * matrix[15] - matrix[14] * matrix[11])
                - matrix[2] * (matrix[9] * matrix[15] - matrix[13] * matrix[11])
                + matrix[3] * (matrix[9] * matrix[14] - matrix[13] * matrix[10]);

            int determinant_5 =
                matrix[0] * (matrix[10] * matrix[15] - matrix[14] * matrix[11])
                - matrix[2] * (matrix[8] * matrix[15] - matrix[12] * matrix[11])
                + matrix[3] * (matrix[8] * matrix[14] - matrix[12] * matrix[10]);

            int determinant_6 =
                matrix[0] * (matrix[9] * matrix[15] - matrix[13] * matrix[11])
                - matrix[1] * (matrix[8] * matrix[15] - matrix[12] * matrix[11])
                + matrix[3] * (matrix[8] * matrix[13] - matrix[12] * matrix[9]);

            int determinant_7 =
                matrix[0] * (matrix[9] * matrix[14] - matrix[13] * matrix[10])
                - matrix[1] * (matrix[8] * matrix[14] - matrix[12] * matrix[10])
                + matrix[2] * (matrix[8] * matrix[13] - matrix[12] * matrix[9]);

            int determinant_8 =
                matrix[1] * (matrix[6] * matrix[15] - matrix[14] * matrix[7])
                - matrix[2] * (matrix[5] * matrix[15] - matrix[13] * matrix[7])
                + matrix[3] * (matrix[5] * matrix[14] - matrix[13] * matrix[6]);

            int determinant_9 =
                matrix[0] * (matrix[6] * matrix[15] - matrix[14] * matrix[7])
                - matrix[2] * (matrix[4] * matrix[15] - matrix[12] * matrix[7])
                + matrix[3] * (matrix[4] * matrix[14] - matrix[12] * matrix[6]);

            int determinant_10 =
                matrix[0] * (matrix[5] * matrix[15] - matrix[13] * matrix[7])
                - matrix[1] * (matrix[4] * matrix[15] - matrix[12] * matrix[7])
                + matrix[3] * (matrix[4] * matrix[13] - matrix[12] * matrix[5]);

            int determinant_11 =
                matrix[0] * (matrix[5] * matrix[14] - matrix[13] * matrix[6])
                - matrix[1] * (matrix[4] * matrix[14] - matrix[12] * matrix[6])
                + matrix[2] * (matrix[4] * matrix[13] - matrix[12] * matrix[5]);

            int determinant_12 =
                matrix[1] * (matrix[6] * matrix[11] - matrix[10] * matrix[7])
                - (matrix[2] * (matrix[5] * matrix[11] - matrix[9] * matrix[7]))
                + matrix[3] * (matrix[5] * matrix[10] - matrix[9] * matrix[6]);

            int determinant_13 =
                matrix[0] * (matrix[6] * matrix[11] - matrix[10] * matrix[7])
                - matrix[2] * (matrix[4] * matrix[11] - matrix[8] * matrix[7])
                + matrix[3] * (matrix[4] * matrix[10] - matrix[8] * matrix[6]);

            int determinant_14 =
                matrix[0] * (matrix[5] * matrix[11] - matrix[9] * matrix[7])
                - matrix[1] * (matrix[4] * matrix[11] - matrix[8] * matrix[7])
                + matrix[3] * (matrix[4] * matrix[9] - matrix[8] * matrix[5]);

            int determinant_15 =
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

        static Mat4 transposed(const Mat4& mat4) {
            return Mat4(
                mat4[0], mat4[4], mat4[8], mat4[12],
                mat4[1], mat4[5], mat4[9], mat4[13],
                mat4[2], mat4[6], mat4[10], mat4[14],
                mat4[3], mat4[7], mat4[11], mat4[15]
            );
        }

        const T* GetPointer() const {
            return &m_Data[0];
        }

        Mat4 operator+(const Mat4& other) const {
            return Mat4(
                m_Data[0] + other[0], m_Data[1] + other[1], m_Data[2] + other[2], m_Data[3] + other[3],
                m_Data[4] + other[4], m_Data[5] + other[5], m_Data[6] + other[6], m_Data[7] + other[7],
                m_Data[8] + other[8], m_Data[9] + other[9], m_Data[10] + other[10], m_Data[11] + other[11],
                m_Data[12] + other[12], m_Data[13] + other[13], m_Data[14] + other[14], m_Data[15] + other[15]
            );
        }

        void operator+=(const Mat4& other) {
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

        Mat4 operator-(const Mat4& other) const {
            return Mat4(
                m_Data[0] - other[0], m_Data[1] - other[1], m_Data[2] - other[2], m_Data[3] - other[3],
                m_Data[4] - other[4], m_Data[5] - other[5], m_Data[6] - other[6], m_Data[7] - other[7],
                m_Data[8] - other[8], m_Data[9] - other[9], m_Data[10] - other[10], m_Data[11] - other[11],
                m_Data[12] - other[12], m_Data[13] - other[13], m_Data[14] - other[14], m_Data[15] - other[15]
            );
        }

        void operator-=(const Mat4& other) {
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

        Mat4 operator*(const Mat4& other) const {
            T v_0 = m_Data[0] * other[0] + m_Data[1] * other[4] + m_Data[2] * other[8] + m_Data[3] * other[12];
            T v_1 = m_Data[0] * other[1] + m_Data[1] * other[5] + m_Data[2] * other[9] + m_Data[3] * other[13];
            T v_2 = m_Data[0] * other[2] + m_Data[1] * other[6] + m_Data[2] * other[10] + m_Data[3] * other[14];
            T v_3 = m_Data[0] * other[3] + m_Data[1] * other[7] + m_Data[2] * other[11] + m_Data[3] * other[15];
            T v_4 = m_Data[4] * other[0] + m_Data[5] * other[4] + m_Data[6] * other[8] + m_Data[7] * other[12];
            T v_5 = m_Data[4] * other[1] + m_Data[5] * other[5] + m_Data[6] * other[9] + m_Data[7] * other[13];
            T v_6 = m_Data[4] * other[2] + m_Data[5] * other[6] + m_Data[6] * other[10] + m_Data[7] * other[14];
            T v_7 = m_Data[4] * other[3] + m_Data[5] * other[7] + m_Data[6] * other[11] + m_Data[7] * other[15];
            T v_8 = m_Data[8] * other[0] + m_Data[9] * other[4] + m_Data[10] * other[8] + m_Data[11] * other[12];
            T v_9 = m_Data[8] * other[1] + m_Data[9] * other[5] + m_Data[10] * other[9] + m_Data[11] * other[13];
            T v_10 = m_Data[8] * other[2] + m_Data[9] * other[6] + m_Data[10] * other[10] + m_Data[11] * other[14];
            T v_11 = m_Data[8] * other[3] + m_Data[9] * other[7] + m_Data[10] * other[11] + m_Data[11] * other[15];
            T v_12 = m_Data[12] * other[0] + m_Data[13] * other[4] + m_Data[14] * other[8] + m_Data[15] * other[12];
            T v_13 = m_Data[12] * other[1] + m_Data[13] * other[5] + m_Data[14] * other[9] + m_Data[15] * other[13];
            T v_14 = m_Data[12] * other[2] + m_Data[13] * other[6] + m_Data[14] * other[10] + m_Data[15] * other[14];
            T v_15 = m_Data[12] * other[3] + m_Data[13] * other[7] + m_Data[14] * other[11] + m_Data[15] * other[15];
            return Mat4(
                v_0, v_1, v_2, v_3,
                v_4, v_5, v_6, v_7,
                v_8, v_9, v_10, v_11,
                v_12, v_13, v_14, v_15
            );
        }

        void operator*=(const Mat4& other) {
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

        Vec4<T> operator*(const Vec4<T>& other) {
            return Vec4<T>(
                m_Data[0] * other.x + m_Data[1] * other.y * m_Data[2] * other.z + m_Data[3] * other.w,
                m_Data[4] * other.x + m_Data[5] * other.y * m_Data[6] * other.z + m_Data[7] * other.w,
                m_Data[8] * other.x + m_Data[9] * other.y * m_Data[10] * other.z + m_Data[11] * other.w,
                m_Data[12] * other.x + m_Data[13] * other.y * m_Data[14] * other.z + m_Data[15] * other.w
            );
        }

        Mat4 operator*(T scalar) const {
            return Mat4(
                m_Data[0] * scalar, m_Data[1] * scalar, m_Data[2] * scalar, m_Data[3] * scalar,
                m_Data[4] * scalar, m_Data[5] * scalar, m_Data[6] * scalar, m_Data[7] * scalar,
                m_Data[8] * scalar, m_Data[9] * scalar, m_Data[10] * scalar, m_Data[11] * scalar,
                m_Data[12] * scalar, m_Data[13] * scalar, m_Data[14] * scalar, m_Data[15] * scalar
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
            m_Data[9] *= scalar;
            m_Data[10] *= scalar;
            m_Data[11] *= scalar;
            m_Data[12] *= scalar;
            m_Data[13] *= scalar;
            m_Data[14] *= scalar;
            m_Data[15] *= scalar;
        }

        Mat4 operator/(T scalar) const {
            T inv_scalar = T(1) / scalar;
            return Mat4(
                m_Data[0] * inv_scalar, m_Data[1] * inv_scalar, m_Data[2] * inv_scalar, m_Data[3] * inv_scalar,
                m_Data[4] * inv_scalar, m_Data[5] * inv_scalar, m_Data[6] * inv_scalar, m_Data[7] * inv_scalar,
                m_Data[8] * inv_scalar, m_Data[9] * inv_scalar, m_Data[10] * inv_scalar, m_Data[11] * inv_scalar,
                m_Data[12] * inv_scalar, m_Data[13] * inv_scalar, m_Data[14] * inv_scalar, m_Data[15] * inv_scalar
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
            m_Data[9] *= inv_scalar;
            m_Data[10] *= inv_scalar;
            m_Data[11] *= inv_scalar;
            m_Data[12] *= inv_scalar;
            m_Data[13] *= inv_scalar;
            m_Data[14] *= inv_scalar;
            m_Data[15] *= inv_scalar;
        }

        T& operator[](size_t idx) {
            return m_Data[idx];
        }

        T operator[](size_t idx) const {
            return m_Data[idx];
        }

        operator std::string() const {
            return fmt::format("|{} {} {} {}|\n|{} {} {} {}|\n|{} {} {} {}|\n|{} {} {} {}|",
                m_Data[0], m_Data[1], m_Data[2], m_Data[3],
                m_Data[4], m_Data[5], m_Data[6], m_Data[7],
                m_Data[8], m_Data[9], m_Data[10], m_Data[11],
                m_Data[12], m_Data[13], m_Data[14], m_Data[15]
            );
        }
        friend std::ostream& operator<<(std::ostream& os, const Mat4& mat) {
            os << fmt::format("|{} {} {} {}|\n|{} {} {} {}|\n|{} {} {} {}|\n|{} {} {} {}|",
                mat[0], mat[1], mat[2], mat[3],
                mat[4], mat[5], mat[6], mat[7],
                mat[8], mat[9], mat[10], mat[11],
                mat[12], mat[13], mat[14], mat[15]
            );
            return os;
        }
    };
}
#endif // !MAT4_H

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