#ifndef INPUTS_H
#define INPUTS_H

#include <stdbool.h>
#include <stdint.h>

#define START_BUTTON_PIN 44u
#define POT_PIN 45u
#define POT_ADC_CHAN 5u
//self explanatory
int init_inputs();
//polling
int update_inputs();
//getters
bool input_button_pressed(void);
uint16_t input_pot_raw(void);
#endif
