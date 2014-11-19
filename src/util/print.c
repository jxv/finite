#include <string.h>

#include "core.h"
#include "print.h"

void printLetter(LetterType l)
{
	assert(l >= letterA);
	assert(l <= letterZ);

	putchar('A' + l - letterA);
}

void printWord(word_t *w)
{
	char str[BOARD_SIZE];
	int j;

	NOT(w);

	for (j = 0; j < w->len; j++) {
		str[j] = 'A' + w->letter[j] - letterA;
	}
	str[j] = '\0';
	printf("[%s]\n", str);
}

void printDict(dict_t *d)
{
	int i;

	NOT(d);

	printf("== Size:%ld\n", d->num);
	for (i = 0; i < d->num; i++) {
		printWord(&d->words[i]);
	}
}

void printTile(tile_t *t)
{
	char c;
	
	NOT(t);

	switch (t->type) {
	case tileWild: c = 'a' + t->letter; break;
	case tileLetter: c = 'A' + t->letter; break;
	default: return;
	}
	putchar(c);
}

void printBag(bag_t *b)
{
	int i;

	NOT(b);

	i = b->head;
	while (i != b->tail) {
		printTile(&b->tile[i]);
		i++;
		i %= BAG_SIZE;
	}
	putchar('\n');
}

void printAction(action_t *a)
{
	NOT(a);

	switch (a->type) {
	case actionInvalid: printf("action invalid\n"); break;
	case actionPlace: {
		printf("action place\n");
		switch (a->data.place.path.type) {
		case pathDot: printf("path_dot\n"); break;
		case pathHorz: printf("path_horz\n");break;
		case pathVert: printf("path_vert\n"); break;
		default: break;
		}
		printf("score: %d\n", a->data.place.score);
		break;
	}
	default: printf("action default?\n"); break;
	}
}

void printScore(game_t *g)
{
	int i;
	
	NOT(g);
	
	for (i = 0; i < g->playerNum; i++) {
		printf("PLAYER_%d: %d\n", i, g->player[i].score);
	}
}

void printBoard(board_t *b)
{
	int x, y;
	char c;
	tile_t *t;
	SqType sq;

	NOT(b);

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			t = &b->tile[y][x];
			sq = b->sq[y][x];
			if (t->type != tileNone) {
				printTile(t);
			} else {
				switch (sq) {
				case sqDblLet: c = '-'; break;
				case sqTrpLet: c = '='; break;
				case sqDblWrd: c = '+'; break;
				case sqTrpWrd: c = '#'; break;
				case sqFree: c = '$'; break;
				case sqNormal: /* fall through */
				default: c = '.'; break;
				}
				putchar(c);
			}
		}
		printf("\n");
	}
}

void printRack(player_t *p)
{
	for (int i = 0; i < RACK_SIZE; i++) {
		tile_t *t = &p->tile[i];
		if (t->type != tileNone) {
                        char c;
			switch(t->type) {
			case tileWild:
                                c = '*';
                                break;
			case tileLetter:
                                c = 'A' + t->letter;
                                break;
			default:
                                c = ' ';
                                break;
			}
			printf("(%d:%c)", i, c);
		}
	}
}

void printPlace(move_place_t *mp)
{
	for (int i = 0; i < mp->num; i++) {
		printf("(%d,%d,%d) ", mp->coor[i].x, mp->coor[i].y, mp->rackIdx[i]);
	}
	printf("\n");
}

void printActionErr(action_err_t err)
{
	switch (err) {
	case actionErrUnknown: puts("[err: unknown]"); break;
	case actionErrPlaceOutOfRange: puts("[err: out of range]"); break;
	case actionErrPlaceSelfOverlap: puts("[err: self overlap]"); break;
	case actionErrPlaceBoardOverlap: puts("[err: board overlap]"); break;
	case actionErrPlaceInvalidRackId: puts("[err: invalid rack id]"); break;
	case actionErrPlaceInvalidSq: puts("[err: place on free sq. or adjacent to a tile]"); break;
	case actionErrPlaceNoRack: puts("[err: no tiles placed on the board]"); break;
	case actionErrPlaceNoDir: puts("[err: tiles don't form a continuous line]"); break;
	case actionErrPlaceInvalidPath: puts("[err: invalid path]"); break;
	case actionErrPlaceInvalidWord: puts("[err: misspelled word(s)]"); break;
	case actionErrDiscardRule: puts("[err: discard rule]"); break;
	case actionErrSkipRule: puts("[err: skip rule]"); break;
	case actionErrQuitRule: puts("[err: quit rule]"); break;
	case actionErrNone: /* fall through */
	default: break;
	}
}

void printLog(log_t *l)
{
	NOT(l);

	switch (l->type) {
	case actionInvalid: {
		switch (l->data.err) {
		case actionErrNone: break;
		default: break;
		}
		break;
	}
	case actionPlace: {
		break;
	}
	case actionDiscard: {
		break;
	}
	case actionSkip: {
		break;
	}
	case actionQuit: {
		break;
	}
	default:
                break;
	}
}

