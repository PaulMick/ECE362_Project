#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "gen_utils.h"
#include "display_driver.h"
#include "display_utils.h"
#include "sound.h"

int init() {
    // stdio
    stdio_init_all();

    // display driver
    DisplayHandle dh = init_display_driver();

    // display utils
    init_display_utils(dh);

    // sound
    init_sound();

    return 0;
}

int run() {
    int x = 1;
    while (1) {
        // fill_frame(0, 0, 0);
        // ///////////////////////////

        // draw_rect(0, 0, 64, 32, 1, 255, 255, 255);

        // draw_str(2, 2, "HELLO WORLD", FONT_5X5_FLEX, 255, 255, 0);

        // draw_img(2, 8, IMG_SMILE);

        
        // draw_line(x, 2, LINE_DOWN, 28, 255, 0, 0);
        // x ++;
        // if (x == 63) {
        //     x = 0;
        // }
        
        // ///////////////////////////
        // sleep_ms(1);
        // update_frame();
        // sleep_ms(9);
        set_freqs(x, x, x, x);
        x ++;
        sleep_ms(10);
    }

    return 0;
}