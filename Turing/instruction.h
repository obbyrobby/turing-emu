#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Enums/direction.h"
#include <iostream>

class Instruction
{
    public:
        Instruction(char new_char, Direction direction, int new_state);
        std::string to_string() const;

        char new_char;
        Direction direction;
        int new_state;
};

#endif // INSTRUCTION_H
