#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "common.h"


void board_init(board_t *b)
{
	assert(BOARD_X == 15);
	assert(BOARD_Y == 15);
	for (int y = 0; y < BOARD_Y; y++)
		for (int x = 0; x < BOARD_X; x++) {
			b->tile[y][x].type = TILE_NONE;
			b->sq[y][x] = SQ_NORMAL;
			if ((x == y) || (BOARD_Y - y - 1 == x))
				b->sq[y][x] = SQ_DBL_WRD;
			if ((y % 7 == 0) && (x % 7 == 0))
				b->sq[y][x] = SQ_TRP_WRD;
			if ((x % 4 == 1) && (y % 4 == 1))
				b->sq[y][x] = SQ_TRP_LET;
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

	b->sq[1][1] = SQ_DBL_WRD;
	b->sq[13][1] = SQ_DBL_WRD;
	b->sq[1][13] = SQ_DBL_WRD;
	b->sq[13][13] = SQ_DBL_WRD;
	b->sq[7][7] = SQ_FREE;

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
	b->sq[8][6] = SQ_DBL_LET;
}


void bag_shake(bag_t *b, int offset)
{
	int val[BAG_SIZE];
	srand(offset);
	for (int i = 0; i < BAG_SIZE; i++)
		val[i] = rand();
	/* quick and dirty, bubble sort */
	int i = b->head;
	while (i != b->tail) {
		int j = i;
		while (j != b->tail) {
			if (val[i] > val[j]) {
				tile_t tmp = b->tile[i];
				int tmp_v = val[i];
				val[i] = val[j];
				val[j] = tmp_v;
				b->tile[i] = b->tile[j];
				b->tile[j] = tmp;
			}
			j++;
			j %= BAG_SIZE;
		}
		i++;
		i %= BAG_SIZE;
	}
}

void bag_init(bag_t *b)
{
	int i, j, k;

	static const int tileLetterNum[LETTER_COUNT] = {
		9,	/* A */
		2,	/* B */
		2,	/* C */
		4,	/* D */
		12,	/* E */
		2,	/* F */
		3,	/* G */
		2,	/* H */
		9,	/* I */
		1,	/* J */
		1,	/* K */
		4,	/* L */
		2,	/* M */
		6,	/* N */
		8,	/* O */
		2,	/* P */
		1,	/* Q */
		6,	/* R */
		4,	/* S */
		6,	/* T */
		4,	/* U */
		2,	/* V */
		2,	/* W */
		1,	/* X */
		2,	/* Y */
		1	/* Z */
	};

	static const int tileWildNum = 2;

	b->head = 0;
	b->tail = BAG_SIZE - 1;
	for (i = 0; i < tileWildNum; i++) {
		b->tile[i].type = TILE_WILD;
		b->tile[i].letter = LETTER_A;
	}
	for (k = 0, i = 2; k < LETTER_COUNT; k++) {
		for (j = 0; j < tileLetterNum[k]; j++, i++) {
			b->tile[i].type = TILE_LETTER;
			b->tile[i].letter = LETTER_A + k;
		}
	}
	for (; i < BAG_SIZE; i++) {
		b->tile[i].type = TILE_NONE;
	}

	/* short for testing */
	/*b->tail = 20; bagShake(b, 0);*/
	bag_shake(b, time(NULL));
}


void player_init(player_t *p, bag_t *b)
{
	for (int i = 0; i < RACK_SIZE; i++) {
		p->tile[i].type = TILE_NONE;
		p->tile[i].letter = LETTER_A;
	}
	rack_refill(b, p);

	p->score = 0;
	p->active = true;
}

void init_player_human(player_t *p, bag_t *b)
{
	player_init(p, b);
	p->type = PLAYER_HUMAN;
}

void init_player_ai(player_t *p, bag_t *b)
{
	player_init(p, b);
	p->type = PLAYER_AI;
	p->aiShare.shareStart = false;
	p->aiShare.shareEnd = false;
	p->aiShare.loading = 0;
	p->aiShare.action.type = ACTION_INVALID;
}

void move_init(move_t *m)
{
	m->type = MOVE_PLACE;
	m->data.place.num = 4;
	for (int i = 0; i < RACK_SIZE; i++) {
		m->data.place.rackIdx[i] = i;
		m->data.place.coor[i].x = 0;
		m->data.place.coor[i].y = 0;
	}
	m->data.place.coor[0].y = 0;
	m->data.place.coor[1].y = 1;
	m->data.place.coor[2].y = 2;
	m->data.place.coor[3].y = 3;
}


void word_cons(word_t *w, const char *str)
{
	w->len = 0;
	for (int i = 0; str[i] != '\0' && w->len < BOARD_SIZE; i++) {
		char c = toupper(str[i]);
		if (c >= 'A' && c <= 'Z') {
			w->letter[w->len] = LETTER_A + c - 'A';
			w->len++;
		}
	}
}


void swap_sord(word_t *w0, word_t *w1)
{
	const word_t tmp = *w0;
	*w0 = *w1;
	*w1 = tmp;
}


int cmp_word_wrapper(const void *p0, const void *p1)
{
	return cmp_word(p0, p1);
}

bool dict_init(dict_t *d, const char *name)
{
	char buf[BOARD_SIZE + 1];
	FILE *f = fopen(name, "r");
	if (f == NULL)
		return false;
	/* count */
	d->num = 0;
	while (fgets(buf, BOARD_SIZE + 1, f)) {
		word_t w;
		word_cons(&w, buf);
		if (w.len > 1)
			d->num++;
	}
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return false;
	}
	rewind(f);
	assert(d->num > 0);
	/* alloc */
	d->words = malloc(sizeof(word_t) * d->num);
	NOT(d->words);
	for (int i = 0; i < d->num && fgets(buf, BOARD_SIZE + 1, f); i++) {
		word_t w;
		word_cons(&w, buf);
		if (w.len > 1)
			d->words[i] = w;
		else
			i--;
	}
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return false;
	}
	fclose(f);
	/* sort */
	qsort(d->words, d->num, sizeof(word_t), cmp_word_wrapper);
	return true;
}

bool dict_init_count_7(dict_t *d, float *count, float increase,
		       const char *name)
{
	char buf[BOARD_SIZE + 1];
	FILE *f = fopen(name, "r");
	if (f == NULL)
		return false;
	*count += increase;
	/* count */
	d->num = 0;
	while (fgets(buf, BOARD_SIZE + 1, f)) {
		word_t w;
		word_cons(&w, buf);
		if (w.len > 1)
			d->num++;
	}
	*count += increase;
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return false;
	}
	rewind(f);
	*count += increase;
	assert(d->num > 0);
	/* alloc */
	d->words = malloc(sizeof(word_t) * d->num);
	*count += increase;
	NOT(d->words);
	for (int i = 0; i < d->num && fgets(buf, BOARD_SIZE + 1, f); i++) {
		word_t w;
		word_cons(&w, buf);
		if (w.len > 1)
			d->words[i] = w;
		else
			i--;
	}
	*count += increase;
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return false;
	}
	fclose(f);
	/* sort */
	*count += increase;
	qsort(d->words, d->num, sizeof(word_t), cmp_word_wrapper);
	*count += increase;
	return true;
}

void dict_quit(dict_t *dict)
{
	free(dict->words);
}
