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

template <typename Type>
struct Serializable_vector_traits : std::false_type {};

template <typename Type>
struct Serializable_vector_traits<std::vector<Type>> : std::is_trivially_copyable<Type> {
    using Elem = Type;
};

template <typename Type>
struct Serializable_map_traits : std::false_type {};

template <typename Keytype, typename Valtype>
struct Serializable_map_traits<std::unordered_map<Keytype, Valtype>> : std::conjunction<
                                                                       std::is_trivially_copyable<Keytype>,
                                                                       std::is_trivially_copyable<Valtype>> {
    using Key = Keytype;
    using Value = Valtype;
};

/// @brief Restores a map with fixed-size keys and values from a byte vector
template <typename Type>
Type deserialize(const Serialized& serialized)
{
    using Vec_traits = Serializable_vector_traits<Type>;
    using Map_traits = Serializable_map_traits<Type>;

    if constexpr (std::is_trivially_copyable<Type>::value) { // Trivial types ________

        Type new_obj;
        if(serialized.size() != sizeof(Type)) {
            throw std::runtime_error("deserialize(): byte vector size is not equal to type size");
        }
        std::memcpy(&new_obj, serialized.data(), sizeof(Type));
        return new_obj;
    }
    else if constexpr (std::is_same<Type, std::string>::value) {

        if (serialized.empty()) {
            return "";
        }
        return std::string(serialized.data(), serialized.size());
    }
    else if constexpr (Vec_traits::value) { // Vector ___________________________________
        
        using Elem = typename Vec_traits::Elem;

        if (serialized.size() % sizeof(Elem) != 0) {
            throw std::runtime_error("deserialize<vector>(): byte vector not divisible by type size");
        }
        unsigned vector_size = serialized.size() / sizeof(Elem);
        
        Type vector;
        vector.resize(vector_size);
    
        std::memcpy((char*)(vector.data()), serialized.data(), sizeof(Elem)*vector_size);
        return vector;
    }
    else if constexpr (Map_traits::value) { // Map ________________________________________

        using Key = typename Map_traits::Key;
        using Value = typename Map_traits::Value;

        if (serialized.size() % (sizeof(Map_traits::Key) + sizeof(Map_traits::Value))) {
            throw std::runtime_error("deserialize_map(): byte vector size not divisible by key+value size");
        }
        unsigned map_size = serialized.size() / (sizeof(Map_traits::Key) + sizeof(Map_traits::Value));
        Type map;

        for (unsigned int i = 0; i < map_size; i++) {
            Serialized serialized_key(serialized.begin() + i*(sizeof(Map_traits::Key) + sizeof(Map_traits::Value)), \
                                    serialized.begin() + i*(sizeof(Map_traits::Key) + sizeof(Map_traits::Value)) + sizeof(Map_traits::Key));
            typename Map_traits::Key key = deserialize<Map_traits::Key>(serialized_key);
            Serialized serialized_val(serialized.begin() + i*(sizeof(Map_traits::Key) + sizeof(Map_traits::Value)) + sizeof(Map_traits::Key), \
                                    serialized.begin() + i*(sizeof(Map_traits::Key) + sizeof(Map_traits::Value)) + sizeof(Map_traits::Key) \
                                    + sizeof(Map_traits::Value));
            typename Map_traits::Value val = deserialize<Map_traits::Value>(serialized_val);
            map.insert({key, val});
        }
        return map;
    }
    else { // Unsupported __________________________________________________________________
        static_assert(false, "deserialize() only works with trivially copyable values or containers (umaps, vectors) of them.");
    }
}