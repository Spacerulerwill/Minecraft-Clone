/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef VEC2_H
#define VEC2_H

#include <string>
#include <ostream>

namespace engine {
	struct Vec2 {

		float x;
		float y;

		Vec2();
		Vec2(float v);
		Vec2(float x, float y);
		[[nodiscard]] float length() const;
		[[nodiscard]] static float dot(const Vec2& first, const Vec2& second);
		[[nodiscard]] static Vec2 normalised(const Vec2& vec);
		void normalise();
		Vec2 operator + (const Vec2& other) const;
		void operator += (const Vec2& other);
		Vec2 operator - (const Vec2& other) const;
		void operator -= (const Vec2 & other);
		Vec2 operator * (float scalar) const;
		void operator *= (float scalar);
		Vec2 operator / (float scalar) const;
		void operator /= (float scalar);
		bool operator == (const Vec2& other) const;
		bool operator != (const Vec2& other) const;
		operator std::string();
		friend std::ostream& operator<<(std::ostream& os, const Vec2& vec);
	};
};

#endif // !VEC2_H

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