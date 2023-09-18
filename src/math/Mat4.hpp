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
	class Mat4 {
	private:
		std::array<float, 16> m_Data;
	public:
		Mat4();
		Mat4(float v);
		Mat4(std::array<float, 16> data);
		Mat4(float v_0, float v_1, float v_2, float v_3, float v_4, float v_5, float v_6, float v_7, float v_8, float v_9, float v_10, float v_11, float v_12, float v_13, float v_14, float v_15);
		// Return a pointer to the first element in the matrix
        [[nodiscard]] const float* GetPointer() const;
		[[nodiscard]] float determinant() const;
		void inverse();
		void transpose();
        [[nodiscard]] static Mat4 inversed(const Mat4& matrix);
		[[nodiscard]] static Mat4 transposed(const Mat4& mat4);
		Mat4 operator+(const Mat4& other) const;
		void operator+=(const Mat4& other);
		Mat4 operator-(const Mat4& other) const;
		void operator-=(const Mat4& other);
		Mat4 operator*(const Mat4& other) const;
		void operator*=(const Mat4& other);
		Vec4 operator*(const Vec4& other);
		Mat4 operator*(float scalar) const;
		void operator*=(float scalar);
		Mat4 operator/(float scalar) const;
		void operator/=(float scalar);
		float& operator[](size_t idx);
		float operator[](size_t idx) const;
		operator std::string() const;
		friend std::ostream& operator<<(std::ostream& os, const Mat4& mat);
	};

	[[nodiscard]] Mat4 translationRemoved(const Mat4& mat);
	[[nodiscard]] Mat4 translate(const Vec3& vec);
	[[nodiscard]] Mat4 scale(const Vec3& vec);
	[[nodiscard]] Mat4 rotate(const Vec3& axis, float theta);
	[[nodiscard]] Mat4 perspective(float fov, float aspect, float near_dist, float far_dist);
	[[nodiscard]] Mat4 orthographic(float bottom, float top, float left, float right, float _near, float _far);
	[[nodiscard]] Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
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