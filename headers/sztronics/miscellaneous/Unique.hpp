#pragma once

#include <vector>
#include <mutex>
#include <stdexcept>
#include <optional>

#define UNIQUE_ENTITY_LIMIT 4096

/// @brief A unique 32-bit unsigned ID for comparing objects.
class Unique 
{
    private:
    uint32_t id;
    bool valid = true;

    static std::vector<bool> claimed_ids;
    static std::mutex id_mutex;

    // Non-copyable
    Unique(const Unique& other) = delete;
    Unique& operator= (const Unique& other) = delete;
    
    public:
    static bool is_taken(unsigned int id);
    inline unsigned int get_uid() const { return id; }
    inline bool is_valid() const { return valid; }

    // Movable
    Unique(Unique&& other) noexcept;
    Unique& operator= (Unique&& other) noexcept;

    Unique();
    virtual ~Unique();

    virtual bool operator==(const Unique& other) const;
    virtual bool operator!=(const Unique& other) const;

    virtual bool operator==(const unsigned int& other) const;
    virtual bool operator!=(const unsigned int& other) const;

    virtual operator unsigned int() const;
};