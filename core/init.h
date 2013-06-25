#ifndef FINITE_INIT_H
#define FINITE_INIT_H

#include "init.h"

void boardInit(struct Board *);
void bagInit(struct Bag *);
void playerInit(struct Player *, struct Bag *);
void moveInit(struct Move *);
bool dictInit(struct Dict *, const char *);
void dictQuit(struct Dict *);

#endif

