#ifndef VECTOR_H
#define VECTOR_H

#include <array>
#include <string>
#include <concepts>
#include <cmath>

template<typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

template<Arithmetic T, std::size_t size>
struct Vector {
    using value_type = T;
    std::array<T, size> elems{};

    // Vector operations
    float length() const requires (std::integral<T>);
    T length() const requires (std::floating_point<T>);
    T dot(Vector<T, size> other) const;
    Vector<T, 3> cross(const Vector<T, 3> other) const requires (size == 3);
    void normalize() requires (std::floating_point<T>);
    Vector<T, size> normalized() const requires (std::floating_point<T>);

    // Arithmetic operations
    Vector<T, size> operator+(const Vector<T, size>& other) const;
    Vector<T, size> operator-(const Vector<T, size>& other) const;
    Vector<T, size> operator*(T scalar) const;
    Vector<T, size> operator/(T scalar) const;
    void operator+=(const Vector<T, size>& other);
    void operator-=(const Vector<T, size>& other);
    void operator*=(T scalar);
    void operator/=(T scalar);
    void operator%=(T divisor) requires (std::integral<T>);
    Vector<T, size> operator%(T divisor) const requires (std::integral<T>);

    // Comparison operators
    bool operator==(const Vector<T, size>& other) const;
    bool operator!=(const Vector<T, size>& other) const;

    // Data access
    T& operator[](std::size_t idx);
    T operator[](std::size_t idx) const;
    const T* GetPointer() const;

    // Type conversion
    operator std::string() const;
    template<Arithmetic NewT>
    operator Vector<NewT, size>() const {
        Vector<NewT, size> result;
        for (std::size_t i = 0; i < size; i++) {
            result[i] = static_cast<NewT>(elems[i]);
        }
        return result;
    }
};

template<Arithmetic T, std::size_t size>
Vector<T, size> operator*(T x, const Vector<T, size>& vec) {
    return vec * x;
}

template<Arithmetic T, std::size_t size>
inline std::ostream& operator<<(std::ostream& out, const Vector<T, size>& vec) {
    for (std::size_t i = 0; i < size; i++) {
        out << vec[i] << ' ';
    }
    return out;
}

using dVec3 = Vector<double, 3>;

#endif //!VECTOR_H