#ifndef FINITE_CORE_AI_H
#define FINITE_CORE_AI_H

#include "core.h"

#define PLACEMENTS(x) (((x) * (x) - (x)) / 2)
#define ALL_PLACEMENTS  (BOARD_X * PLACEMENTS(BOARD_Y) + BOARD_Y * PLACEMENTS(BOARD_X))

void aiFindMove(Move *, int, Game *, Rule *, float *loading);

#endif

