#ifndef MACHINE_H
#define MACHINE_H

#include "instruction.h"
#include "Enums/flag.h"
#include "../utilities.h"
#include <map>
#include <array>
#include <optional>
#include <vector>
#include <algorithm>

class Machine
{
    private:
        std::vector<char> tape;
        std::vector<char> initial_tape;
        Flag flag;
    public:
        constexpr static int MAX_SYMBOLS = 64;
        constexpr static int MAX_STATES = 64;

        constexpr static int TAPE_LEN = 10;
        constexpr static int MAX_INSTR = 1000;
        constexpr static int NUM_STATES = 4;

        using Program = std::map<char, std::array<std::optional<Instruction>, NUM_STATES>>;
        Program functionalMatrix;

        Machine();
        Machine(Machine::Program functionalMatrix, std::vector<char> tape);

        void ExecuteSingle();
        void Execute();
        void Reset();
        char GetCurrentChar();

        const std::vector<char>& getTape() const;
        const Flag GetFlag() const;
        const std::vector<char> GetTapeSymbols() const;
        const int GetTapeSymbolsAmt() const;
        void setInitialTape(std::vector<char>& initial_tape);
        void setInstr_fromString(std::string str, int row, int col_state);
        void setInstr_fromString(std::string str, char row_char, int col_state);
        void changeTapeValue(char c, int index);

        int currentState;
        int currentCell;
};

#endif // MACHINE_H
