#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hardware/pwm.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "sound.h"
#include "gen_utils.h"

// number of wave samples
#define WAVE_SAMPLES 256

// speaker pin numbers
#define SPEAKER0 30
#define SPEAKER1 32
#define SPEAKER2 34
#define SPEAKER3 36
#define PWM_CHAN_SPEAKER0 7
#define PWM_CHAN_SPEAKER1 8
#define PWM_CHAN_SPEAKER2 9
#define PWM_CHAN_SPEAKER3 10
#define DMA_CHAN_SPEAKER0 0
#define DMA_CHAN_SPEAKER1 1
#define DMA_CHAN_SPEAKER2 2
#define DMA_CHAN_SPEAKER3 3

uint8_t wavetable[WAVE_SAMPLES];

chord_t *current_sound;
uint32_t sound_counter = 0;

// initialize wavetable, normalized to 0-255 range
void init_wavetable() {
    for (int i = 0; i < WAVE_SAMPLES; i ++) {
        wavetable[i] = (uint8_t) (25 * (sin((((float) i) / ((float) WAVE_SAMPLES)) * 2 * 3.14159265)) + 25);
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
    pwm_set_clkdiv(PWM_CHAN_SPEAKER0, 5.859375f);
    pwm_set_clkdiv(PWM_CHAN_SPEAKER1, 5.859375f);
    pwm_set_clkdiv(PWM_CHAN_SPEAKER2, 5.859375f);
    pwm_set_clkdiv(PWM_CHAN_SPEAKER3, 5.859375f);
    pwm_set_wrap(PWM_CHAN_SPEAKER0, 50);
    pwm_set_wrap(PWM_CHAN_SPEAKER1, 50);
    pwm_set_wrap(PWM_CHAN_SPEAKER2, 50);
    pwm_set_wrap(PWM_CHAN_SPEAKER3, 50);
    pwm_set_chan_level(PWM_CHAN_SPEAKER0, PWM_CHAN_A, 0);
    pwm_set_chan_level(PWM_CHAN_SPEAKER1, PWM_CHAN_A, 0);
    pwm_set_chan_level(PWM_CHAN_SPEAKER2, PWM_CHAN_A, 0);
    pwm_set_chan_level(PWM_CHAN_SPEAKER3, PWM_CHAN_A, 0);
    pwm_set_enabled(PWM_CHAN_SPEAKER0, true);
    pwm_set_enabled(PWM_CHAN_SPEAKER1, true);
    pwm_set_enabled(PWM_CHAN_SPEAKER2, true);
    pwm_set_enabled(PWM_CHAN_SPEAKER3, true);

    // initialize dma
    dma_channel_config_t cfg_speaker0 = dma_channel_get_default_config(DMA_CHAN_SPEAKER0);
    dma_channel_config_t cfg_speaker1 = dma_channel_get_default_config(DMA_CHAN_SPEAKER1);
    dma_channel_config_t cfg_speaker2 = dma_channel_get_default_config(DMA_CHAN_SPEAKER2);
    dma_channel_config_t cfg_speaker3 = dma_channel_get_default_config(DMA_CHAN_SPEAKER3);
    channel_config_set_transfer_data_size(&cfg_speaker0, DMA_SIZE_8);
    channel_config_set_transfer_data_size(&cfg_speaker1, DMA_SIZE_8);
    channel_config_set_transfer_data_size(&cfg_speaker2, DMA_SIZE_8);
    channel_config_set_transfer_data_size(&cfg_speaker3, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg_speaker0, true);
    channel_config_set_read_increment(&cfg_speaker1, true);
    channel_config_set_read_increment(&cfg_speaker2, true);
    channel_config_set_read_increment(&cfg_speaker3, true);
    channel_config_set_write_increment(&cfg_speaker0, false);
    channel_config_set_write_increment(&cfg_speaker1, false);
    channel_config_set_write_increment(&cfg_speaker2, false);
    channel_config_set_write_increment(&cfg_speaker3, false);
    channel_config_set_ring(&cfg_speaker0, false, 8);
    channel_config_set_ring(&cfg_speaker1, false, 8);
    channel_config_set_ring(&cfg_speaker2, false, 8);
    channel_config_set_ring(&cfg_speaker3, false, 8);
    channel_config_set_dreq(&cfg_speaker0, pwm_get_dreq(PWM_CHAN_SPEAKER0));
    channel_config_set_dreq(&cfg_speaker1, pwm_get_dreq(PWM_CHAN_SPEAKER1));
    channel_config_set_dreq(&cfg_speaker2, pwm_get_dreq(PWM_CHAN_SPEAKER2));
    channel_config_set_dreq(&cfg_speaker3, pwm_get_dreq(PWM_CHAN_SPEAKER3));
    dma_channel_configure(DMA_CHAN_SPEAKER0, &cfg_speaker0, &(pwm_hw->slice[PWM_CHAN_SPEAKER0].cc), wavetable, WAVE_SAMPLES | (DMA_CH0_TRANS_COUNT_MODE_VALUE_TRIGGER_SELF << DMA_CH0_TRANS_COUNT_MODE_LSB), true);
    dma_channel_configure(DMA_CHAN_SPEAKER1, &cfg_speaker1, &(pwm_hw->slice[PWM_CHAN_SPEAKER1].cc), wavetable, WAVE_SAMPLES | (DMA_CH1_TRANS_COUNT_MODE_VALUE_TRIGGER_SELF << DMA_CH1_TRANS_COUNT_MODE_LSB), true);
    dma_channel_configure(DMA_CHAN_SPEAKER2, &cfg_speaker2, &(pwm_hw->slice[PWM_CHAN_SPEAKER2].cc), wavetable, WAVE_SAMPLES | (DMA_CH2_TRANS_COUNT_MODE_VALUE_TRIGGER_SELF << DMA_CH2_TRANS_COUNT_MODE_LSB), true);
    dma_channel_configure(DMA_CHAN_SPEAKER3, &cfg_speaker3, &(pwm_hw->slice[PWM_CHAN_SPEAKER3].cc), wavetable, WAVE_SAMPLES | (DMA_CH3_TRANS_COUNT_MODE_VALUE_TRIGGER_SELF << DMA_CH3_TRANS_COUNT_MODE_LSB), true);

    // timer
    hw_set_bits(&timer0_hw->inte, 1 << 0);
    irq_set_exclusive_handler(timer_hardware_alarm_get_irq_num(timer0_hw, 0), sound_isr);
    irq_set_enabled(timer_hardware_alarm_get_irq_num(timer0_hw, 0), true);
}

void set_dividers(float div0, float div1, float div2, float div3) {
    if (div0 > 255.9375f) {
        div0 = 255.9375f;
    }
    if (div1 > 255.9375f) {
        div1 = 255.9375f;
    }
    if (div2 > 255.9375f) {
        div2 = 255.9375f;
    }
    if (div3 > 255.9375f) {
        div3 = 255.9375f;
    }
    pwm_set_clkdiv(PWM_CHAN_SPEAKER0, div0);
    pwm_set_clkdiv(PWM_CHAN_SPEAKER1, div1);
    pwm_set_clkdiv(PWM_CHAN_SPEAKER2, div2);
    pwm_set_clkdiv(PWM_CHAN_SPEAKER3, div3);
}

void set_freqs(float freq0, float freq1, float freq2, float freq3) {
    if (freq0 <= 0) {
        pwm_set_enabled(PWM_CHAN_SPEAKER0, false);
    } else {
        pwm_set_enabled(PWM_CHAN_SPEAKER0, true);
    }
    if (freq1 <= 0) {
        pwm_set_enabled(PWM_CHAN_SPEAKER1, false);
    } else {
        pwm_set_enabled(PWM_CHAN_SPEAKER1, true);
    }
    if (freq2 <= 0) {
        pwm_set_enabled(PWM_CHAN_SPEAKER2, false);
    } else {
        pwm_set_enabled(PWM_CHAN_SPEAKER2, true);
    }
    if (freq3 <= 0) {
        pwm_set_enabled(PWM_CHAN_SPEAKER3, false);
    } else {
        pwm_set_enabled(PWM_CHAN_SPEAKER3, true);
    }
    if (freq0 > FREQ_MAX) {
        freq0 = FREQ_MAX;
    }
    if (freq1 > FREQ_MAX) {
        freq1 = FREQ_MAX;
    }
    if (freq2 > FREQ_MAX) {
        freq2 = FREQ_MAX;
    }
    if (freq3 > FREQ_MAX) {
        freq3 = FREQ_MAX;
    }
    set_dividers(FREQ_MAX / freq0, FREQ_MAX / freq1, FREQ_MAX / freq2, FREQ_MAX / freq3);
}

void set_notes(note_t note0, note_t note1, note_t note2, note_t note3) {
    set_freqs(((float) note0) / NOTE_FACTOR, ((float) note1) / NOTE_FACTOR, ((float) note2) / NOTE_FACTOR, ((float) note3) / NOTE_FACTOR);
}

void play_sound(chord_t sound[]) {
    current_sound = sound;
    sound_counter = 0;
    uint64_t sound_target = timer0_hw->timerawl + 10;
    timer_hardware_alarm_set_target(timer0_hw, 0, sound_target);
}

void sound_isr() {
    hw_set_bits(&timer0_hw->intr, 1 << 0);
    chord_t current_chord = current_sound[sound_counter];
    if (current_chord.duration == END) {
        set_notes(REST, REST, REST, REST);
        return;
    }
    set_notes(current_chord.note0, current_chord.note1, current_chord.note2, current_chord.note3);
    uint64_t sound_target = timer0_hw->timerawl + current_chord.duration * US_64TH_NOTE;
    timer_hardware_alarm_set_target(timer0_hw, 0, sound_target);
    sound_counter ++;
}