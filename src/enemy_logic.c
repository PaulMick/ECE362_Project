#include <stdint.h>
#include <stdlib.h>
#include "enemy_logic.h"
#include "display_driver.h"
#include "display_utils.h"
#include "gen_utils.h"
#include "bullets.h"
#include "sound.h"

#define FP_SHIFT 8 //enemy velo

typedef struct {
    int enemy_count;
    int enemy_cols;
    int enemy_rows;
    int enemy_gap_x;
    int enemy_gap_y;
} level_specs_t;

static const level_specs_t LEVEL1_SPECS = {
    .enemy_count = 4,
    .enemy_cols = 2,
    .enemy_rows = 2,
    .enemy_gap_x = 10,
    .enemy_gap_y = 5,
};

static const level_specs_t LEVEL2_SPECS = {
    .enemy_count = 8,
    .enemy_cols = 4,
    .enemy_rows = 2,
    .enemy_gap_x = 4,
    .enemy_gap_y = 3,
};

static const level_specs_t LEVEL3_SPECS = {
    .enemy_count = 40,
    .enemy_cols = 10,
    .enemy_rows = 4,
    .enemy_gap_x = 0,
    .enemy_gap_y = 0,
};

static int active_enemy_count = 0;
static int formation_dir = 1;
static int formation_speed_fp = 0;

static int to_fp(int px) {
    return px << FP_SHIFT;
}

static int from_fp(int fp) {
    return fp >> FP_SHIFT;
}

static int formation_speed_for_specs(const level_specs_t *specs) {
    if (specs->enemy_count <= 4) {
        return to_fp(1) / 5;
    }
    if (specs->enemy_count <= 8) {
        return to_fp(1) / 4;
    }
    return to_fp(1) / 3;
}

static void enemy_logic_init_with_specs(const level_specs_t *specs) {
    const img_t sprites[4] = {IMG_ENEMY1, IMG_ENEMY2, IMG_ENEMY3, IMG_ENEMY4};
    const int bottom_reserved_h = 7;
    const int formation_w = specs->enemy_cols * ENEMY_W + (specs->enemy_cols - 1) * specs->enemy_gap_x;
    const int formation_h = specs->enemy_rows * ENEMY_H + (specs->enemy_rows - 1) * specs->enemy_gap_y;
    const int formation_x0 = (COLS - formation_w) / 2;
    const int formation_y0 = ((ROWS - bottom_reserved_h) - formation_h) / 2 - 2;
    active_enemy_count = specs->enemy_count;
    formation_dir = 1;
    formation_speed_fp = formation_speed_for_specs(specs);

    for (int i = 0; i < active_enemy_count; i++) {
        const int col = i % specs->enemy_cols;
        const int row = i / specs->enemy_cols;
        const int target_x = formation_x0 + col * (ENEMY_W + specs->enemy_gap_x);
        const int target_y = formation_y0 + row * (ENEMY_H + specs->enemy_gap_y);
        const int frames = 30 + i * 4;
        enemies[i].x_fp = to_fp((col * (COLS / specs->enemy_cols)) - ENEMY_W);
        enemies[i].y_fp = to_fp(-(10 + row * 6 + (i % 3)));
        enemies[i].target_x_fp = to_fp(target_x);
        enemies[i].target_y_fp = to_fp(target_y);
        enemies[i].vx_fp = (enemies[i].target_x_fp - enemies[i].x_fp) / frames;
        enemies[i].vy_fp = (enemies[i].target_y_fp - enemies[i].y_fp) / frames;
        enemies[i].fly_in_frames = frames;
        enemies[i].fly_in_step = 0;
        enemies[i].img = sprites[i % 4];
        enemies[i].motion = ENEMY_FLY_IN;
        enemies[i].alive = true;
    }

    for (int i = active_enemy_count; i < MAX_ENEMY_COUNT; i++) {
        enemies[i].x_fp = to_fp(-ENEMY_W);
        enemies[i].y_fp = to_fp(-ENEMY_H);
        enemies[i].vx_fp = 0;
        enemies[i].vy_fp = 0;
        enemies[i].target_x_fp = enemies[i].x_fp;
        enemies[i].target_y_fp = enemies[i].y_fp;
        enemies[i].fly_in_frames = 0;
        enemies[i].fly_in_step = 0;
        enemies[i].img = IMG_ENEMY1;
        enemies[i].motion = ENEMY_IDLE;
        enemies[i].alive = false;
    }
}

