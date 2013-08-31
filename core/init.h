#ifndef FINITE_INIT_H
#define FINITE_INIT_H

#include <opk.h>

#include "init.h"

void boardInit(Board *);
void bagInit(Bag *);
void playerInit(Player *, Bag *, int rackSize);
void initPlayerHuman(Player *, Bag *, int rackSize);
void initPlayerAI(Player *, Bag *, int rackSize);
void moveInit(Move *);
bool dictInit(Dict *, const char *);
bool dictInitCount7(Dict *, float *count, float increase, const char *);
void dictQuit(Dict *);

#endif

