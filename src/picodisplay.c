#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "display_driver.h"
#include "display_utils.h"
#include "enemy_logic.h"

int init() {
    // stdio
    stdio_init_all();

    // display driver
    DisplayHandle dh = init_display_driver();

    // display utils
    init_display_utils(dh);

    enemy_logic_init();

    return 0;
}

int run() {
    while (1) {
        fill_frame(0, 0, 0);

        enemy_logic_update();
        enemy_logic_draw();
        draw_img((COLS - 10) / 2, ROWS - 7, IMG_SHOOTER);

        update_frame();
        sleep_ms(16);
    }

    return 0;
}