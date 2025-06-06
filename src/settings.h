#ifndef SETTINGS_H
#define SETTINGS_H

#include <cstdint>

#define NUM_INPUTS 10
#define NUM_OUTPUTS 4

enum inputs {
    INPUT_MIC1,
    INPUT_MIC2,
    INPUT_MIC3,
    INPUT_MIC4,
    INPUT_DAW1,
    INPUT_DAW2,
    INPUT_DAW3,
    INPUT_DAW4,
    INPUT_LOOPBACK1,
    INPUT_LOOPBACK2
};

extern const char *input_names[NUM_INPUTS];

enum outputs {
    OUTPUT1,
    OUTPUT2,
    OUTPUT3,
    OUTPUT4
};

extern const char *output_names[NUM_OUTPUTS];

#endif
