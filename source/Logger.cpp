#include <sztronics/miscellaneous/Logger.hpp>

Logger& Logger::get()
{
    static Logger instance;
    return instance;
}

Logger::~Logger()
{
    disable();
}

void Logger::enable(const std::string& filename)
{
    if (logfile.has_value()) {
        logfile->close();
    }
    logfile.emplace();
    logfile->open(filename);
    if (!logfile->is_open()) {
        logfile.reset(); // Reset if the file couldn't be opened
    }
}

void Logger::disable()
{
    if (logfile.has_value()) {
        logfile->close();
    }
    logfile.reset();
}

void Logger::flush() {
    if (logfile.has_value()) {
        logfile.value() << std::endl;
    }
}