/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef MATH_H
#define MATH_H

#include <cmath>

namespace engine {
	constexpr float PI = 3.14159265358979323846f;
	constexpr float PI_OVER_180 = PI / 180.0f;
	constexpr float ONE_EIGHTY_OVER_PI = 180.0f / PI;

	inline float radians(float theta) {
		return theta * PI_OVER_180;
	}

	inline float degrees(float theta) {
		return theta * ONE_EIGHTY_OVER_PI;
	}

	template<typename T>
	inline int sign(T x) {
		return x > 0 ? 1 : (x < 0 ? -1 : 0);
	}

	template<typename T>
	inline T fract(T x) {
		return x - floor(x);
	}

	template<typename T>
	inline T fract1(T x) {
		return 1 - x + floorf(x);
	}
}

#endif // !MATH_H

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