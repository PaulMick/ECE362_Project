#ifndef SOUND_H
#define SOUND_H

void init_wavetable();
void init_sound();

void set_dividers(float div0, float div1, float div2, float div3);
void set_freqs(float freq0, float freq1, float freq2, float freq3);

#endif