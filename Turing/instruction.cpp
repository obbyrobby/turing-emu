#include "instruction.h"

Instruction::Instruction(char new_char, Direction direction, int new_state)
{
    this->new_char = new_char;
    this->direction = direction;
    this->new_state = new_state;
}

std::string Instruction::to_string() const {
    std::string inst_text;
    inst_text += this->new_char;
    switch(this->direction) {
        case LEFT:
            inst_text += 'S';
            break;
        case RIGHT:
            inst_text += 'D';
            break;
        case STILL:
            inst_text += 'F';
            break;
    }

    if(this->new_state == -1) {
        inst_text += " HALT";
    } else {
        inst_text += "q" + std::to_string(new_state);
    }
    return inst_text;
}
