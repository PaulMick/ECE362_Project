#ifndef GEN_UTILS
#define GEN_UTILS

#include <stdint.h>

typedef enum {
    LINE_UP, LINE_DOWN, LINE_LEFT, LINE_RIGHT
} line_dir_t;

typedef enum {
    FONT_5X5_FLEX
} font_t;

typedef enum {
    IMG_SMILE, IMG_ENEMY1, IMG_ENEMY2, IMG_ENEMY3, IMG_ENEMY4, IMG_SHOOTER
} img_t;

typedef enum {
    ENEMY_FLY_IN,
    ENEMY_IDLE
} enemy_motion_t;

typedef struct {
    int32_t x_fp;
    int32_t y_fp;
    int32_t vx_fp;
    int32_t vy_fp;
    int32_t target_x_fp;
    int32_t target_y_fp;
    int fly_in_frames;
    int fly_in_step;
    img_t img;
    enemy_motion_t motion;
} enemy_t;

#define MAX_ENEMY_COUNT 40

extern enemy_t enemies[MAX_ENEMY_COUNT];

#endif