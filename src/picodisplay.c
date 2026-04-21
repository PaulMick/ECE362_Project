#include <stdio.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "display_driver.h"
#include "display_utils.h"
#include "enemy_logic.h"
#include "start_screen.h"
#include "inputs.h"
#include "sound.h"
#include "leaderboard.h"
#include "player.h"
#include "bullets.h"
#include "wall_logic.h"
#include "score.h"
#include "leaderboard_display.h"

static int active_level = 1;
static int start_screen = 1;
static int entering_player_info = 0;
static int showing_leaderboard = 0;
static uint32_t final_score = 0;
int frame_count = 0;


static void enemy_level_init(void) {
    if (active_level == 1) {
        enemy_logic_init_level1();
    } else if (active_level == 2) {
        enemy_logic_init_level2();
    } else {
        enemy_logic_init_level3();
    }
}

static void enemy_level_update(void) {
    if (active_level == 1) {
        enemy_logic_update_level1();
    } else if (active_level == 2) {
        enemy_logic_update_level2();
    } else {
        enemy_logic_update_level3();
    }
}

static void advance_level_if_complete(void) {
    if (!enemy_logic_all_dead()) {
        return;
    }

    if (active_level < 3) {
        active_level++;
        enemy_level_init();
        bullets_init();
    } else {
        //player cleared level 3 — win path into score flow
        survived_to_end = 1;
        final_score = score_compute();
        if (final_score > leaderboard[2].score) {
            leaderboard_display_init_entry(final_score);
            entering_player_info = 1;
        } else {
            leaderboard_display_init_board(final_score);
            showing_leaderboard = 1;
        }
        active_level = 1;
    }
}

int init() {
    // stdio
    stdio_init_all();

    // display driver
    DisplayHandle dh = init_display_driver();

    // display utils
    init_display_utils(dh);

    //input handling
    init_inputs();

    // sound
    init_sound();

    // leaderboard
    init_leaderboard();

    return 0;
}

int run() {
    while (1) {
        update_inputs();
        fill_frame(0, 0, 0);
        if (start_screen){
            start_screen_draw();
            if (input_button_pressed()) {
                play_sound(background_music, SEL_A);
                start_screen = 0;
                score_reset();
                enemy_level_init();
                player_init();
                bullets_init();
                walls_init();
            }
        }
        else if (entering_player_info) {
            if (leaderboard_display_update_entry()) {
                entering_player_info = 0;
                showing_leaderboard = 1;
            }
            leaderboard_display_draw_entry();
        }
        else if (showing_leaderboard) {
            leaderboard_display_draw_board();
            if (input_button_pressed()) {
                showing_leaderboard = 0;
                start_screen = 1;
                reset_start_screen();
            }
        }
        else{
            if (player.lives == 0) {
                play_sound(stop_sound, SEL_A);
                sleep_ms(500);
                play_sound(player_death_sound, SEL_A);
                sleep_ms(2000);
                //resync button state after 2.5s of blocked input handling
                update_inputs();
                final_score = score_compute();
                if (final_score > leaderboard[2].score) {
                    leaderboard_display_init_entry(final_score);
                    entering_player_info = 1;
                } else {
                    leaderboard_display_init_board(final_score);
                    showing_leaderboard = 1;
                }
                active_level = 1;
                continue;
            }
            // logic
            survival_frames++;
            player_update();
            enemy_level_update();
            bullets_update();
            bullets_check_player();
            bullets_check_enemies();
            advance_level_if_complete();

            // draw (back to front)
            print_walls();
            enemy_logic_draw();
            bullets_draw();
            player_draw();
        }
        update_frame();
        frame_count++;
        sleep_ms(16);
    }

    return 0;
}