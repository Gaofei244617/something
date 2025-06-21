#include <string>
#include <vector>
#include <iostream>
#include <sstream>

std::vector<std::string> split(const std::string &text, char delim)
{
    std::vector<std::string> tokens;
    std::stringstream ss(text);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (!item.empty())
        {
            tokens.push_back(item);
        }
    }
    return tokens;
}