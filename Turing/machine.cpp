#include "machine.h"

Machine::Machine() {
    this->Reset();
}

Machine::Machine(Machine::Program functionalMatrix, std::vector<char> tape) {
    this->functionalMatrix = functionalMatrix;
    this->initial_tape = tape;

    this->Reset();
}

void Machine::ExecuteSingle() {
    if(this->flag == Flag::NOT_RUNNING) this->flag = Flag::RUNNING;

    if(this->flag != Flag::RUNNING) return; // Machine cannot continue executing in this state
    char curr_char = GetCurrentChar();
    auto curr_inst_opt = this->functionalMatrix[curr_char][this->currentState];

    if(curr_inst_opt.has_value()) {
        Instruction curr_inst = curr_inst_opt.value();

        tape[currentCell] = curr_inst.new_char;

        // Wrap around!
        switch(curr_inst.direction) {
        case LEFT:
            this->currentCell--;
            if(currentCell < 0) {
                currentCell = Machine::TAPE_LEN - 1;
            }
            break;
        case RIGHT:
            this->currentCell++;
            if(currentCell >= Machine::TAPE_LEN) {
                currentCell = 0;
            }
            break;
        case STILL:
            // Nothing
            break;
        }

        this->currentState = curr_inst.new_state;
        if(this->currentState == -1) {
            this->flag = Flag::HALTED;
        }
    } else {
        // Invalid instruction! Halt the machine
        this->flag = Flag::INVALID_INSTR;
    }

}

void Machine::Execute() {
    int curr_num_inst = 0;
    while(this->currentState != -1 && curr_num_inst < Machine::MAX_INSTR) {
        this->ExecuteSingle();
        curr_num_inst++;
    }

    if(curr_num_inst >= Machine::MAX_INSTR) {
        this->flag = Flag::TOOMANYINSTR;
    }
}

char Machine::GetCurrentChar() {
    return tape[currentCell];
}

void Machine::Reset() {
    this->currentState = 0;
    this->currentCell = Machine::TAPE_LEN - 1;
    this->tape.clear();
    this->flag = Flag::NOT_RUNNING;
    std::copy(this->initial_tape.begin(), this->initial_tape.end(), back_inserter(this->tape));
}

const std::vector<char>& Machine::getTape() const {
    return this->tape;
}

void Machine::setInitialTape(std::vector<char>& initial_tape) {
    this->initial_tape.clear();
    std::copy(initial_tape.begin(), initial_tape.end(), back_inserter(this->initial_tape));
    this->Reset();
}

void Machine::setInstr_fromString(std::string str, int row, int col_state) {
    char row_char = this->GetTapeSymbols()[row];
    this->Machine::setInstr_fromString(str, row_char, col_state);
}

void Machine::setInstr_fromString(std::string str, char row_char, int col_state) {
    if(str.empty()) {
        functionalMatrix[row_char][col_state] = std::nullopt;
        return;
    }

    if(str.length() < 4) {
        throw std::runtime_error(str + " is not a valid instruction.");
    }

    char new_char = str[0];

    Direction new_direction;
    char char_newdirection = str[1];

    std::string new_state_str = str.substr(2);
    Utilities::Rtrim(new_state_str);
    int new_state;

    auto tape_symbols = this->GetTapeSymbols();
    if (!(std::find(tape_symbols.begin(), tape_symbols.end(), new_char) != tape_symbols.end())) {
        throw std::runtime_error(std::to_string(new_char) + " is not a valid symbol for this machine.");
    }

    switch (char_newdirection)
    {
        case 'S':
            new_direction = Direction::LEFT;
            break;
        case 'D':
            new_direction = Direction::RIGHT;
            break;
        case 'F':
            new_direction = Direction::STILL;
            break;
        default:
            throw std::runtime_error(std::to_string(char_newdirection) + " is not a valid direction value.");
            break;
    }

    if(new_state_str == " HALT") {
        new_state = -1;
    } else if(new_state_str[0] == 'q') {
        new_state_str = new_state_str.substr(1);
        if(!Utilities::IsInteger(new_state_str)) {
            throw std::runtime_error(new_state_str + " is an invalid state index");
        }
        new_state = std::stoi(new_state_str);
    } else {
        throw std::runtime_error(new_state_str + " is an invalid state.");
    }

    Instruction instr(new_char, new_direction, new_state);
    functionalMatrix[row_char][col_state] = instr;
}

void Machine::changeTapeValue(char c, int index) {
    if(index >= this->initial_tape.size()) {
        throw std::runtime_error("Attempted to write to an out of range slot on the tape. This should never happen!");
    }
    this->initial_tape[index] = c;
}

const std::vector<char> Machine::GetTapeSymbols() const {
    std::vector<char> symbols;
    for (auto it = functionalMatrix.begin(); it != functionalMatrix.end(); ++it)
    {
        // Add the key to the vector
        symbols.push_back(it->first);
    }
    return symbols;
}
const int Machine::GetTapeSymbolsAmt() const {
    return this->functionalMatrix.size();
}

const Flag Machine::GetFlag() const {
    return flag;
}
