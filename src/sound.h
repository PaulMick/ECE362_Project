#ifndef SOUND_H
#define SOUND_H

#define FREQ_MAX 11718.75f
#define NOTE_FACTOR 16384.0f
#define BPM 180
#define US_64TH_NOTE (int) (1000000.0f / (16.0f * ((float) BPM / 60.0f)))
#define US_CUTOFF (int) 16000.0f / ((float) BPM / (float) 200)

typedef enum {
    REST = 0,
    A2 = (uint32_t) (110.000f * NOTE_FACTOR),
    As2 = (uint32_t) (116.541f * NOTE_FACTOR),
    Bf2 = (uint32_t) (116.541f * NOTE_FACTOR),
    B2 = (uint32_t) (123.471f * NOTE_FACTOR),
    C3 = (uint32_t) (130.813f * NOTE_FACTOR),
    Cs3 = (uint32_t) (138.591f * NOTE_FACTOR),
    Df3 = (uint32_t) (138.591f * NOTE_FACTOR),
    D3 = (uint32_t) (146.832f * NOTE_FACTOR),
    Ds3 = (uint32_t) (155.563f * NOTE_FACTOR),
    Ef3 = (uint32_t) (155.563f * NOTE_FACTOR),
    E3 = (uint32_t) (164.814f * NOTE_FACTOR),
    F3 = (uint32_t) (174.614f * NOTE_FACTOR),
    Fs3 = (uint32_t) (184.997f * NOTE_FACTOR),
    Gf3 = (uint32_t) (184.997f * NOTE_FACTOR),
    G3 = (uint32_t) (195.998f * NOTE_FACTOR),
    Gs3 = (uint32_t) (207.652f * NOTE_FACTOR),
    Af3 = (uint32_t) (207.652f * NOTE_FACTOR),
    A3 = (uint32_t) (220.000f * NOTE_FACTOR),
    As3 = (uint32_t) (233.082f * NOTE_FACTOR),
    Bf3 = (uint32_t) (233.082f * NOTE_FACTOR),
    B3 = (uint32_t) (246.942f * NOTE_FACTOR),
    C4 = (uint32_t) (261.626f * NOTE_FACTOR),
    Cs4 = (uint32_t) (277.183f * NOTE_FACTOR),
    Df4 = (uint32_t) (277.183f * NOTE_FACTOR),
    D4 = (uint32_t) (293.665f * NOTE_FACTOR),
    Ds4 = (uint32_t) (311.127f * NOTE_FACTOR),
    Ef4 = (uint32_t) (311.127f * NOTE_FACTOR),
    E4 = (uint32_t) (329.628f * NOTE_FACTOR),
    F4 = (uint32_t) (349.228f * NOTE_FACTOR),
    Fs4 = (uint32_t) (369.994f * NOTE_FACTOR),
    Gf4 = (uint32_t) (369.994f * NOTE_FACTOR),
    G4 = (uint32_t) (391.995f * NOTE_FACTOR),
    Gs4 = (uint32_t) (415.305f * NOTE_FACTOR),
    Af4 = (uint32_t) (415.305f * NOTE_FACTOR),
    A4 = (uint32_t) (440.000f * NOTE_FACTOR),
    As4 = (uint32_t) (466.164f * NOTE_FACTOR),
    Bf4 = (uint32_t) (466.164f * NOTE_FACTOR),
    B4 = (uint32_t) (493.883f * NOTE_FACTOR),
    C5 = (uint32_t) (523.251f * NOTE_FACTOR),
    Cs5 = (uint32_t) (554.365f * NOTE_FACTOR),
    Df5 = (uint32_t) (554.365f * NOTE_FACTOR),
    D5 = (uint32_t) (587.330f * NOTE_FACTOR),
    Ds5 = (uint32_t) (622.254f * NOTE_FACTOR),
    Ef5 = (uint32_t) (622.254f * NOTE_FACTOR),
    E5 = (uint32_t) (659.255f * NOTE_FACTOR),
    F5 = (uint32_t) (698.456f * NOTE_FACTOR),
    Fs5 = (uint32_t) (739.989f * NOTE_FACTOR),
    Gf5 = (uint32_t) (739.989f * NOTE_FACTOR),
    G5 = (uint32_t) (783.991f * NOTE_FACTOR),
    Gs5 = (uint32_t) (830.609f * NOTE_FACTOR),
    Af5 = (uint32_t) (830.609f * NOTE_FACTOR),
    A5 = (uint32_t) (880.000f * NOTE_FACTOR),
    As5 = (uint32_t) (932.328f * NOTE_FACTOR),
    Bf5 = (uint32_t) (932.328f * NOTE_FACTOR),
    B5 = (uint32_t) (987.767f * NOTE_FACTOR),
    C6 = (uint32_t) (1046.50f * NOTE_FACTOR),
    Cs6 = (uint32_t) (1108.73f * NOTE_FACTOR),
    Df6 = (uint32_t) (1108.73f * NOTE_FACTOR),
    D6 = (uint32_t) (1174.66f * NOTE_FACTOR),
    Ds6 = (uint32_t) (1244.51f * NOTE_FACTOR),
    Ef6 = (uint32_t) (1244.51f * NOTE_FACTOR),
    E6 = (uint32_t) (1318.51f * NOTE_FACTOR),
    F6 = (uint32_t) (1396.91f * NOTE_FACTOR),
    Fs6 = (uint32_t) (1479.98f * NOTE_FACTOR),
    Gf6 = (uint32_t) (1479.98f * NOTE_FACTOR),
    G6 = (uint32_t) (1567.98f * NOTE_FACTOR),
    Gs6 = (uint32_t) (1661.22f * NOTE_FACTOR),
    Af6 = (uint32_t) (1661.22f * NOTE_FACTOR),
    A6 = (uint32_t) (1760.00f * NOTE_FACTOR),
    As6 = (uint32_t) (1864.66f * NOTE_FACTOR),
    Bf6 = (uint32_t) (1864.66f * NOTE_FACTOR),
    B6 = (uint32_t) (1975.53f * NOTE_FACTOR),
    C7 = (uint32_t) (2093.00f * NOTE_FACTOR),
    Cs7 = (uint32_t) (2217.46f * NOTE_FACTOR),
    Df7 = (uint32_t) (2217.46f * NOTE_FACTOR),
    D7 = (uint32_t) (2349.32f * NOTE_FACTOR),
    Ds7 = (uint32_t) (2489.02f * NOTE_FACTOR),
    Ef7 = (uint32_t) (2489.02f * NOTE_FACTOR),
    E7 = (uint32_t) (2637.02f * NOTE_FACTOR),
    F7 = (uint32_t) (2793.83f * NOTE_FACTOR),
    Fs7 = (uint32_t) (2959.96f * NOTE_FACTOR),
    Gf7 = (uint32_t) (2959.96f * NOTE_FACTOR),
    G7 = (uint32_t) (3135.96f * NOTE_FACTOR),
    Gs7 = (uint32_t) (3322.44f * NOTE_FACTOR),
    Af7 = (uint32_t) (3322.44f * NOTE_FACTOR),
    A7 = (uint32_t) (3520.00f * NOTE_FACTOR),
} note_t;

