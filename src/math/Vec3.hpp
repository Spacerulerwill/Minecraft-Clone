/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef VEC3_H
#define VEC3_H

#include <string>
#include <ostream>
#include <functional>

namespace engine {
	struct Vec3 {

		float x;
		float y;
		float z;

		Vec3();
		Vec3(float v);
		Vec3(float x, float y, float z);
		[[nodiscard]] float length() const;
		[[nodiscard]] static float dot(const Vec3& first, const Vec3& second);
		[[nodiscard]] static Vec3 cross(const Vec3& first, const Vec3& second);
		[[nodiscard]] static Vec3 normalised(const Vec3& vec);
		void normalise();
		Vec3 operator+(const Vec3& other) const;
		void operator+=(const Vec3& other);
		Vec3 operator-(const Vec3& other) const;
		void operator-=(const Vec3& other);
		Vec3 operator*(float scalar) const;
		void operator*=(float scalar);
		Vec3 operator/(float scalar);
		void operator/=(float scalar);
		bool operator==(const Vec3& other) const;
		bool operator!=(const Vec3& other) const;
		operator std::string();
		friend std::ostream& operator<<(std::ostream& os, const Vec3& vec);
	};
}

namespace std {
	template<> struct hash<engine::Vec3> {
		std::size_t operator()(engine::Vec3 const& vec) const noexcept {		
			return (((int)vec.x * 73856093) xor ((int)vec.y * 19349663) xor ((int)vec.z * 83492791)) % 3; // or use boost::hash_combine (see Discussion) https://en.cppreference.com/w/Talk:cpp/utility/hash
		}
	};
}
#endif // !VEC3_H

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