#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

void player_init(void);
void player_update(void);
void player_draw(void);
void player_on_hit(void);
bool player_is_dead(void);

#endif
