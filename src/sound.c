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

chord_t *current_sound_a;
chord_t *current_sound_b;
uint32_t sound_counter_a = 0;
uint32_t sound_counter_b = 0;

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
    irq_set_exclusive_handler(timer_hardware_alarm_get_irq_num(timer0_hw, 0), sound_isr_a);
    irq_set_enabled(timer_hardware_alarm_get_irq_num(timer0_hw, 0), true);
    hw_set_bits(&timer0_hw->inte, 1 << 1);
    irq_set_exclusive_handler(timer_hardware_alarm_get_irq_num(timer0_hw, 1), sound_cutoff_isr_a);
    irq_set_enabled(timer_hardware_alarm_get_irq_num(timer0_hw, 1), true);
    hw_set_bits(&timer0_hw->inte, 1 << 2);
    irq_set_exclusive_handler(timer_hardware_alarm_get_irq_num(timer0_hw, 2), sound_isr_b);
    irq_set_enabled(timer_hardware_alarm_get_irq_num(timer0_hw, 2), true);
    hw_set_bits(&timer0_hw->inte, 1 << 3);
    irq_set_exclusive_handler(timer_hardware_alarm_get_irq_num(timer0_hw, 3), sound_cutoff_isr_b);
    irq_set_enabled(timer_hardware_alarm_get_irq_num(timer0_hw, 3), true);
    
}

void set_divider(float div, uint8_t index) {
    if (div > 255.9375f) {
        div = 255.9375f;
    }
    pwm_set_clkdiv(PWM_CHAN_SPEAKER0 + index, div);
}

void set_freq(float freq, uint8_t index) {
    if (freq <= 0) {
        pwm_set_enabled(PWM_CHAN_SPEAKER0 + index, false);
    } else {
        pwm_set_enabled(PWM_CHAN_SPEAKER0 + index, true);
    }
    if (freq > FREQ_MAX) {
        freq = FREQ_MAX;
    }
    set_divider(FREQ_MAX / freq, index);
}

void set_note(note_t note, uint8_t index) {
    set_freq(((float) note) / NOTE_FACTOR, index);
}

void play_sound(chord_t sound[], sound_selection_t sel) {
    if (sel == SEL_A) {
        current_sound_a = sound;
        sound_counter_a = 0;
    } else {
        current_sound_b = sound;
        sound_counter_b = 0;
    }
    uint64_t sound_target = timer0_hw->timerawl + 10;
    if (sel == SEL_A) {
        timer_hardware_alarm_set_target(timer0_hw, 0, sound_target);
    } else {
        timer_hardware_alarm_set_target(timer0_hw, 2, sound_target);
    }
}

void sound_isr_a() {
    hw_clear_bits(&timer0_hw->intr, 1 << 0);
    chord_t current_chord = current_sound_a[sound_counter_a];
    if (current_chord.duration == END) {
        set_note(REST, 0);
        set_note(REST, 1);
        set_note(REST, 2);
        return;
    }
    set_note(current_chord.note0, 0);
    set_note(current_chord.note1, 1);
    set_note(current_chord.note2, 2);
    uint64_t sound_target = timer0_hw->timerawl + current_chord.duration * US_64TH_NOTE;
    uint64_t cutoff_target = timer0_hw->timerawl + current_chord.duration * US_64TH_NOTE - US_CUTOFF;
    timer_hardware_alarm_set_target(timer0_hw, 0, sound_target);
    timer_hardware_alarm_set_target(timer0_hw, 1, cutoff_target);
    sound_counter_a ++;
}

void sound_isr_b() {
    hw_clear_bits(&timer0_hw->intr, 1 << 2);
    chord_t current_chord = current_sound_b[sound_counter_b];
    if (current_chord.duration == END) {
        set_note(REST, 3);
        return;
    }
    set_note(current_chord.note3, 3);
    uint64_t sound_target = timer0_hw->timerawl + current_chord.duration * US_64TH_NOTE;
    uint64_t cutoff_target = timer0_hw->timerawl + current_chord.duration * US_64TH_NOTE - US_CUTOFF;
    timer_hardware_alarm_set_target(timer0_hw, 2, sound_target);
    timer_hardware_alarm_set_target(timer0_hw, 3, cutoff_target);
    sound_counter_b ++;
}

void sound_cutoff_isr_a() {
    printf("a\n");
    hw_clear_bits(&timer0_hw->intr, 1 << 1);
    set_note(REST, 0);
    set_note(REST, 1);
    set_note(REST, 2);
}

void sound_cutoff_isr_b() {
    printf("b\n");
    hw_clear_bits(&timer0_hw->intr, 1 << 3);
    set_note(REST, 3);
}