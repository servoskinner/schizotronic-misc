#include <sztronics/miscellaneous/Unique.hpp>
#include <iostream>

std::vector<bool> Unique::claimed_ids = {};
std::mutex Unique::id_mutex;

Unique::Unique()
{
    std::lock_guard<std::mutex> lock(id_mutex);
    bool id_found = false;
    for(int i = 0; i < UNIQUE_ENTITY_LIMIT; i++) {
        if (i == claimed_ids.size()) {
            claimed_ids.push_back(true);
            id = i;
            id_found = true;
            break;
        }
        if (!claimed_ids[i]) {
            claimed_ids[i] = true;
            id = i;
            id_found = true;
            break;
        }
    }
    if (!id_found) {
        throw std::runtime_error("Unique(): Unique entity limit exceeded");
    }
    valid = true;
}

Unique::Unique(Unique&& other) noexcept {
    id = other.id;
    valid = other.valid;
    other.valid = false;
}

Unique& Unique::operator=(Unique&& other) noexcept {
    id = other.id;
    valid = other.valid;
    other.valid = false;
    return *this;
}

Unique::~Unique()
{
    if (valid) {
        std::lock_guard<std::mutex> lock(id_mutex);
        claimed_ids[id] = false;
    }
}

bool Unique::is_taken(unsigned int id)
{
    std::lock_guard<std::mutex> lock(id_mutex);
    return id < claimed_ids.size() && claimed_ids[id];
}

bool Unique::operator==(const Unique& other) const 
{ return other.id == id;}
bool Unique::operator!=(const Unique& other) const 
{ return other.id != id;}

bool Unique::operator==(const unsigned int& other) const 
{ return other == id;}
bool Unique::operator!=(const unsigned int& other) const 
{ return other != id;}

Unique::operator unsigned int() const 
{ return id; }