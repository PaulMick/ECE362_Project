#ifndef LEADERBOARD_DISPLAY_H
#define LEADERBOARD_DISPLAY_H

#include <stdint.h>

//EITHER: player made top 3, so  prompt for initials, then go to board
void leaderboard_display_init_entry(uint32_t score);
int  leaderboard_display_update_entry(void); // returns 1 when all 3 letters entered and saved
void leaderboard_display_draw_entry(void);

//OR: player missed top 3, so skip data entry, go straight to board
void leaderboard_display_init_board(uint32_t score);

//board screen
void leaderboard_display_draw_board(void);

#endif
