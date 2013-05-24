#ifndef SCABS_INIT_H
#define SCABS_INIT_H


#include "init.h"


void init_board(struct board *b);
void init_bag(struct bag *b);
void init_player(struct player *p, struct bag *b);
void init_move(struct move *m);


#endif