static void enemy_logic_update_with_specs(const level_specs_t *specs) {
    (void)specs;
    for (int i = 0; i < active_enemy_count; i++) {
        enemy_t *e = &enemies[i];
        if (!e->alive) {
            continue;
        }
        if (e->motion == ENEMY_FLY_IN) {
            e->x_fp += e->vx_fp;
            e->y_fp += e->vy_fp;
            e->fly_in_step++;
            if (e->fly_in_step >= e->fly_in_frames) {
                e->x_fp = e->target_x_fp;
                e->y_fp = e->target_y_fp;
                e->motion = ENEMY_IDLE;
                e->vx_fp = 0;
                e->vy_fp = 0;
            }
            continue;
        }

        e->x_fp += e->vx_fp;
        e->y_fp += e->vy_fp;
    }

    const int min_x_fp = to_fp(0);
    const int max_x_fp = to_fp(COLS - ENEMY_W);
    int side_dx_fp = formation_dir * formation_speed_fp;
    bool hit_edge = false;
    bool has_idle_enemy = false;

    for (int i = 0; i < active_enemy_count; i++) {
        enemy_t *e = &enemies[i];
        if (!e->alive || e->motion == ENEMY_FLY_IN) {
            continue;
        }
        has_idle_enemy = true;
        const int next_x_fp = e->x_fp + side_dx_fp;
        if (next_x_fp < min_x_fp || next_x_fp > max_x_fp) {
            hit_edge = true;
            break;
        }
    }

    if (has_idle_enemy) {
        if (hit_edge) {
            formation_dir = -formation_dir;
            side_dx_fp = formation_dir * formation_speed_fp;
        }

        for (int i = 0; i < active_enemy_count; i++) {
            enemy_t *e = &enemies[i];
            if (!e->alive || e->motion == ENEMY_FLY_IN) {
                continue;
            }
            e->x_fp += side_dx_fp;
        }
    }

    // Enemy firing — ~1/64 chance per tick; a random enemy (if alive) fires.
    if ((rand() & 0x3F) == 0 && active_enemy_count > 0) {
        int i = rand() % active_enemy_count;
        if (enemies[i].alive) {
            int ex = from_fp(enemies[i].x_fp);
            int ey = from_fp(enemies[i].y_fp);
            bullets_spawn_enemy(ex + ENEMY_W/2, ey + ENEMY_H);
            play_sound(enemy_bullet_sound, SEL_B);
        }
    }
}

void enemy_logic_init_level1(void) {
    enemy_logic_init_with_specs(&LEVEL1_SPECS);
}

void enemy_logic_init_level2(void) {
    enemy_logic_init_with_specs(&LEVEL2_SPECS);
}

void enemy_logic_init_level3(void) {
    enemy_logic_init_with_specs(&LEVEL3_SPECS);
}

void enemy_logic_init(void) {
    enemy_logic_init_level2();
}

void enemy_logic_update(void) {
    enemy_logic_update_level2();
}

void enemy_logic_update_level1(void) {
    enemy_logic_update_with_specs(&LEVEL1_SPECS);
}

void enemy_logic_update_level2(void) {
    enemy_logic_update_with_specs(&LEVEL2_SPECS);
}

void enemy_logic_update_level3(void) {
    enemy_logic_update_with_specs(&LEVEL3_SPECS);
}

void enemy_logic_draw(void) {
    for (int i = 0; i < active_enemy_count; i++) {
        if (!enemies[i].alive) {
            continue;
        }
        draw_img(from_fp(enemies[i].x_fp), from_fp(enemies[i].y_fp), enemies[i].img);
    }
}

bool enemy_logic_all_dead(void) {
    for (int i = 0; i < active_enemy_count; i++) {
        if (enemies[i].alive) {
            return false;
        }
    }
    return true;
}