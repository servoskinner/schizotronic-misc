#pragma once

#include <algorithm>

/// @brief 2D integer vector.
struct Vector2i
{
    int32_t x;
    int32_t y;

    constexpr Vector2i(int x = 0, int y = 0) : x(x), y(y) {} 
    constexpr Vector2i(const int32_t (&list)[2]) : x(list[0]), y(list[1]) {}

    constexpr inline Vector2i operator+ (const Vector2i& other) const { return { x + other.x, y + other.y }; }
    constexpr inline Vector2i operator- (const Vector2i& other) const { return { x - other.x, y - other.y }; }

    constexpr inline Vector2i operator* (const int& other) const { return { x * other, y * other }; }
    constexpr inline Vector2i operator/ (const int& other) const { return { x / other, y / other }; }

    constexpr inline Vector2i operator* (const double& other) const { return { (int)((double)x * other), (int)((double)y * other) }; }
    constexpr inline Vector2i operator/ (const double& other) const { return { (int)((double)x / other), (int)((double)y / other) }; }

    constexpr inline bool operator== (const Vector2i& other) const { return x == other.x && y == other.y; }
    constexpr inline bool operator!= (const Vector2i& other) const { return x != other.x || y != other.y; }

    // Used to check 2D array index bounds and whether a point lies within given rectangle.
    // Note that "<" is not equivalent to "!(>=)" and so on.
    constexpr inline bool operator< (const Vector2i& other) const { return x < other.x && y < other.y; }
    constexpr inline bool operator<= (const Vector2i& other) const { return x <= other.x && y <= other.y; }
    constexpr inline bool operator> (const Vector2i& other) const { return x > other.x && y > other.y; }
    constexpr inline bool operator>= (const Vector2i& other) const { return x >= other.x && y >= other.y; }
    
    /// @brief manhattan (taxicab) metric length.
    constexpr inline int len_manh() { return abs(x) + abs(y); }
    /// @brief chessboard (chebyshev) metric length.
    constexpr inline int len_chess() { return std::max(abs(x), abs(y)); }

    /// @brief Clamps both coordinates independently, EXCLUDING THE UPPER BOUND
    constexpr inline Vector2i clamp (const Vector2i& upper_bound, const Vector2i& lower_bound = {0, 0}) {
        return { std::max(lower_bound.x, std::min(x, upper_bound.x - 1)), std::max(lower_bound.y, std::min(y, upper_bound.y - 1))};
    }
};