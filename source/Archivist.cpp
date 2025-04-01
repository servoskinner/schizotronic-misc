#include <sztronics/miscellaneous/Archivist.hpp>

Archivist::Archivist(std::string storage_file) : filename(storage_file)
{   
    file.open(storage_file, std::ios::in | std::ios::out | std::ios::binary);
    if (file.bad() || file.fail()) {
        // try to create file if an error occurs
        file.clear();
        file.open(storage_file, std::ios::out | std::ios::trunc);
        if (file.bad() || file.fail()) {
            throw std::runtime_error("Storage_manager: error opening file");
        }
        file.close();
        // Open it again with correct flags
        file.open(storage_file, std::ios::in | std::ios::out | std::ios::binary);
        if (file.bad() || file.fail()) {
            throw std::runtime_error("Storage_manager: error opening file");
        }
    }

    file.seekg(0);
    file.read((char*)&n_entries, sizeof(n_entries));
    if (file.eof()) {
        file.clear();
        n_entries = 0;

        file.seekp(0);
        file.write((char*)&n_entries, sizeof(n_entries));
        file.flush();
    }
}

Archivist::~Archivist() {
    file.flush();
    file.close();
}

Archivist& Archivist::get_default()
{
    static Archivist storage_manager(DEFAULT_STORAGE_FILE);
    return storage_manager;
}

std::optional<unsigned> Archivist::next_entry(unsigned prev_pos)
{
    file.seekg(prev_pos);
    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }

    unsigned short locator_size;
    unsigned int data_size;

    file.read((char*)&locator_size, sizeof(locator_size));
    file.read((char*)&data_size, sizeof(data_size));

    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }

    file.seekg(locator_size + data_size, std::ios::cur);

    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }

    return {(unsigned)file.tellg()};
}

std::optional<unsigned> Archivist::locate_idx(int index)
{
    if (index >= n_entries) {
        return {};
    }
    file.seekg(4);

    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }
    unsigned int cur_pos = (unsigned)file.tellg();

    for (int i = 0; i < index; i++) {
        std::optional<unsigned> next_pos = next_entry(cur_pos);
        if (!next_pos.has_value()) {
            return {};
        }
        cur_pos = next_pos.value();
    }

    return {(unsigned)file.tellg()};
}

std::optional<unsigned> Archivist::locate_entry(std::string locator)
{
    file.seekg(4);
    
    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }
    unsigned short locator_size;
    unsigned int data_size;

    while (true) {

        file.read((char*)&locator_size, sizeof(locator_size));
        file.read((char*)&data_size, sizeof(data_size));

        if (file.fail() || file.eof()) {
            file.clear();
            return {};
        }
        std::string entry_locator;
        entry_locator.resize(locator_size);

        file.read(entry_locator.data(), locator_size);
        if (file.fail() || file.eof()) {
            file.clear();
            return {};
        }
        if (entry_locator == locator) {
            return (unsigned)file.tellg() - sizeof(locator_size) - sizeof(data_size) - locator_size;
        }
        file.seekg((unsigned)file.tellg() + data_size);
    }
}

std::optional<Serialized> Archivist::read_entry_at(unsigned at_pos)
{
    Serialized data;

    file.seekg(at_pos);
    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }
    unsigned short locator_size;
    unsigned int data_size;

    file.read((char*)&locator_size, sizeof(locator_size));
    file.read((char*)&data_size, sizeof(data_size));
    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }
    data.resize(data_size);

    file.seekg(locator_size, std::ios::cur);
    file.read(data.data(), data_size);

    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }

    return {data};
}

bool Archivist::write_entry_at(unsigned at_pos, Archivist::Storage_entry& entry)
{
    file.seekp(at_pos);
    if (file.fail() || file.eof()) {
        file.clear();
        return false;
    }
    unsigned short locator_size = entry.locator.size();
    unsigned int data_size = entry.data.size();

    file.write((char*)&locator_size, sizeof(locator_size));
    file.write((char*)&data_size, sizeof(data_size));
    file.write(entry.locator.data(), locator_size);
    file.write(entry.data.data(), data_size);

    file.flush();
    return !(file.fail() || file.eof());
}

std::optional<Serialized> Archivist::get_raw(std::string locator)
{
    std::optional<unsigned int> location = locate_entry(locator);
    if (!location.has_value()) {
        return {};
    }
    Serialized data = read_entry_at(location.value()).value();

    return data;
}

bool Archivist::put_raw(std::string locator, std::vector<char> value)
{
    Storage_entry entry;
    entry.locator = locator;
    entry.data = value;

    file.seekp(0, std::ios::end);
    std::optional<unsigned> write_position = locate_entry(locator);

    // key already exists
    if(write_position.has_value()) {
        // check data size
        unsigned int data_size;
        file.seekg(sizeof(unsigned short) + write_position.value());
        file.read((char*)&data_size, sizeof(unsigned int));
        // if same, write new data over
        if (data_size == value.size()) {
            return write_entry_at(write_position.value(), entry);
        }
        else { // if different, delete entry and add it again as if it was new
            if(!del(locator)) {
                return false;
            }
            n_entries++; // entry was not deleted -- increment counter
            file.seekp(0);
            file.write((char*)&n_entries, sizeof(n_entries));
            if (file.fail() || file.eof()) {
                file.clear();
                return false;
            }
            file.flush();
        }
    }
    // key does not exist or was erased
    file.seekg(0, std::ios::end);
    if(file.fail()) {
            file.clear();
            return 0;
        }

    if (!write_entry_at((unsigned)file.tellg(), entry)) {
        return false;
    }
    // update entry count
    n_entries += 1;
    file.seekp(0);
    file.write((char*)&n_entries, sizeof(n_entries));
    if (file.fail() || file.eof()) {
        file.clear();
        return false;
    }
    file.flush();

    return true;
}

bool Archivist::del(std::string locator)
{
    std::optional<unsigned> entry_loc = locate_entry(locator);
    if (!entry_loc.has_value()) {
        return false;
    }
    std::vector<char> buffer;
    unsigned new_file_size = 0;

    std::optional<unsigned> next_entry_start = next_entry(entry_loc.value());
    // deleting last entry
    if (!next_entry_start.has_value()) {
        new_file_size = entry_loc.value();
    }
    else {
        // move all entries back
        unsigned entry_size = next_entry_start.value() - entry_loc.value();
        std::optional<unsigned> next_entry_end = next_entry(next_entry_start.value());
        while (next_entry_end.has_value()) {
            unsigned moved_entry_size = next_entry_end.value() - next_entry_start.value();
            buffer.resize(moved_entry_size);
            // move next 
            file.seekg(next_entry_start.value());
            file.read(buffer.data(), moved_entry_size);
            if (file.fail()) {
                file.clear();
                return false;
            }
            file.seekp(entry_loc.value());
            file.write(buffer.data(), moved_entry_size);
            if (file.fail()) {
                file.clear();
                return false;
            }
            entry_loc = entry_loc.value() + moved_entry_size;
            next_entry_start = next_entry_end;
            next_entry_end = next_entry(next_entry_start.value());
        }
        new_file_size = entry_loc.value();
    }
    // resize file, then reopen it
    file.close();
    std::filesystem::resize_file(filename, new_file_size);
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    // update entries counter
    n_entries--;
    file.seekp(0);
    file.write((char*)&n_entries, sizeof(n_entries));
    if (file.fail()) {
        file.clear();
        return false;
    }
    file.flush();
    return true;
}