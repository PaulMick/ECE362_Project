#ifndef SCORE_H
#define SCORE_H

#include <stdint.h>

// run state written by gameplay, consumed on death/win
// 3 scoring metrics:
extern uint16_t enemies_killed;
extern uint32_t survival_frames;
extern int survived_to_end;

// reset counters on start
void score_reset(void);
// compute final score from the counters above
uint32_t score_compute(void);

#endif
