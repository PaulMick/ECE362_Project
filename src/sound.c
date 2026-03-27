#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hardware/pwm.h"
#include "pico/stdlib.h"

#define WAVE_SAMPLES 1000

uint16_t wavetable[WAVE_SAMPLES];

void init_wavetable() {
    for (int i = 0; i < WAVE_SAMPLES; i ++) {
        wavetable[i] = (1 << 15) * sin(i / (WAVE_SAMPLES * 4)) + (1 << 15);
    }
}