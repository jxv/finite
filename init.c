#include "core.h"
#include "dict.h"
#include "init.h"


void init_board(struct board *b)
{
	int x, y;

	NOT(b);

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			b->tile[y][x].type = TILE_NONE;
			b->sq[y][x] = SQ_NORMAL;
			if ((x == y) || (BOARD_Y - y - 1 == x)) {
				b->sq[y][x] = SQ_DBL_WRD;
			}
			if ((y % 7 == 0) && (x % 7 == 0)) {
				b->sq[y][x] = SQ_TRP_WRD;
			}
			if ((x % 4 == 1) && (y % 4 == 1)) {
				b->sq[y][x] = SQ_TRP_LET;
			}
			
		}
	}
	b->sq[1][1] = SQ_DBL_WRD;
	b->sq[13][1] = SQ_DBL_WRD;
	b->sq[1][13] = SQ_DBL_WRD;
	b->sq[13][13] = SQ_DBL_WRD;
	b->sq[7][7] = SQ_FREE;

	b->sq[14][3] = SQ_DBL_LET;
	b->sq[3][14] = SQ_DBL_LET;
	b->sq[0][3] = SQ_DBL_LET;
	b->sq[3][0] = SQ_DBL_LET;
	b->sq[0][11] = SQ_DBL_LET;
	b->sq[11][0] = SQ_DBL_LET;
	b->sq[14][11] = SQ_DBL_LET;
	b->sq[11][14] = SQ_DBL_LET;
	b->sq[2][8] = SQ_DBL_LET;
	b->sq[8][2] = SQ_DBL_LET;
	b->sq[2][6] = SQ_DBL_LET;
	b->sq[6][2] = SQ_DBL_LET;
	b->sq[3][7] = SQ_DBL_LET;
	b->sq[7][3] = SQ_DBL_LET;
	b->sq[8][2] = SQ_DBL_LET;
	b->sq[2][8] = SQ_DBL_LET;
	b->sq[12][6] = SQ_DBL_LET;
	b->sq[6][12] = SQ_DBL_LET;
	b->sq[11][7] = SQ_DBL_LET;
	b->sq[7][11] = SQ_DBL_LET;
	b->sq[12][8] = SQ_DBL_LET;
	b->sq[8][12] = SQ_DBL_LET;
	b->sq[8][8] = SQ_DBL_LET;
	b->sq[6][6] = SQ_DBL_LET;
	b->sq[6][8] = SQ_DBL_LET;
	b->sq[8][6] = SQ_DBL_LET;
}


void init_bag(struct bag *b)
{
	int i;

	NOT(b);

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

	NOT(p);

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
	
	NOT(m);

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


