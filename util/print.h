#ifndef UTIL_PRINT_H
#define UTIL_PRINT_H

#include "core.h"

void		printWord(struct word*);
void		printDict(struct dict*);
void		printTile(struct tile*);
void		printBag(struct bag*);
void		printAction(struct action*);
void		printScore(struct game*);
void		printBoard(struct board*);
void		printRack(struct player*);
void		printPlace(struct movePlace*);
void		printActionErr(ActionErrType);


#endif

