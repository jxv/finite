#ifndef UTIL_PRINT_H
#define UTIL_PRINT_H

#include "core.h"
#include "log.h"

void printLetter(letter_t);
void printWord(const word_t *);
void printDict(const dict_t *);
void printTile(const tile_t *);
void printBag(const bag_t *);
void printAction(const action_t *);
void printScore(const game_t *);
void printBoard(const board_t *);
void printRack(const player_t *);
void printPlace(const move_place_t *);
void printActionErr(action_err_t);
void printLog(const log_t *);

#endif
