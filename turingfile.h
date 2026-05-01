#ifndef TURINGFILE_H
#define TURINGFILE_H

#include <cstdint>
#include <optional>
#include "Turing/machine.h"
#include <iostream>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sstream>

class TuringFile
{
    public:
        static Machine Load(const std::string& filepath);
        static void Save(const Machine& machine, const std::string& filepath);
    private:
        static inline const uint32_t magic_constant = 0x5465746F;
};

#endif // TURINGFILE_H
