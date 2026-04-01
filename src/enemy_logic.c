#include <stdint.h>
#include "enemy_logic.h"
#include "display_driver.h"
#include "display_utils.h"
#include "gen_utils.h"

#define ENEMY_COUNT 8

#define FP_SHIFT 8
#define FP_ONE (1 << FP_SHIFT)

typedef enum {
    ENEMY_FLY_IN,
    ENEMY_ROAM
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
    int roam_min_x;
    int roam_max_x;
    int roam_min_y;
    int roam_max_y;
    img_t img;
    enemy_motion_t motion;
} enemy_t;

static enemy_t enemies[ENEMY_COUNT];

static int to_fp(int px) {
    return px << FP_SHIFT;
}

static int from_fp(int fp) {
    return fp >> FP_SHIFT;
}

static int32_t abs_i32(int32_t v) {
    return (v < 0) ? -v : v;
}

void enemy_logic_init(void) {
    const int target_x[ENEMY_COUNT] = {4, 18, 32, 46, 4, 18, 32, 46};
    const int target_y[ENEMY_COUNT] = {5, 5, 5, 5, 12, 12, 12, 12};
    const int start_x[ENEMY_COUNT] = {-8, 16, 40, 72, -10, 14, 42, 74};
    const int start_y[ENEMY_COUNT] = {-8, -10, -9, -11, -20, -18, -19, -21};
    const img_t sprites[4] = {IMG_ENEMY1, IMG_ENEMY2, IMG_ENEMY3, IMG_ENEMY4};

    for (int i = 0; i < ENEMY_COUNT; i++) {
        const int frames = 30 + i * 4;
        enemies[i].x_fp = to_fp(start_x[i]);
        enemies[i].y_fp = to_fp(start_y[i]);
        enemies[i].target_x_fp = to_fp(target_x[i]);
        enemies[i].target_y_fp = to_fp(target_y[i]);
        enemies[i].vx_fp = (enemies[i].target_x_fp - enemies[i].x_fp) / frames;
        enemies[i].vy_fp = (enemies[i].target_y_fp - enemies[i].y_fp) / frames;
        enemies[i].fly_in_frames = frames;
        enemies[i].fly_in_step = 0;
        enemies[i].roam_min_x = target_x[i] - 3;
        enemies[i].roam_max_x = target_x[i] + 3;
        enemies[i].roam_min_y = target_y[i] - 2;
        enemies[i].roam_max_y = target_y[i] + 2;
        enemies[i].img = sprites[i % 4];
        enemies[i].motion = ENEMY_FLY_IN;
    }
}

void enemy_logic_update(void) {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        enemy_t *e = &enemies[i];
        if (e->motion == ENEMY_FLY_IN) {
            e->x_fp += e->vx_fp;
            e->y_fp += e->vy_fp;
            e->fly_in_step++;
            if (e->fly_in_step >= e->fly_in_frames) {
                e->x_fp = e->target_x_fp;
                e->y_fp = e->target_y_fp;
                e->motion = ENEMY_ROAM;
                e->vx_fp = ((i % 2) == 0) ? (FP_ONE / 4) : -(FP_ONE / 4);
                e->vy_fp = ((i % 3) == 0) ? (FP_ONE / 8) : -(FP_ONE / 8);
            }
            continue;
        }

        e->x_fp += e->vx_fp;
        e->y_fp += e->vy_fp;

        int x = from_fp(e->x_fp);
        int y = from_fp(e->y_fp);

        if (x <= e->roam_min_x) {
            e->x_fp = to_fp(e->roam_min_x);
            e->vx_fp = abs_i32(e->vx_fp);
        } else if (x >= e->roam_max_x) {
            e->x_fp = to_fp(e->roam_max_x);
            e->vx_fp = -abs_i32(e->vx_fp);
        }

        if (y <= e->roam_min_y) {
            e->y_fp = to_fp(e->roam_min_y);
            e->vy_fp = abs_i32(e->vy_fp);
        } else if (y >= e->roam_max_y) {
            e->y_fp = to_fp(e->roam_max_y);
            e->vy_fp = -abs_i32(e->vy_fp);
        }
    }
}

void enemy_logic_draw(void) {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        draw_img(from_fp(enemies[i].x_fp), from_fp(enemies[i].y_fp), enemies[i].img);
    }
}