#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <stdint.h>

void init_leaderboard();
void eeprom_write(uint16_t write_addr, uint8_t *read_addr, int len_bytes);
void eeprom_read(uint16_t read_addr, uint8_t *write_addr, int len_bytes);
int reserved_addr(uint8_t addr);
void scan_i2c();

#endif