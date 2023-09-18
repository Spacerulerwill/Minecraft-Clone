/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef MAT3_H
#define MAT3_H

#include <array>
#include <ostream>
#include <math/Math.hpp>

namespace engine {
	class Mat3 {
	private:
		std::array<float, 9> m_Data;
	public:
		Mat3();
		Mat3(float v);
		Mat3(std::array<float, 9> data);
		Mat3(float v_0, float v_1, float v_2, float v_3, float v_4, float v_5, float v_6, float v_7, float v_8);
        // Mutatively inverse matrix
		void inverse();
        // Mutatively tranpose matrix
		void transpose();
        // Return the inversed matrix
        static Mat3 inversed(const Mat3& mat3);
        // Return the tranposed matrix
		static Mat3 transposed(const Mat3& mat3);
        float determinant() const;
		Mat3 operator+(const Mat3& other) const;
		void operator+=(const Mat3& other);
		Mat3 operator-(const Mat3& other) const;
		void operator-=(const Mat3& other);
		Mat3 operator*(const Mat3& other) const;
		void operator*=(const Mat3& other);
		Mat3 operator*(float scalar) const;
		void operator*=(float scalar);
		Mat3 operator/(float scalar) const;
		void operator/=(float scalar);
		float& operator[](size_t idx);
		float operator[](size_t idx) const;
		operator std::string();
		friend std::ostream& operator<<(std::ostream& os, const Mat3& mat);
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