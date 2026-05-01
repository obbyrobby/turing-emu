#include "sampledata.h"

Machine::Program SampleData::GetSampleProgram() {

    Machine::Program functionalMatrix;
    functionalMatrix['^'][0] = Instruction('^', LEFT, 0);
    functionalMatrix['^'][1] = Instruction('^', RIGHT, 2);
    functionalMatrix['^'][3] = Instruction('^', LEFT, 0);

    functionalMatrix['|'][0] = Instruction('^', LEFT, 1);
    functionalMatrix['|'][1] = Instruction('|', LEFT, 1);
    functionalMatrix['|'][2] = Instruction('^', RIGHT, 3);
    functionalMatrix['|'][3] = Instruction('|', RIGHT, 3);

    functionalMatrix['*'][0] = Instruction('^', STILL, -1);
    functionalMatrix['*'][1] = Instruction('*', LEFT, 1);
    functionalMatrix['*'][3] = Instruction('*', RIGHT, 3);

    return functionalMatrix;
}

std::vector<char> SampleData::GetSampleTape() {
    std::vector<char> initial_tape = { '^', '^', '|', '|', '|', '*', '|', '^', '^', '^'};
    return initial_tape;
}
