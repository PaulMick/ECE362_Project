#ifndef GEN_UTILS
#define GEN_UTILS

typedef enum {
    LINE_UP, LINE_DOWN, LINE_LEFT, LINE_RIGHT
} line_dir_t;

typedef enum {
    FONT_5X5_FLEX
} font_t;

typedef enum {
    IMG_SMILE, IMG_ENEMY1, IMG_ENEMY2, IMG_ENEMY3, IMG_ENEMY4, IMG_SHOOTER, IMG_WALL_1, IMG_WALL_DMG_1, IMG_WALL_DMG_2, IMG_WALL_DMG_3, IMG_WALL_DMG_4, IMG_EMPTY_WALL
} img_t;

typedef struct {
    int8_t x;
    int8_t y;
    int8_t state; //0=undamaged, 1=damaged, 2=more damaged, 3=more damaged, 4=last stage before disappearing, 5=destroyed
} wall_t;

extern wall_t walls[3];

#endif