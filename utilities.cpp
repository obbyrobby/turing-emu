#include "utilities.h"

bool Utilities::IsInteger(const std::string& s) {
    return std::all_of(std::begin(s), std::end(s),
                       [](unsigned char c) {
                           return std::isdigit(c);
                       });
}

void Utilities::Ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
}

void Utilities::Rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), s.end());
}

void Utilities::Trim(std::string &s) {
    Utilities::Ltrim(s);
    Utilities::Rtrim(s);
}
