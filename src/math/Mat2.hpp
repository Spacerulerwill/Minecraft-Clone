/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef MAT2_H
#define MAT2_H

#include <array>
#include <ostream>
#include <math/Math.hpp>

namespace engine {
	class Mat2 {
	private:
		std::array<float, 4> m_Data;
	public:
		Mat2();
		Mat2(float v);
		Mat2(std::array<float, 4> data);
		Mat2(float v_0, float v_1, float v_2, float v_3);
        // Mutatively inverse matrix
		void inverse();
        // Mutatively transpose matrix
		void tranpose();
        inline float determinant() const;
        // Return the inversed matrix
		static Mat2 inversed(const Mat2& matrix);
        // Return the transposed matrix
		static Mat2 transposed(const Mat2& mat2);
		Mat2 operator+(const Mat2& other) const;
		void operator+=(const Mat2& other);
		Mat2 operator-(const Mat2& other) const;
		void operator-=(const Mat2& other);
		Mat2 operator*(const Mat2& other) const;
		void operator*=(const Mat2& other);
		Mat2 operator*(float scalar) const;
		void operator*=(float scalar);
		Mat2 operator/(float scalar) const;
		void operator/=(float scalar);
		float& operator [](size_t idx);
		float operator [](size_t idx) const;
		operator std::string();
		friend std::ostream& operator<<(std::ostream& os, const Mat2& mat);
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