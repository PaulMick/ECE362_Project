#ifndef GEN_UTILS
#define GEN_UTILS

#include <stdint.h>
#include <stdbool.h>

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

typedef struct {
    int32_t x_fp;
    int32_t y_fp;
    int32_t vy_fp;
    bool active;
} bullet_t;

typedef struct {
    int32_t x_fp;
    int32_t vx_fp;
    int lives;
    int shoot_cooldown;
    int dmg_frames;
} player_t;

#define MAX_ENEMY_COUNT 40
#define MAX_ENEMY_BULLETS 12

#define PLAYER_START_LIVES 3
#define MAX_PLAYER_BULLETS 8
#define DMG_DURATION 60 // num frames of damage flashing
#define DMG_FLASH_PERIOD 4 // how quick to flash (in frames)

extern enemy_t enemies[MAX_ENEMY_COUNT];
extern bullet_t player_bullets[MAX_PLAYER_BULLETS];
extern bullet_t enemy_bullets[MAX_ENEMY_BULLETS];
extern player_t player;

#endif