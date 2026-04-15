#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <stdint.h>

void init_leaderboard();
void eeprom_write(uint16_t eeprom_addr, void *read_addr, uint32_t len_bytes);

#endif