#include "turingfile.h"

Machine TuringFile::Load(const std::string& filepath) {
    std::ifstream in(filepath, std::ios::binary);

    if(!in.is_open()) {
        throw std::runtime_error("Could not open the file.");
    }

    uint32_t file_magicnumber;
    in.read(reinterpret_cast<char*>(&file_magicnumber), sizeof(file_magicnumber));
    if(file_magicnumber != magic_constant) {
        throw std::runtime_error("Opened file is not a valid TuringEmu machine file");
    }

    int amt_symbols;
    in.read(reinterpret_cast<char*>(&amt_symbols), sizeof(amt_symbols));
    if(amt_symbols > Machine::MAX_SYMBOLS) {
        throw std::runtime_error("Too many symbols.");
    }

    int num_states;
    in.read(reinterpret_cast<char*>(&num_states), sizeof(num_states));
    if(num_states > Machine::MAX_STATES) {
        throw std::runtime_error("Too many states.");
    }

    Machine::Program functionalMatrix;
    for(int char_index = 0; char_index < amt_symbols; char_index++) {
        char symbol;

        in.read(reinterpret_cast<char*>(&symbol), sizeof(symbol));
        if(symbol < 33 || symbol > 126) {
            throw std::runtime_error("The file is corrupt. An entry in the functional matrix points to an invalid character.");
        }

        for(int state_index = 0; state_index < num_states; state_index++) {
            char new_symbol;
            Direction direction;
            uint8_t direction_val;
            int new_state = 0;

            in.read(reinterpret_cast<char*>(&new_symbol), sizeof(new_symbol));
            in.read(reinterpret_cast<char*>(&direction_val), sizeof(direction_val));
            in.read(reinterpret_cast<char*>(&new_state), sizeof(new_state));

            if(new_symbol == 0) {
                // Unset instruction
                functionalMatrix[symbol][state_index] = std::nullopt;
                //std::cout << "Debug: " << symbol << " Unset instruction" << std::endl;
                continue;
            }

            if(new_state < -1 || new_state > 64) {
                std::stringstream error_stream;
                error_stream << "The file is corrupt. The instruction for the character " << symbol << ", for the state ";
                error_stream << state_index << ", attempts to set an invalid state q" << new_state << ".";
                throw std::runtime_error(error_stream.str());
            }

            direction = (Direction)direction_val;
            Instruction i(new_symbol, direction, new_state);
            functionalMatrix[symbol][state_index] = i;

            //std::cout << "Debug: " << symbol << ": " << i.to_string() << std::endl;
        }
    }

    uint32_t tapeSize;
    in.read(reinterpret_cast<char*>(&tapeSize), sizeof(tapeSize));

    std::vector<char> tape(tapeSize);
    in.read(reinterpret_cast<char*>(tape.data()), tapeSize);

    in.close();
    return Machine(std::move(functionalMatrix), std::move(tape));
}

void TuringFile::Save(const Machine& machine, const std::string& filepath) {
    std::ofstream out(filepath, std::ios::binary | std::ios::trunc);
    if(!out.is_open()) {
        throw std::runtime_error("Could not write to the file");
    }

    // Write the Magic Number
    out.write(reinterpret_cast<const char*>(&TuringFile::magic_constant), sizeof(TuringFile::magic_constant));

    // Write the amount of symbols
    int amtSymbols = machine.GetTapeSymbolsAmt();
    out.write(reinterpret_cast<const char*>(&amtSymbols), sizeof(amtSymbols));

    // Write the amount of states
    int numStates = machine.NUM_STATES;
    out.write(reinterpret_cast<const char*>(&numStates), sizeof(numStates));

    // For each symbol available, write the symbol then { instruction } * numStates
    for (const auto& [symbol, instrArray] : machine.functionalMatrix) {
        out.write(reinterpret_cast<const char*>(&symbol), sizeof(symbol));
        for(int state = 0; state < Machine::NUM_STATES; state++) {
            const auto& opt_instr = instrArray[state];

            char new_char = 0;
            Direction direction = STILL;
            int new_state = -1;

            if(opt_instr.has_value()) {
                const Instruction& instr = opt_instr.value();
                new_char = instr.new_char;
                direction = instr.direction;
                new_state = instr.new_state;
            }

            // Casting to a uint8_t to ensure size is fixed
            uint8_t direction_val = static_cast<uint8_t>(direction);

            out.write(reinterpret_cast<const char*>(&new_char), sizeof(new_char));
            out.write(reinterpret_cast<const char*>(&direction_val), sizeof(direction_val));
            out.write(reinterpret_cast<const char*>(&new_state), sizeof(new_state));
        }
    }


    const auto& tape = machine.getTape();
    uint32_t tapeSize = tape.size();
    out.write(reinterpret_cast<const char*>(&tapeSize), sizeof(tapeSize));
    out.write(tape.data(), tapeSize);

    out.close();
}


