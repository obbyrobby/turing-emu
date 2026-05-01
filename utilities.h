#ifndef UTILITIES_H
#define UTILITIES_H

#include <algorithm>
#include <iostream>

class Utilities
{
    public:
        static bool IsInteger(const std::string& s);
        static void Ltrim(std::string &s);
        static void Rtrim (std::string &s);
        static void Trim(std::string &s);
};

#endif // UTILITIES_H
