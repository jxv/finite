#include <string.h>

#include "core.h"
#include "print.h"

void printWord(struct Word *w)
{
	char str[BOARD_SIZE];
	int j;

	NOT(w);

	for (j = 0; j < w->len; j++) {
		str[j] = 'A' + w->letter[j] - LETTER_A;
	}
	str[j] = '\0';
	printf("[%s]\n", str);
}

void printDict(struct Dict *d)
{
	int i;

	NOT(d);

	printf("== Size:%ld\n", d->num);
	for (i = 0; i < d->num; i++) {
		printWord(&d->words[i]);
	}
}

void printTile(struct Tile *t)
{
	char c;
	
	NOT(t);

	switch (t->type) {
	case TILE_WILD: c = 'a' + t->letter; break;
	case TILE_LETTER: c = 'A' + t->letter; break;
	default: return;
	}
	putchar(c);
}

void printBag(struct Bag *b)
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

void printAction(struct Action *a)
{
	NOT(a);

	switch (a->type) {
	case ACTION_INVALID: printf("action invalid\n"); break;
	case ACTION_PLACE: {
		printf("action place\n");
		switch (a->data.place.path.type) {
		case PATH_DOT: printf("path_dot\n"); break;
		case PATH_HORZ: printf("path_horz\n");break;
		case PATH_VERT: printf("path_vert\n"); break;
		default: break;
		}
		printf("score: %d\n", a->data.place.score);
		break;
	}
	default: printf("action default?\n"); break;
	}
}

void printScore(struct Game *g)
{
	int i;
	
	NOT(g);
	
	for (i = 0; i < g->playerNum; i++) {
		printf("PLAYER_%d: %d\n", i, g->player[i].score);
	}
}

void printBoard(struct Board *b)
{
	int x, y;
	char c;
	struct Tile *t;
	SqType sq;

	NOT(b);

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			t = &b->tile[y][x];
			sq = b->sq[y][x];
			if (t->type != TILE_NONE) {
				printTile(t);
			} else {
				switch (sq) {
				case SQ_DBL_LET: c = '-'; break;
				case SQ_TRP_LET: c = '='; break;
				case SQ_DBL_WRD: c = '+'; break;
				case SQ_TRP_WRD: c = '#'; break;
				case SQ_FREE: c = '$'; break;
				case SQ_NORMAL: /* fall through */
				default: c = '.'; break;
				}
				putchar(c);
			}
		}
		printf("\n");
	}
}

void printRack(struct Player *p)
{
	int i;
	char c;
	struct Tile *t;

	NOT(p);

	for (i = 0; i < RACK_SIZE; i++) {
		t = &p->tile[i];
		if (t->type != TILE_NONE) {
			switch(t->type) {
			case TILE_WILD: c = '*'; break;
			case TILE_LETTER: c = 'A' + t->letter; break;
			default: c = ' '; break;
			}
			printf("(%d:%c)", i, c);
		}
	}
}

void printPlace(struct MovePlace *mp)
{
	int i;
	
	NOT(mp);

	for (i = 0; i < mp->num; i++) {
		printf("(%d,%d,%d) ", mp->coor[i].x, mp->coor[i].y, mp->rackIdx[i]);
	}
	printf("\n");
}

void printActionErr(ActionErrType err)
{
	switch (err) {
	case ACTION_ERR_PLACE_OUT_OF_RANGE: printf("[err: out of range]"); break;
	case ACTION_ERR_PLACE_SELF_OVERLAP: printf("[err: self overlap]"); break;
	case ACTION_ERR_PLACE_BOARD_OVERLAP: printf("[err: board overlap]"); break;
	case ACTION_ERR_PLACE_INVALID_RACK_ID: printf("[err: invalid rack id]"); break;
	case ACTION_ERR_PLACE_INVALID_SQ: printf("[err: place on free sq. or adjacent to a tile]"); break;
	case ACTION_ERR_PLACE_NO_RACK: printf("[err: no tiles placed on the board]"); break;
	case ACTION_ERR_PLACE_NO_DIR: printf("[err: tiles don't form a continuous line]"); break;
	case ACTION_ERR_PLACE_INVALID_PATH: printf("[err: invalid path, misspelled words?]"); break;
	case ACTION_ERR_NONE: /* fall through */
	default: break;
	}
}

