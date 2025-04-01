#pragma once

#include <iostream>
#include <fstream>
#include <optional>
#include <filesystem>

#include <sztronics/miscellaneous/Serialization.hpp>

#define DEFAULT_STORAGE_FILE "userdata.arc"

/// @brief Manages long-term variable storage by writing
///        and reading key-value pairs from a file.

// todo: make search log(n)
class Archivist
{
    private:
    std::string filename;
    Archivist& operator=(Archivist&) = delete;
    Archivist(Archivist&) = delete;
    int n_entries;
    std::fstream file;

    /// @brief Single locator-data pair from a file.
    struct Storage_entry
    {   
        // + uint16 locator_size
        // + uint data_size
        std::string locator;
        std::vector<char> data;
    };
    /// @brief Writes given entry over file contents, starting from given position.
    /// @warning Does NOT relocate contents if new entry overlaps with the next one.
    /// @return Whether no files occurred when interacting with file.
    bool write_entry_at(unsigned at_pos, Storage_entry& entry);

    /// @brief Seeks entry that comes after entry starting at prev_pos.
    /// @return Position of next entry's start or nothing if EOF is reached.
    std::optional<unsigned> next_entry(unsigned prev_pos);

    /// @brief Reads the contents of entry that starts at given position.
    /// @param at_pos Position of entry start (not entry's data!)
    /// @return Serialized contents of the entry or nothing if an error occurred.
    std::optional<Serialized> read_entry_at(unsigned at_pos);

    /// @brief Locates an entry by its Locator.
    /// @return Position of entry or nothing if no such entry exists.
    std::optional<unsigned int> locate_entry(std::string locator);

    /// @brief Locate n-th entry from file start.
    /// @return Position of entry or nothing if EOF is reached.
    std::optional<unsigned> locate_idx(int index);
    
    public:
    ~Archivist();
    Archivist(std::string storage_file = DEFAULT_STORAGE_FILE);

    /// @brief Gets the global Archivist instance to store data used by multiple modules.
    static Archivist& get_default();
    
    /// @brief Retreives serialized data marked with given Locator.
    std::optional<Serialized> get_raw(std::string locator);

    /// @brief Creates or overwrites an entry with given Locator.
    /// @return
    bool put_raw(std::string locator, Serialized value);

    /// @brief Deletes data marked with given Locator.
    /// @return 
    bool del(std::string locator);

    template <typename Type>
    inline std::optional<Type> get(std::string locator)
    {
        std::optional<Serialized> serialized = get_raw(locator);
        if (!serialized.has_value()) {
            return {};
        }
        return { deserialize<Type>(serialized.value()) };
    }

    template <typename Type>
    inline bool put(std::string locator, Type value)
    {
        Serialized serialized = serialize(value);
        return put_raw(locator, serialized);
    }
    // void flush();
};