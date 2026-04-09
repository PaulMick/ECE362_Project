#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "display_driver.h"
#include "display_utils.h"
#include "enemy_logic.h"
#include "start_screen.h"
#include "inputs.h"

static int active_level = 1;
static int start_screen = 1;

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

    return 0;
}

int run() {
    int player_x = (COLS - 10) / 2;
    float player_vel = 0;
    while (1) {
        update_inputs();
        fill_frame(0, 0, 0);
        if (start_screen){
            start_screen_draw();
            if (input_button_pressed()) {
                start_screen = 0;
                enemy_level_init();
            }
        }
        else{
            //player_x = move_player(player_x, player_vel); coming soon
            enemy_level_update();
            enemy_logic_draw();
            draw_img(player_x, ROWS - 7, IMG_SHOOTER);
        }
        update_frame();
        sleep_ms(50);
    }

    return 0;
}