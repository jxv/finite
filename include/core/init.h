#ifndef FINITE_INIT_H
#define FINITE_INIT_H

#include "init.h"

void board_init(board_t *);
void bag_init(bag_t *);
void player_init(player_t *, bag_t *);
void init_player_human(player_t *, bag_t *);
void init_player_ai(player_t *, bag_t *);
void move_init(move_t *);
bool dict_init(dict_t *, const char *);
bool dict_init_count_7(dict_t *, float *count, float increase, const char *);
void dict_quit(dict_t *);

#endif
