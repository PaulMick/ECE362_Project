#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leaderboard_display.h"
#include "leaderboard.h"
#include "inputs.h"
#include "display_utils.h"

#define POT_MAX 4095

//same as start screen
#define C_R 0
#define C_G 255
#define C_B 100
//gold for player highlights
#define H_R 255
#define H_G 200
#define H_B 0

extern int frame_count; // defined in picodisplay.c

//entry state
static char entry_initials[4] = "***"; //switch to AAA if this breaks
static int entry_cursor = 0;
static uint16_t pot_samples[4]; // smooth input buffer
static int pot_idx = 0;

//result state, shared between entry and board screens
static uint32_t pending_score = 0;
static int player_rank = -1; // 0-2 if on board, -1 otherwise

// input smoothing for chill letter transitions
static uint16_t pot_smoothed(void) {
    pot_samples[pot_idx] = input_pot_raw();
    pot_idx = (pot_idx + 1) & 3;
    uint32_t sum = pot_samples[0] + pot_samples[1] + pot_samples[2] + pot_samples[3];
    return (uint16_t)(sum >> 2);
}

void leaderboard_display_init_entry(uint32_t score) {
    strcpy(entry_initials, "***"); //switch to AAA if this breaks
    entry_cursor = 0;
    pending_score = score;
    player_rank = -1;
    //prefill buffer for current pot pos
    uint16_t p = input_pot_raw();
    for (int i = 0; i < 4; i++) pot_samples[i] = p;
    pot_idx = 0;
}

void leaderboard_display_init_board(uint32_t score) {
    pending_score = score;
    player_rank = -1;
}

int leaderboard_display_update_entry(void) {
    //pot to letter index in [0,25]
    uint16_t pot = pot_smoothed();
    int idx = (pot * 26) / (POT_MAX + 1);
    if (idx > 25) idx = 25;
    entry_initials[entry_cursor] = 'A' + idx;

    if (input_button_pressed()) {
        entry_cursor++;
        if (entry_cursor == 3) {
            // get player rank before array dissapears
            player_rank = -1;
            for (int i = 0; i < LEADERBOARD_SLOTS; i++) {
                if (pending_score > leaderboard[i].score) {
                    player_rank = i;
                    break;
                }
            }
            //local initials buffer,  add_score_leaderboard stores the pointer
            char *stored = malloc(5);
            stored[0] = entry_initials[0];
            stored[1] = entry_initials[1];
            stored[2] = entry_initials[2];
            stored[3] = '\0';
            stored[4] = '\0';
            add_score_leaderboard(stored, pending_score);
            save_leaderboard(); // latency here but thats fine
            return 1;
        }
    }
    return 0;
}

void leaderboard_display_draw_entry(void) {
    //header across two lines (matches start_screen.c two-line layout)
    draw_str(15, 2, "NEW HIGH", FONT_5X5_FLEX, C_R, C_G, C_B);
    draw_str(23, 9, "SCORE",    FONT_5X5_FLEX, C_R, C_G, C_B);

    //three letter slots, active one blinks
    static const int slot_x[3] = {22, 32, 42};
    int slot_y = 20;
    for (int i = 0; i < 3; i++) {
        int is_active = (i == entry_cursor);
        //blink off half the time when active
        if (is_active && (frame_count / 3) % 2 == 0) continue;
        if (is_active) {
            draw_char(slot_x[i], slot_y, entry_initials[i], FONT_5X5_FLEX, H_R, H_G, H_B);
        } else {
            draw_char(slot_x[i], slot_y, entry_initials[i], FONT_5X5_FLEX, C_R, C_G, C_B);
        }
    }
}

void leaderboard_display_draw_board(void) {
    //header
    draw_str(5, 0, "LEADERBOARD", FONT_5X5_FLEX, C_R, C_G, C_B);

    //top 3 rows
    char row[16];
    for (int i = 0; i < 3; i++) {
        const char *initials = leaderboard[i].initials ? leaderboard[i].initials : "---";
        snprintf(row, sizeof(row), "%d %s %lu", i + 1, initials, (unsigned long)leaderboard[i].score);
        int highlighted = (i == player_rank);
        if (highlighted) {
            draw_str(2, 7 + i * 6, row, FONT_5X5_FLEX, H_R, H_G, H_B);
        } else {
            draw_str(2, 7 + i * 6, row, FONT_5X5_FLEX, C_R, C_G, C_B);
        }
    }

    //player's score is always higlighted
    snprintf(row, sizeof(row), "YOU: %lu", (unsigned long)pending_score);
    draw_str(2, 25, row, FONT_5X5_FLEX, H_R, H_G, H_B);
}
