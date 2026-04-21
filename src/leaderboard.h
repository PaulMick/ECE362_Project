#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <stdint.h>

#define LEADERBOARD_SLOTS 10

typedef struct {
    char *initials; // 4 initial chars and \0
    uint32_t score; // score
} leaderboard_slot_t;

extern leaderboard_slot_t leaderboard[LEADERBOARD_SLOTS]; //for display

void init_leaderboard();
void eeprom_write(uint16_t write_addr, uint8_t *read_addr, int len_bytes);
void eeprom_read(uint16_t read_addr, uint8_t *write_addr, int len_bytes);
leaderboard_slot_t *get_leaderboard();
void add_score_leaderboard(char *initials, uint32_t score);
void save_leaderboard();
void wipe_leaderboard();

int reserved_addr(uint8_t addr);
void scan_i2c();

#endif