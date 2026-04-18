#include <stdint.h>
#include <stdbool.h>
#include "player.h"
#include "gen_utils.h"
#include "inputs.h"
#include "display_driver.h"
#include "display_utils.h"
#include "assets.h"
#include "bullets.h"

#define FP_SHIFT 8
#define KP_FP ((int32_t)((1 << FP_SHIFT) / 4)) // ~0.25, raise if response sluggish (/3 etc)
#define KD_FP ((int32_t)(((1 << FP_SHIFT) * 2) / 5)) // ~0.40, raise if too zippy (/4 etc)
#define SHOOT_COOLDOWN 4 // in frames
#define POT_MAX 4095
#define PLAYER_Y (ROWS - SHOOTER_HEIGHT - 1)

player_t player;

static int32_t to_fp(int px){ 
    return (int32_t)px << FP_SHIFT; 
}
static int from_fp(int32_t fp){ 
    return (int)(fp >> FP_SHIFT); 
}

void player_init(void) {
    player.x_fp = to_fp((COLS - SHOOTER_WIDTH) / 2);
    player.vx_fp = 0;
    player.lives = PLAYER_START_LIVES;
    player.shoot_cooldown = 0;
    player.dmg_frames = 0;
}

void player_update(void) {
    // pd move from potentiometer target
    int target_px = ((uint32_t)input_pot_raw() * (COLS - SHOOTER_WIDTH)) / POT_MAX;
    int32_t target_fp = to_fp(target_px);
    int32_t error_fp = target_fp - player.x_fp;
    player.vx_fp = ((KP_FP * error_fp) >> FP_SHIFT) - ((KD_FP * player.vx_fp) >> FP_SHIFT);

    player.x_fp += player.vx_fp;
    if (player.x_fp < 0) {
        player.x_fp  = 0;
        player.vx_fp = 0;
    }
    int32_t max_fp = to_fp(COLS - SHOOTER_WIDTH);
    if (player.x_fp > max_fp) {
        player.x_fp  = max_fp;
        player.vx_fp = 0;
    }

    // if player in damage animation, decrement
    if (player.dmg_frames > 0){
        player.dmg_frames--;
    }

    // shoot on button press if alive and not on cooldown
    if (player.shoot_cooldown > 0){
        player.shoot_cooldown--;
    }
    if (player.lives > 0 && player.shoot_cooldown == 0 && input_button_pressed()) {
        bullets_spawn_player(from_fp(player.x_fp) + SHOOTER_WIDTH/2, PLAYER_Y - 1);
        player.shoot_cooldown = SHOOT_COOLDOWN;
    }
}

void player_draw(void) {
    if (player.dmg_frames > 0 && ((player.dmg_frames / DMG_FLASH_PERIOD) & 1)){
        return;
    }
    if (player.lives <= 0 && player.dmg_frames == 0){
        return;
    }
    draw_img(from_fp(player.x_fp), PLAYER_Y, IMG_SHOOTER);
}

void player_on_hit(void) {
    if (player.dmg_frames > 0){
        return;
    }
    player.lives--;
    player.dmg_frames = DMG_DURATION;
}

bool player_is_dead(void) {
    return player.lives <= 0;
}
