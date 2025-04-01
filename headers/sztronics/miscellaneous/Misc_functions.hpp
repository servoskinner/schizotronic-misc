#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <optional>
#include <stdint.h>

#include <sztronics/miscellaneous/Vector2i.hpp>

/// @brief Attempts to find an element in vector and remove it.
/// @return Whether the element was found and popped.
template <typename Iterable_type, typename Key_type>
inline bool pop_val(Iterable_type& container, const Key_type& key) 
{
    auto found = std::find(container.begin(), container.end(), key)
    if (found == container.end()) {
        return false;
    }
    container.erase(found);
    return true;
}

/// @brief  Finds an element in array and returns its index.
/// @return index if element was found, none if it was not.
template <typename Iterable_type, typename Key_type>
inline std::optional<size_t> find_idx(const Iterable_type container, const Key_type& key) 
{
    int index = 0;
    for(auto iter = container.cbegin(); iter != container.cend(); iter++, index++) {
        if(*iter == key) {
            return {index};
        }
    }
    return {};
}

/// @brief attempts to erase the vector element at given index
/// @returns whether deleting was successful
template <typename Iterable_type>
inline void pop_idx(Iterable_type& container, const size_t& index) //Remove element with target index from a vector
{
    size_t wrapped_idx = index % container.size();
    auto iter = container.begin() + wrapped_idx;
    container.erase(iter);
}

/// @brief Split text into lines to fit inside given line width constraint.
std::vector<std::string> wrap_text(const std::string& text, int32_t line_width);

// character classes

/// @return Whether a character is a non-special symbol
constexpr inline bool is_printable(uint32_t ch)
{
    return (31 < ch && ch < 127);
}

/// @return Whether a character is a legal name symbol ([a-zA-Z0-9_])
constexpr inline bool is_wordch(uint32_t ch) 
{
    return ('0' < ch && ch < '9') || \
           ('a' < ch && ch < 'z') || \
           ('A' < ch && ch < 'Z') ||
            ch == '_';
}

/// @brief Maps 2d array indices into 1d array index.
constexpr inline int to_1d(std::pair<uint32_t, uint32_t> indices, uint32_t width)
{
    return indices.first * width + indices.second;
}
/// @brief Maps 2d array indices into 1d array index.
constexpr inline int to_1d(Vector2i indices, uint32_t width)
{
    return indices.x * width + indices.y;
}
/// @brief Maps 2d array indices into 1d array index.
constexpr inline int to_1d(uint32_t x, uint32_t y, uint32_t width)
{
    return x * width + y;
}

/// @brief Maps 1d array index into 2d array pair of indices.
constexpr inline Vector2i to_2d(uint32_t index, uint32_t width)
{
    return {index / width, index % width};
}