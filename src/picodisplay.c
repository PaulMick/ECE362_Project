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

static int active_level = 2;
static int start_screen = 1;
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
                enemy_level_init();
                player_init();
                bullets_init();
            }
        }
        else{
            if (player.lives == 0) {
                play_sound(stop_sound, SEL_A);
                sleep_ms(500);
                play_sound(player_death_sound, SEL_A);
                sleep_ms(2000);
                start_screen = 1;
                continue;
            }
            // logic
            player_update();
            enemy_level_update();
            bullets_update();
            bullets_check_player();
            bullets_check_enemies();

            // draw (back to front)
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