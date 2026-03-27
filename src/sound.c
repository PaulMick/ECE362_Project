#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hardware/pwm.h"
#include "pico/stdlib.h"

// number of wave samples
#define WAVE_SAMPLES 1000

// speaker pin numbers
#define SPEAKER0 30
#define SPEAKER1 32
#define SPEAKER0_PWM_CHAN 7
#define SPEAKER1_PWM_CHAN 8

uint16_t wavetable[WAVE_SAMPLES];

int speaker0_period = 100;
int speaker1_period = 100;

// initialize wavetable, normalized to 0-2^16 range
void init_wavetable() {
    for (int i = 0; i < WAVE_SAMPLES; i ++) {
        wavetable[i] = (1 << 15) * sin((i / WAVE_SAMPLES) * 2 * 3.14159265) + (1 << 15);
    }
}

// initalize whole sound module
void init_sound() {
    init_wavetable();

    gpio_init(SPEAKER0);
    gpio_init(SPEAKER1);
    gpio_set_function(SPEAKER0, GPIO_FUNC_PWM);
    gpio_set_function(SPEAKER1, GPIO_FUNC_PWM);
    pwm_set_clkdiv(SPEAKER0_PWM_CHAN, 150);
    pwm_set_clkdiv(SPEAKER1_PWM_CHAN, 150);
    pwm_set_wrap(SPEAKER1_PWM_CHAN, speaker0_period);
    pwm_set_wrap(SPEAKER1_PWM_CHAN, speaker1_period);
    pwm_set_chan_level(SPEAKER0_PWM_CHAN, PWM_CHAN_A, 0);
    pwm_set_chan_level(SPEAKER1_PWM_CHAN, PWM_CHAN_A, 0);
    pwm_set_enabled(SPEAKER0_PWM_CHAN, true);
    pwm_set_enabled(SPEAKER1_PWM_CHAN, true);
}