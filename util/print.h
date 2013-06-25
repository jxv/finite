#ifndef UTIL_PRINT_H
#define UTIL_PRINT_H

#include "core.h"

void printWord(struct Word *);
void printDict(struct Dict *);
void printTile(struct Tile *);
void printBag(struct Bag *);
void printAction(struct Action *);
void printScore(struct Game *);
void printBoard(struct Board *);
void printRack(struct Player *);
void printPlace(struct MovePlace *);
void printActionErr(ActionErrType);


#endif