typedef enum {
    END = 0,
    _64TH = 1,
    _32ND = 2,
    _16TH = 4,
    _8TH = 8,
    _QTR = 16,
    _HLF = 32,
    _WHL = 64,
    _DQTR = 24,
    _3QTR = 48
} duration_t;

typedef enum {
    SEL_A,
    SEL_B
} sound_selection_t;

typedef struct {
    note_t note0;
    note_t note1;
    note_t note2;
    note_t note3;
    uint32_t duration; // in # of 64th notes, same format as duration_t, 0 to signify end of chord sequence
} chord_t;

void init_wavetable();
void init_sound();

void set_divider(float div, uint8_t index);
void set_freq(float freq, uint8_t index);
void set_note(note_t note, uint8_t index);
void play_sound(chord_t sound[], sound_selection_t sel);
void sound_isr_a();
void sound_cutoff_isr_a();
void sound_isr_b();
void sound_cutoff_isr_b();

static chord_t beep[] = {
    {A5, A5, A5, A5, _32ND},
    {REST, REST, REST, REST, END}
};

static chord_t intro_sound[] = {
    {E5, Fs5, G5, REST, _32ND},
    {REST, REST, REST, REST, END}
};

static chord_t long_note[] = {
    {REST, REST, REST, A4, 256},
    {REST, REST, REST, REST, END}
};

#endif