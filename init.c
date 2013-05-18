#include "core.h"
#include "dict.h"
#include "init.h"


void init_board(struct board *b)
{
	int x, y;
	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			b->tile[y][x].type = TILE_NONE;
			b->sq[y][x] = SQ_NORMAL;
			b->sq[y][x] = SQ_FREE;
		}
	}
}


void init_bag(struct bag *b)
{
	int i;
	b->head = 0;
	b->tail = BAG_SIZE - 1;
	for (i = 0; i < BAG_SIZE; i++) {
		b->tile[i].type = TILE_LETTER;
		b->tile[i].letter = LETTER_A;
	}
}


void init_player(struct player *p)
{
	int i;
	for (i = 0; i < RACK_SIZE; i++) {
		p->tile[i].type = TILE_LETTER;
		p->tile[i].letter = LETTER_A;
	}
	p->tile[0].letter = LETTER_C;
	p->tile[1].letter = LETTER_R;
	p->tile[2].letter = LETTER_A;
	p->tile[3].letter = LETTER_P;
}


void init_move(struct move *m)
{
	int i;
	m->type = MOVE_PLACE;
	m->data.place.num = 4;
	for (i = 0; i < RACK_SIZE; i++) {
		m->data.place.rack_id[i] = i;
		m->data.place.coor[i].x = 0;
		m->data.place.coor[i].y = 0;
	}
	m->data.place.coor[0].y = 0;
	m->data.place.coor[1].y = 1;
	m->data.place.coor[2].y = 2;
	m->data.place.coor[3].y = 3;
}


