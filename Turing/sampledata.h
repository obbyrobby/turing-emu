#ifndef SAMPLEDATA_H
#define SAMPLEDATA_H

#include "machine.h"

class SampleData
{
public:
    static Machine::Program GetSampleProgram();
    static std::vector<char> GetSampleTape();
};

#endif // SAMPLEDATA_H
