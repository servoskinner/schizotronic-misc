#pragma once

#include <vector>
#include <cstring>
#include <unordered_map>
#include <utility>
#include <cstdlib>

/// @brief Raw data suitable for network transfer or writing to file.
typedef std::vector<char> Serialized;

/// @brief Packs a fixed-size struct into a byte vector.
template <typename Type>
Serialized serialize(const Type& object)
{
    static_assert(std::is_trivially_copyable<Type>::value, \
                  "serialize() only works with trivially copyable values or containers of them.");

    std::vector<char> serialized(sizeof(Type));
    std::memcpy(serialized.data(), &object, sizeof(Type));
    return serialized;
}

/// @brief Serializes a string.
template <>
inline Serialized serialize(const std::string& str) {
    return Serialized(str.begin(), str.end());
}

/// @brief Packs a vector of fixed-size structs or primitive types into a byte vector.
template <typename Type>
Serialized serialize(const std::vector<Type>& vector)
{
    static_assert(std::is_trivially_copyable<Type>::value, \
                  "serialize() only works with trivially copyable values or containers of them.");

    Serialized serialized(vector.size()*sizeof(Type));
    
    std::memcpy(serialized.data(), \
                (char*)(vector.data()), \
                vector.size()*sizeof(Type));
    return serialized;
}

/// @brief Packs a map with fixed-size keys and values into a byte vector.
template <typename Keytype, typename Valtype>
Serialized serialize(const std::unordered_map<Keytype, Valtype>& map)
{
    static_assert(std::is_trivially_copyable<Keytype>::value, \
                  "serialize() only works with trivially copyable values or containers of them.");
    static_assert(std::is_trivially_copyable<Valtype>::value, \
                  "serialize() only works with trivially copyable values or containers of them.");

    Serialized serialized;
    for (std::pair<Keytype, Valtype> pair : map) {
        Serialized serialized_key = serialize(pair.first);
        Serialized serialized_value = serialize(pair.second);
        // append to result
        std::copy(serialized_key.begin(), serialized_key.end(), std::back_inserter(serialized)); 
        std::copy(serialized_value.begin(), serialized_value.end(), std::back_inserter(serialized)); 
    }
    return serialized;
}

/// @brief Restores a fixed-size struct from a byte vector.
template <typename Type>
Type deserialize(const Serialized& serialized)
{
    static_assert(std::is_trivially_copyable<Type>::value, \
                  "deserialize() only works with trivially copyable values or containers of them.");

    Type new_obj;
    if(serialized.size() != sizeof(Type)) {
        throw std::runtime_error("deserialize_struct(): byte vector size is not equal to type size");
    }
    std::memcpy(&new_obj, serialized.data(), sizeof(Type));
    return new_obj;
}

/// @brief Restores a string from a byte vector.
template <>
inline std::string deserialize(const Serialized& serialized) {
    if (serialized.empty()) {
        return std::string();
    }

    return std::string(serialized.data(), serialized.size());
}

/// @brief Restores a vector of fixed-size structs or primitive types from a byte vector.
template <typename Type>
std::vector<Type> deserialize(const Serialized& serialized)
{
    static_assert(std::is_trivially_copyable<Type>::value, \
                  "deserialize() only works with trivially copyable values or containers of them.");

    if (serialized.size() % sizeof(Type) != 0) {
        throw std::runtime_error("deserialize_vector(): byte vector not divisible by type size");
    }
    unsigned vector_size = serialized.size() / sizeof(Type);
    
    std::vector<Type> vector;
    vector.resize(vector_size);

    std::memcpy((char*)(vector.data()), serialized.data(), sizeof(Type)*vector_size);
    return vector;
}

/// @brief Restores a map with fixed-size keys and values from a byte vector
template <typename Keytype, typename Valtype>
std::unordered_map<Keytype, Valtype> deserialize(const Serialized& serialized)
{
    static_assert(std::is_trivially_copyable<Keytype>::value, \
                  "deserialize() only works with trivially copyable values or containers of them.");
    static_assert(std::is_trivially_copyable<Valtype>::value, \
                  "deserialize() only works with trivially copyable values or containers of them.");
    
    if (serialized.size() % (sizeof(Keytype) + sizeof(Valtype))) {
        throw std::runtime_error("deserialize_map(): byte vector size not divisible by key+value size");
    }
    unsigned map_size = serialized.size() / (sizeof(Keytype) + sizeof(Valtype));
    std::unordered_map<Keytype, Valtype> map;

    for (unsigned int i = 0; i < map_size; i++) {
        Serialized serialized_key(serialized.begin() + i*(sizeof(Keytype) + sizeof(Valtype)), \
                                  serialized.begin() + i*(sizeof(Keytype) + sizeof(Valtype)) + sizeof(Keytype));
        Keytype key = deserialize<Keytype>(serialized_key);
        Serialized serialized_val(serialized.begin() + i*(sizeof(Keytype) + sizeof(Valtype)) + sizeof(Keytype), \
                                  serialized.begin() + i*(sizeof(Keytype) + sizeof(Valtype)) + sizeof(Keytype) \
                                  + sizeof(Valtype));
        Valtype val = deserialize<Valtype>(serialized_val);
        map.insert({key, val});
    }
    return map;
}