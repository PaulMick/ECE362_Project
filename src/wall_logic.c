#include <stdint.h>
#include "display_driver.h"
#include "display_utils.h"
#include "gen_utils.h"
#include "wall_logic.h"

wall_t walls[3];

void walls_init() {
    walls[0].x = 10;
    walls[0].y = 17;
    walls[0].state = 0;
    walls[1].x = 27;
    walls[1].y = 17;
    walls[1].state = 0;
    walls[2].x = 44;
    walls[2].y = 17;
    walls[2].state = 0;
}

void damage_wall(int i) { //could also use a wall obj here to directly modify
    walls[i].state++;
    if (walls[i].state > 5) {
        walls[i].state = 5;
    }
}

void print_walls() {
    for(int i = 0; i < 3; i++) {
        // printf("%d\n", walls[i].state);
        switch(walls[i].state) {
            case(0):
                draw_img(walls[i].x, walls[i].y, IMG_WALL_1);
            break;
            case(1):
                draw_img(walls[i].x, walls[i].y, IMG_WALL_DMG_1);
            break;
            case(2):
                draw_img(walls[i].x, walls[i].y, IMG_WALL_DMG_2);
            break;
            case(3):
                draw_img(walls[i].x, walls[i].y, IMG_WALL_DMG_3);
            break;
            case(4):
                draw_img(walls[i].x, walls[i].y, IMG_WALL_DMG_4);
            break;
            case(5):
                draw_img(walls[i].x, walls[i].y, IMG_EMPTY_WALL);
            break;
        }
    }
}