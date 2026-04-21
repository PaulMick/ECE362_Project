#include <stdint.h>
#include <stdbool.h>
#include "bullets.h"
#include "gen_utils.h"
#include "display_driver.h"
#include "display_utils.h"
#include "assets.h"
#include "player.h"
#include "sound.h"
#include "wall_logic.h"
#include "score.h"

#define FP_SHIFT 8
#define BULLET_SPEED_FP ((int32_t)2 << FP_SHIFT) // 2 px/frame
#define BULLET_W 1
#define BULLET_H 2

bullet_t player_bullets[MAX_PLAYER_BULLETS];
bullet_t enemy_bullets[MAX_ENEMY_BULLETS];

static int32_t to_fp(int px){ 
    return (int32_t)px << FP_SHIFT; 
}
static int from_fp(int32_t fp) { 
    return (int)(fp >> FP_SHIFT); 
}

static void pool_init(bullet_t *pool, int n) {
    for (int i = 0; i < n; i++) pool[i].active = false;
}

static bool pool_spawn(bullet_t *pool, int n, int x_px, int y_px, int32_t vy_fp) {
    for (int i = 0; i < n; i++) {
        if (pool[i].active) continue;
        pool[i].x_fp   = to_fp(x_px);
        pool[i].y_fp   = to_fp(y_px);
        pool[i].vy_fp  = vy_fp;
        pool[i].active = true;
        return true;
    }
    return false;
}

static void pool_update(bullet_t *pool, int n) {
    for (int i = 0; i < n; i++) {
        if (!pool[i].active) continue;
        pool[i].y_fp += pool[i].vy_fp;
        int y = from_fp(pool[i].y_fp);
        if (y + BULLET_H <= 0 || y >= ROWS) pool[i].active = false;
    }
}

static void pool_draw(bullet_t *pool, int n, uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < n; i++) {
        if (!pool[i].active) continue;
        int x = from_fp(pool[i].x_fp);
        int y = from_fp(pool[i].y_fp);
        for (int dy = 0; dy < BULLET_H; dy++) {
            draw_pixel(x, y + dy, r, g, b);
        }
    }
}

void bullets_init(void) {
    pool_init(player_bullets, MAX_PLAYER_BULLETS);
    pool_init(enemy_bullets, MAX_ENEMY_BULLETS);
}

void bullets_update(void) {
    pool_update(player_bullets, MAX_PLAYER_BULLETS);
    pool_update(enemy_bullets, MAX_ENEMY_BULLETS);
}

void bullets_draw(void) {
    pool_draw(player_bullets, MAX_PLAYER_BULLETS, 255, 255, 255);
    pool_draw(enemy_bullets, MAX_ENEMY_BULLETS, 255, 50, 50);
}

bool bullets_spawn_player(int x_px, int y_px) {
    return pool_spawn(player_bullets, MAX_PLAYER_BULLETS, x_px, y_px, -BULLET_SPEED_FP);
}

bool bullets_spawn_enemy(int x_px, int y_px) {
    return pool_spawn(enemy_bullets, MAX_ENEMY_BULLETS, x_px, y_px, BULLET_SPEED_FP);
}

// collision checking enemy bullets vs player sprite at (player.x_fp, ROWS - SHOOTER_HEIGHT - 1).
// player_on_hit() does nothing during damage animation (invincible), so we deactivate bullets on overlap
void bullets_check_player(void) {
    int px = from_fp(player.x_fp);
    int py = ROWS - SHOOTER_HEIGHT - 1;
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!enemy_bullets[i].active){
            continue;
        }
        int bx = from_fp(enemy_bullets[i].x_fp);
        int by = from_fp(enemy_bullets[i].y_fp);
        if (bx + BULLET_W > px && bx < px + SHOOTER_WIDTH && by + BULLET_H > py && by < py + SHOOTER_HEIGHT) {
            enemy_bullets[i].active = false;
            player_on_hit();
        }
        // walls
        if (bx >= 10 && bx <= 20 && by >= 17 && by <= 24 && get_wall_state(0) != 5) {
            enemy_bullets[i].active = false;
            damage_wall(0);
        }
        if (bx >= 27 && bx <= 37 && by >= 17 && by <= 24 && get_wall_state(1) != 5) {
            enemy_bullets[i].active = false;
            damage_wall(1);
        }
        if (bx >= 44 && bx <= 54 && by >= 17 && by <= 24 && get_wall_state(2) != 5) {
            enemy_bullets[i].active = false;
            damage_wall(2);
        }
    }
}

// collision checking player bullets vs alive enemies. On hit, deactivate bullet and kill enemy.
void bullets_check_enemies(void) {
    for (int b = 0; b < MAX_PLAYER_BULLETS; b++) {
        if (!player_bullets[b].active) continue;
        int bx = from_fp(player_bullets[b].x_fp);
        int by = from_fp(player_bullets[b].y_fp);
        for (int e = 0; e < MAX_ENEMY_COUNT; e++) {
            if (!enemies[e].alive) continue;
            int ex = from_fp(enemies[e].x_fp);
            int ey = from_fp(enemies[e].y_fp);
            if (bx + BULLET_W > ex && bx < ex + ENEMY_W &&
                by + BULLET_H > ey && by < ey + ENEMY_H) {
                play_sound(enemy_death_sound, SEL_B);
                player_bullets[b].active = false;
                enemies[e].alive = false;
                enemies_killed++;
                break;
            }
            // walls
            if (bx >= 10 && bx <= 20 && by >= 17 && by <= 24 && get_wall_state(0) != 5) {
                player_bullets[b].active = false;
                // damage_wall(0);
            }
            if (bx >= 27 && bx <= 37 && by >= 17 && by <= 24 && get_wall_state(1) != 5) {
                player_bullets[b].active = false;
                // damage_wall(1);
            }
            if (bx >= 44 && bx <= 54 && by >= 17 && by <= 24 && get_wall_state(2) != 5) {
                player_bullets[b].active = false;
                // damage_wall(2);
            }
        }
    }
}
