#pragma once

#include <optional>
#include <fstream>
#include <string>
#include <iostream>

class Logger
{
    public:
    static Logger& get();

    void enable(const std::string& filename);
    void disable();

    template<typename T>
    Logger& operator<<(const T& message) {
        if (logfile.has_value()) {
            logfile.value() << message;
        }
        return *this;
    }

    void flush();

    private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    ~Logger();

    std::optional<std::ofstream> logfile;
};