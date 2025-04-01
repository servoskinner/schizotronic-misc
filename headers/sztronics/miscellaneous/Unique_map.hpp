#pragma once

#include <sztronics/miscellaneous/Unique.hpp>

#include <type_traits>
#include <map>
#include <iterator>
#include <functional>
#include <memory>
#include <utility>



template <typename Type>
class Unique_map_base {

public:
    using map_t = std::map<uint32_t, Type>;
    using map_iter_t = typename map_t::iterator;
    using map_const_iter_t = typename map_t::const_iterator;

    class iterator;
    class const_iterator;

    virtual uint32_t get_uid(const Type& unique) = 0;
    
    inline void emplace(Type&& value) {
        map.emplace(get_uid(value), std::move(value)); 
    }

    inline void clear() { map.clear(); }
    inline void swap(Unique_map_base<Type>& other) { map.swap(other.map); }
    inline iterator find(uint32_t key) { return iterator(map.find(key)); }

    inline Type& operator[](uint32_t key) { 
        auto access_iter = map.find(key);
        if (access_iter == map.end()) {
            throw std::out_of_range("Unique_map::operator[]: Element does not exist");
        }
        return access_iter->second;
    }

    inline bool has_key(uint32_t key) const {
        for (const std::pair<uint32_t, Type>& entry : map) {
            if (entry.first == key) {
                return true;
            }
        }
        return false;
    }
    inline size_t size() const { return map.size(); }

    inline void erase(uint32_t key) { map.erase(key); }
    inline void erase(iterator iter) { map.erase(iter.iter); }
    inline void erase(const_iterator iter) { map.erase(iter.iter); }

    inline iterator begin() { return iterator(map.begin()); }
    inline const_iterator begin() const { return const_iterator(map.begin()); }
    inline const_iterator cbegin() const noexcept { return const_iterator(map.cbegin()); }

    inline iterator end() { return iterator(map.end()); }
    inline const_iterator end() const { return const_iterator(map.end()); }
    inline const_iterator cend() const noexcept { return const_iterator(map.end()); }

    class iterator {
        private:
        map_iter_t iter;
        
        public:
        iterator(map_iter_t map_iter) : iter(map_iter) {}

        Type& operator->() { return iter->second; }
        Type& operator*() { return iter->second; }

        iterator& operator++() { 
            ++iter;
            return *this;
        }

        bool operator==(const iterator& other) const { return iter == other.iter; }
        bool operator!=(const iterator& other) const { return iter != other.iter; }
        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const;
    };

    class const_iterator {
        private:
        map_const_iter_t iter;

        public:
        const_iterator(map_const_iter_t map_const_iter) : iter(map_const_iter) {}

        const Type& operator->() { return iter->second; }
        const Type& operator*() { return iter->second; }

        const_iterator& operator++() { 
            ++iter;
            return *this;
        }

        bool operator==(const const_iterator& other) const { return iter == other.iter; }
        bool operator!=(const const_iterator& other) const { return iter == other.iter; }
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
    };

protected:
    map_t map = {};
};

template<typename Type>
class Unique_map : public Unique_map_base<Type> {
    static_assert(std::is_base_of<Unique, Type>::value, "Unique_map contents or elements they refer to must have Unique as their base.");
    virtual uint32_t get_uid(const Type& unique) override { return unique.get_uid(); }
};

template<typename Type>
class Unique_map<std::unique_ptr<Type>> : public Unique_map_base<std::unique_ptr<Type>> {
    static_assert(std::is_base_of<Unique, Type>::value, "Unique_map contents or elements they refer to must have Unique as their base.");
    virtual uint32_t get_uid(const std::unique_ptr<Type>& unique) override { return unique->get_uid(); }
};

template<typename Type>
class Unique_map<std::shared_ptr<Type>> : public Unique_map_base<std::shared_ptr<Type>> {
    static_assert(std::is_base_of<Unique, Type>::value, "Unique_map contents or elements they refer to must have Unique as their base.");
    using BaseType = Unique_map_base<std::reference_wrapper<Type>>;

    virtual uint32_t get_uid(const std::shared_ptr<Type>& unique) override { return unique->get_uid(); }

    void insert(const std::shared_ptr<Type>& value) {
        BaseType::map[get_uid(value)] = value;
    }
};

template<typename Type>
class Unique_map<std::reference_wrapper<Type>> : public Unique_map_base<std::reference_wrapper<Type>> {
    static_assert(std::is_base_of<Unique, Type>::value);
    using BaseType = Unique_map_base<std::reference_wrapper<Type>>;

    virtual uint32_t get_uid(const std::reference_wrapper<Type>& unique) override { return unique.get().get_uid(); }

    void insert(const std::reference_wrapper<Type>& value) {
        BaseType::map[get_uid(value)] = value;
    }
};


template <typename Type>
bool Unique_map_base<Type>::iterator::operator==(const Unique_map_base<Type>::const_iterator& other) const { return iter == other.iter; }
template <typename Type>
bool Unique_map_base<Type>::iterator::operator!=(const Unique_map_base<Type>::const_iterator& other) const { return iter != other.iter; }
template <typename Type>
bool Unique_map_base<Type>::const_iterator::operator==(const Unique_map_base<Type>::iterator& other) const { return iter == other.iter; }
template <typename Type>
bool Unique_map_base<Type>::const_iterator::operator!=(const Unique_map_base<Type>::iterator& other) const { return iter == other.iter; }