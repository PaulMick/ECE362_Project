#ifndef ENEMY_LOGIC_H
#define ENEMY_LOGIC_H

#include <stdbool.h>

void enemy_logic_init(void);
void enemy_logic_update(void);
void enemy_logic_draw(void);

void enemy_logic_init_level1(void);
void enemy_logic_init_level2(void);
void enemy_logic_init_level3(void);

void enemy_logic_update_level1(void);
void enemy_logic_update_level2(void);
void enemy_logic_update_level3(void);

bool enemy_logic_all_dead(void);

#endif