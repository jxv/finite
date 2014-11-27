#ifndef FINITE_CORE_AI_H
#define FINITE_CORE_AI_H

#include "core.h"

#define PLACEMENTS(x) (((x) * (x) - (x)) / 2)
#define ALL_PLACEMENTS  (BOARD_X * PLACEMENTS(BOARD_Y) + \
			 BOARD_Y * PLACEMENTS(BOARD_X))

void ai_find_move(move_t *, int, game_t *, rule_t *, float *loading);

#endif
