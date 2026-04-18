#ifndef BULLETS_H
#define BULLETS_H

#include <stdbool.h>

void bullets_init(void);
void bullets_update(void);
void bullets_draw(void);

bool bullets_spawn_player(int x_px, int y_px);
bool bullets_spawn_enemy(int x_px, int y_px);

void bullets_check_player(void);
void bullets_check_enemies(void);

#endif
