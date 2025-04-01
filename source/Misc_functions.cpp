#include <sztronics/miscellaneous/Misc_functions.hpp>

/// @brief Split text into multiple lines with an optional upper bound on line width
/// @param text The string to split
/// @param line_width Maximum allowed line width. Set to 0 or negative to disable (handle only newlines)
/// @return Each new line as vector element
std::vector<std::string> wrap_text(const std::string& text, int line_width)
{
    std::vector<std::string> lines = {""};
    std::string word = "";
    // scan all words and move them to next lines if they don't fit
    for (char ch : text)
    {
        if (ch == ' ' || ch == '\n') { // Word end detected -- determine if it should be divided
            if (lines.back().size() + word.size() > line_width && line_width > 0) {
                while (word.size() > line_width) // If word is too big, handle its parts
                {   
                    if (lines.back().size() == 0) { // Last line has empty string -- append it without adding new one
                        lines.back() += {word.begin(), word.begin()+line_width};
                    }
                    else {  
                        lines.push_back({word.begin(), word.begin()+line_width});
                    }
                    word = {word.begin()+line_width, word.end()}; // Cut part off that has been put into lines[]
                } 
                lines.push_back(word); // When it is less than max width, add it as new line.
            }
            else { // Can fit -- append last line with word
                lines.back() += word;
            }

            if (ch == ' ' && (lines.back().size() < line_width || line_width <= 0)) {
                lines.back() += ' ';
            }
            else if (ch == '\n') { // Force end line
                lines.push_back("");
            }
            word = ""; // Clear word buffer
        }
        else {
            word += ch; // If regular character, store it in word buffer until the word ends
        }
    }
    // handle the last word
    if (lines.back().size() + word.size() > line_width && line_width > 0)
    {
        while (word.size() > line_width) // Exactly as before
        {
            if (lines.back().size() == 0) {
                lines.back() += {word.begin(), word.begin()+line_width};
            }
            else {
            lines.push_back({word.begin(), word.begin()+line_width});
            }
            word = {word.begin()+line_width, word.end()};
        }
        lines.push_back(word);
    }
    else
    {
        lines.back() += word;
    }
    // Trim whitespaces at line ends
    for (std::string& s : lines) 
    {
        int new_size = s.size();
        while(new_size > 1 && s[new_size-1] == ' ') {
            new_size--;
        }
        s.resize(new_size);
    }
    // Trim trailing empty lines

    // int new_line_count = lines.size();
    // while(new_line_count > 1 && lines[new_line_count-1].size() == 0) {
    //     new_line_count--;
    // }
    // lines.resize(new_line_count);

    return lines;
}