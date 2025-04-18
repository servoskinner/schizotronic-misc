#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <math.h>

/// @brief generic 2D vector.
template <typename T>
struct Vector2
{
    T x;
    T y;

    constexpr Vector2(T x = T(), T y = T()) : x(x), y(y) {} 
    constexpr Vector2(const T (&list)[2]) : x(list[0]), y(list[1]) {}

    template <typename T2>
    constexpr inline operator Vector2<T2>() const { return Vector2<T2>(static_cast<T2>(x), static_cast<T2>(y)); }

    constexpr inline Vector2<T> operator-() const { return {-x, -y}; }

    template <typename T2>
    constexpr inline Vector2<T> operator+ (const Vector2<T2> other) const { return { static_cast<T>(x + other.x), static_cast<T>(y + other.y) }; }
    template <typename T2>
    constexpr inline Vector2<T> operator- (const Vector2<T2> other) const { return { static_cast<T>(x - other.x), static_cast<T>(y - other.y) }; }

    template <typename T2>
    constexpr inline Vector2<T>& operator+= (const Vector2<T2> other) { 
        x = static_cast<T>(x + other.x);
        y = static_cast<T>(y + other.y);
        return *this;
    }
    template <typename T2>
    constexpr inline Vector2<T>& operator-= (const Vector2<T2> other) { 
        x = static_cast<T>(x - other.x);
        y = static_cast<T>(y - other.y);
        return *this;
    }
    
    template <typename T2>
    constexpr inline Vector2<T> operator* (const Vector2<T2> other) const { return { static_cast<T>(x * other.x), static_cast<T>(y * other.y) }; }
    template <typename T2>
    constexpr inline Vector2<T> operator/ (const Vector2<T2> other) const { return { static_cast<T>(x / other.x), static_cast<T>(y / other.y) }; }

    template <typename T2>
    constexpr inline Vector2<T>& operator*= (const Vector2<T2> other) { 
        x = static_cast<T>(x * other.x);
        y = static_cast<T>(y * other.y);
        return *this;
    }
    template <typename T2>
    constexpr inline Vector2<T>& operator/= (const Vector2<T2> other) { 
        x = static_cast<T>(x / other.x);
        y = static_cast<T>(y / other.y);
        return *this;
    }

    template <typename T2>
    constexpr inline Vector2<T> operator+ (const T2 other) const { return { static_cast<T>(x + other), static_cast<T>(y + other) }; }
    template <typename T2>
    constexpr inline Vector2<T> operator- (const T2 other) const { return { static_cast<T>(x - other), static_cast<T>(y - other) }; }

    template <typename T2>
    constexpr inline Vector2<T>& operator+= (const T2 other) { 
        x = static_cast<T>(x + other); 
        y = static_cast<T>(y + other); 
        return *this; 
    }
    template <typename T2>
    constexpr inline Vector2<T>& operator-= (const T2 other) { 
        x = static_cast<T>(x - other); 
        y = static_cast<T>(y - other); 
        return *this; 
    }
    template <typename T2>
    constexpr inline Vector2<T> operator* (const T2 other) const { return { static_cast<T>(x * other), static_cast<T>(y * other) }; }
    template <typename T2>
    constexpr inline Vector2<T> operator/ (const T2 other) const { return { static_cast<T>(x / other), static_cast<T>(y / other) }; }

    template <typename T2>
    constexpr inline Vector2<T>& operator*= (const T2 other) { 
        x = static_cast<T>(x * other); 
        y = static_cast<T>(y * other); 
        return *this; 
    }
    template <typename T2>
    constexpr inline Vector2<T>& operator/= (const T2 other) { 
        x = static_cast<T>(x / other); 
        y = static_cast<T>(y / other); 
        return *this; 
    }

    template <typename T2>
    constexpr inline bool operator== (const Vector2<T2> other) const { return x == other.x && y == other.y; }
    template <typename T2>
    constexpr inline bool operator!= (const Vector2<T2> other) const { return x != other.x || y != other.y; }

    // Used to check 2D array index bounds and whether a point lies within given rectangle.
    // Note that "<" is not equivalent to "!(>=)" and so on.
    template <typename T2>
    constexpr inline bool operator< (const Vector2<T2> other) const { return x < other.x && y < other.y; }
    template <typename T2>
    constexpr inline bool operator<= (const Vector2<T2> other) const { return x <= other.x && y <= other.y; }
    template <typename T2>
    constexpr inline bool operator> (const Vector2<T2> other) const { return x > other.x && y > other.y; }
    template <typename T2>
    constexpr inline bool operator>= (const Vector2<T2> other) const { return x >= other.x && y >= other.y; }
    
    /// @brief manhattan (taxicab) metric length.
    constexpr inline T len_manh() { return std::abs(x) + std::abs(y); }
    /// @brief chessboard (chebyshev) metric length.
    constexpr inline T len_chess() { return std::max(abs(x), abs(y)); }
    /// @brief Euclidean metric
    constexpr inline double len() { return sqrt(static_cast<double>(x*x + y*y)); }

    /// @brief Clamps both coordinates independently, EXCLUDING THE UPPER BOUND
    template <typename T2>
    constexpr inline Vector2 clamp (const Vector2<T2>& upper_bound, const Vector2<T2>& lower_bound = {0, 0}) {
        return { std::max(lower_bound.x, std::min(x, static_cast<T>(upper_bound.x - 1))), 
                 std::max(lower_bound.y, std::min(y, static_cast<T>(upper_bound.y - 1)))};
    }

    constexpr inline operator std::string() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, Vector2<T> vec2) {
    return stream << "(" << vec2.x << ", " << vec2.y << ")";
}

namespace std {
    template <typename T>
    string to_string(Vector2<T> vec2) {
        return "(" + to_string(vec2.x) + ", " + to_string(vec2.y) + ")";
    }
}

typedef Vector2<int8_t>  Vector2b;
typedef Vector2<int16_t> Vector2h;
typedef Vector2<int32_t> Vector2i;
typedef Vector2<int64_t> Vector2l;

typedef Vector2<uint8_t>  Vector2ub;
typedef Vector2<uint16_t> Vector2uh;
typedef Vector2<uint32_t> Vector2u;
typedef Vector2<uint64_t> Vector2ul;

typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;