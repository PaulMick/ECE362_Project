#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hardware/pwm.h"
#include "hardware/dma.h"
#include "pico/stdlib.h"
#include "sound.h"
#include "gen_utils.h"

// number of wave samples
#define WAVE_SAMPLES 1000

// speaker pin numbers
#define SPEAKER0 30
#define SPEAKER1 31
#define SPEAKER2 32
#define SPEAKER3 33
#define SPEAKER_PWM_CHAN0 7
#define SPEAKER_PWM_CHAN1 8

uint8_t wavetable[WAVE_SAMPLES];

int speaker0_level = 0;
int speaker1_level = 0;
int speaker2_level = 0;
int speaker3_level = 0;

// initialize wavetable, normalized to 0-255 range
void init_wavetable() {
    for (int i = 0; i < WAVE_SAMPLES; i ++) {
        wavetable[i] = (uint8_t) (50 * sin((i / WAVE_SAMPLES) * 2 * 3.14159265) + 50);
    }
}

// initalize whole sound module
void init_sound() {
    // initialize wavetable
    init_wavetable();

    // intialize pwm
    gpio_init(SPEAKER0);
    gpio_init(SPEAKER1);
    gpio_init(SPEAKER2);
    gpio_init(SPEAKER3);
    gpio_set_function(SPEAKER0, GPIO_FUNC_PWM);
    gpio_set_function(SPEAKER1, GPIO_FUNC_PWM);
    gpio_set_function(SPEAKER2, GPIO_FUNC_PWM);
    gpio_set_function(SPEAKER3, GPIO_FUNC_PWM);
    pwm_set_clkdiv(SPEAKER_PWM_CHAN0, 150);
    pwm_set_clkdiv(SPEAKER_PWM_CHAN1, 150);
    pwm_set_wrap(SPEAKER_PWM_CHAN1, 100); // max output freq of 10kHz
    pwm_set_wrap(SPEAKER_PWM_CHAN1, 100);
    pwm_set_chan_level(SPEAKER_PWM_CHAN0, PWM_CHAN_A, 0);
    pwm_set_chan_level(SPEAKER_PWM_CHAN0, PWM_CHAN_B, 0);
    pwm_set_chan_level(SPEAKER_PWM_CHAN1, PWM_CHAN_A, 0);
    pwm_set_chan_level(SPEAKER_PWM_CHAN1, PWM_CHAN_B, 0);
    pwm_set_enabled(SPEAKER_PWM_CHAN0, true);
    pwm_set_enabled(SPEAKER_PWM_CHAN1, true);

    // initialize dma

}

// set level values of all 4 outputs (0-100 incl.)
void set_levels(uint8_t val0, uint8_t val1, uint8_t val2, uint8_t val3) {
    pwm_set_chan_level(SPEAKER_PWM_CHAN0, PWM_CHAN_A, val0);
    pwm_set_chan_level(SPEAKER_PWM_CHAN0, PWM_CHAN_B, val1);
    pwm_set_chan_level(SPEAKER_PWM_CHAN1, PWM_CHAN_A, val2);
    pwm_set_chan_level(SPEAKER_PWM_CHAN1, PWM_CHAN_B, val3);
}

