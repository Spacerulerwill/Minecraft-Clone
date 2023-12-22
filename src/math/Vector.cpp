/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <math/Vector.hpp>      // Vector
#include <assert.h>             // assert
#include <array>                // std::array
#include <numeric>              // std::inner_product
#include <cmath>                // std:sqrt
#include <initializer_list>     // std::initializer_list
#include <sstream>              // std::ostringstream
#include <string>               // std::string
#include <algorithm>            // std::copy,
#include <util/Concepts.hpp>    // Arithmetic 
#include <concepts>             // std:is_integral, std::is_floating_point, std::is_integral
#include <util/Log.hpp>

/*
=================
Vector operations
=================
*/

template<Arithmetic T, std::size_t size>
float Vector<T, size>::length() const requires (std::integral<T>) {
    return static_cast<float>(std::sqrt(std::inner_product(elems.begin(), elems.end(), elems.begin(), T(0))));
}

template<Arithmetic T, std::size_t size>
T Vector<T, size>::length() const requires (std::floating_point<T>)
{
    return T(std::sqrt(std::inner_product(elems.begin(), elems.end(), elems.begin(), T(0))));
}

template<Arithmetic T, std::size_t size>
T Vector<T, size>::dot(Vector<T, size> other) const {
    return T(std::inner_product(elems.begin(), elems.end(), other.elems.begin(), T(0)));
}

template<Arithmetic T, std::size_t size>
Vector<T, 3> Vector<T, size>::cross(const Vector<T, 3> other) const requires (size == 3) {
    T x = elems[1] * other[2] - elems[2] * other[1];
    T y = elems[2] * other[0] - elems[0] * other[2];
    T z = elems[0] * other[1] - elems[1] * other[0];
    return Vector<T, 3> {x, y, z};
}

template<Arithmetic T, std::size_t size>
void Vector<T, size>::normalize() requires (std::floating_point<T>) {
    T len = length();
    assert(len != 0);
    T invLen = T(1) / len;
    for (std::size_t i = 0; i < size; i++) {
        elems[i] *= invLen;
    }
}

template<Arithmetic T, std::size_t size>
Vector<T, size> Vector<T, size>::normalized() const requires (std::floating_point<T>) {
    T len = length();
    assert(len != 0);
    T invLen = T(1) / len;

    Vector<T, size> vec;
    for (std::size_t i = 0; i < size; i++) {
        vec[i] = elems[i] * invLen;
    }
    return vec;
}

/*
=====================
Arithmetic operations
=====================
*/

template<Arithmetic T, std::size_t size>
Vector<T, size> Vector<T, size>::operator+(const Vector<T, size>& other) const {
    Vector<T, size> result;
    for (std::size_t i = 0; i < size; i++) {
        result[i] = elems[i] + other[i];
    }
    return result;
}

template<Arithmetic T, std::size_t size>
Vector<T, size> Vector<T, size>::operator-(const Vector<T, size>& other) const {
    return *this + (other * -1);
}

template<Arithmetic T, std::size_t size>
Vector<T, size> Vector<T, size>::operator*(T scalar) const {
    Vector<T, size> result;
    for (std::size_t i = 0; i < size; i++) {
        result[i] = elems[i] * scalar;
    }
    return result;
}

template<Arithmetic T, std::size_t size>
Vector<T, size> Vector<T, size>::operator/(T scalar) const {
    assert(scalar != T(0));
    /*
    If T is an integral type, we must divide each element by scalar to do
    proper integer divison. as 1/X where x is integral, always resutls in 0
    */
    if constexpr (std::is_integral<T>()) {
        Vector<T, size> result;
        for (std::size_t i = 0; i < size; i++) {
            result[i] = elems[i] / scalar;
        }
        return result;
    }
    /*
    If T is a floating point type, we can calculate 1 / scalar and multiply
    by that instead
    */
    else {
        T inverseScalar = T(1) / scalar;
        return *this * inverseScalar;
    }
}

template<Arithmetic T, std::size_t size>
void Vector<T, size>::operator+=(const Vector<T, size>& other) {
    for (std::size_t i = 0; i < size; i++) {
        elems[i] += other[i];
    }
}

template<Arithmetic T, std::size_t size>
void Vector<T, size>::operator-=(const Vector<T, size>& other) {
    *this += (other * -1);
}

template<Arithmetic T, std::size_t size>
void Vector<T, size>::operator*=(T scalar) {
    for (std::size_t i = 0; i < size; i++) {
        elems[i] *= scalar;
    }
}

template<Arithmetic T, std::size_t size>
void Vector<T, size>::operator/=(T scalar) {
    assert(scalar != 0);
    /*
    If T is an integral type, we must divide each element by scalar to do
    proper integer divison. as 1/X where x is integral, always resutls in 0
    */
    if constexpr (std::is_integral<T>()) {
        for (std::size_t i = 0; i < size; i++) {
            elems[i] /= scalar;
        }
    }
    /*
    If T is a floating point type, we can calculate 1 / scalar and multiply
    by that instead.
    */
    else {
        T inverseScalar = T(1) / scalar;
        *this *= inverseScalar;
    }
}

template<Arithmetic T, std::size_t size>
void Vector<T, size>::operator%=(T divisor) requires (std::integral<T>)
{
    assert(divisor != 0);
    for (std::size_t i = 0; i < size; i++) {
        elems[i] %= divisor;
    }
}
template<Arithmetic T, std::size_t size>
Vector<T, size> Vector<T, size>::operator%(T divisor) const requires (std::integral<T>) {
    assert(divisor != 0);
    Vector<T, size> result;
    for (std::size_t i = 0; i < size; i++) {
        result[i] = elems[i] % divisor;
    }
    return result;
};


/*
====================
Comparison operators
====================
*/

template<Arithmetic T, std::size_t size>
bool Vector<T, size>::operator==(const Vector<T, size>& other) const {
    return elems == other.elems;
}

template<Arithmetic T, std::size_t size>
bool Vector<T, size>::operator!=(const Vector<T, size>& other) const {
    return !(*this == other);
}

/*
===========
Data access
===========
*/

template<Arithmetic T, std::size_t size>
T& Vector<T, size>::operator[](std::size_t idx) {
    return elems[idx];
}

template<Arithmetic T, std::size_t size>
T Vector<T, size>::operator[](std::size_t idx) const {
    return elems[idx];
}

template<Arithmetic T, std::size_t size>
const T* Vector<T, size>::GetPointer() const
{
    return &elems[0];
}

/*
=================
String conversion
=================
*/


template<Arithmetic T, std::size_t size>
Vector<T, size>::operator std::string() const {
    std::ostringstream oss;
    std::copy(elems.begin(), elems.end() - 1, std::ostream_iterator<T>(oss, ","));
    oss << elems.back();
    return oss.str();
}

/*
================================
Explicit template instantiations
================================
*/

template struct Vector<float, 2>;
template struct Vector<float, 3>;
template struct Vector<float, 4>;

template struct Vector<int, 2>;
template struct Vector<int, 3>;
template struct Vector<int, 4>;

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