#ifndef FINITE_INIT_H
#define FINITE_INIT_H


#include "init.h"


void boardInit(struct board *b);
void bagInit(struct bag *b);
void playerInit(struct player *p, struct bag *b);
void moveInit(struct move *m);
bool dictInit(struct dict*, const char*);
void dictQuit(struct dict*);


#endif


