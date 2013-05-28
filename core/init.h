#ifndef FINITE_INIT_H
#define FINITE_INIT_H


#include "init.h"


void init_board(struct board *b);
void init_bag(struct bag *b);
void init_player(struct player *p, struct bag *b);
void init_move(struct move *m);
bool init_dict(struct dict*, const char*);
void quit_dict(struct dict*);


#endif


