#ifndef FINITE_INIT_H
#define FINITE_INIT_H

#include <opk.h>

#include "init.h"

void boardInit(Board *);
void bagInit(Bag *);
void playerInit(Player *, Bag *);
void initPlayerHuman(Player *, Bag *);
void initPlayerAI(Player *, Bag *);
void moveInit(Move *);
bool dictInit(Dict *, const char *);
bool dictInitCount7(Dict *, float *count, float increase, const char *);
void dictQuit(Dict *);

#endif

