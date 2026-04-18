#ifndef GEN_UTILS
#define GEN_UTILS

typedef enum {
    LINE_UP, LINE_DOWN, LINE_LEFT, LINE_RIGHT
} line_dir_t;

typedef enum {
    FONT_5X5_FLEX
} font_t;

typedef enum {
    IMG_SMILE, IMG_ENEMY1, IMG_ENEMY2, IMG_ENEMY3, IMG_ENEMY4, IMG_SHOOTER
} img_t;

uint32_t start_time;

#endif