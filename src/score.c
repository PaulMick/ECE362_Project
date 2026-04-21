#include "score.h"

// TODO: tune
#define TIME_WEIGHT 10 // per second survived
#define KILL_WEIGHT 100 // per enemy killed
#define COMPLETION_BONUS 5000 // for clearing level 3

uint16_t enemies_killed = 0;
uint32_t survival_frames = 0;
int survived_to_end = 0;

void score_reset(void) {
    enemies_killed = 0;
    survival_frames = 0;
    survived_to_end = 0;
}

uint32_t score_compute(void) {
    return (survival_frames / 60) * TIME_WEIGHT // assume 60fps
        + (uint32_t)enemies_killed * KILL_WEIGHT
        + (survived_to_end ? COMPLETION_BONUS : 0);
}
