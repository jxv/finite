#ifndef UTIL_PRINT_H
#define UTIL_PRINT_H

#include "core.h"
#include "log.h"

void printLetter(LetterType);
void printWord(Word *);
void printDict(Dict *);
void printTile(Tile *);
void printBag(Bag *);
void printAction(Action *);
void printScore(Game *);
void printBoard(Board *);
void printRack(Player *);
void printPlace(MovePlace *);
void printActionErr(ActionErrType);
void printLog(Log *);

#endif

