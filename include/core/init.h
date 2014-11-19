#ifndef FINITE_INIT_H
#define FINITE_INIT_H

#include "init.h"

void boardInit(board_t *);
void bagInit(bag_t *);
void playerInit(player_t *, bag_t *);
void initplayer_tHuman(player_t *, bag_t *);
void initplayer_tAI(player_t *, bag_t *);
void moveInit(move_t *);
bool dictInit(dict_t *, const char *);
bool dictInitCount7(dict_t *, float *count, float increase, const char *);
void dictQuit(dict_t *);

#endif
