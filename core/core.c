#include "common.h"
#include "print.h"

int tileScore(struct Tile *t)
{
	NOT(t);

	if (t->type == TILE_LETTER) {
		switch (t->letter) {
		case LETTER_A: return 1;
		case LETTER_B: return 3;
		case LETTER_C: return 3;
		case LETTER_D: return 2;
		case LETTER_E: return 1;
		case LETTER_F: return 4;
		case LETTER_G: return 2;
		case LETTER_H: return 4;
		case LETTER_I: return 1;
		case LETTER_J: return 8;
		case LETTER_K: return 5;
		case LETTER_L: return 1;
		case LETTER_M: return 3;
		case LETTER_N: return 1;
		case LETTER_O: return 1;
		case LETTER_P: return 3;
		case LETTER_Q: return 10;
		case LETTER_R: return 1;
		case LETTER_S: return 1;
		case LETTER_T: return 1;
		case LETTER_U: return 1;
		case LETTER_V: return 4;
		case LETTER_W: return 4;
		case LETTER_X: return 8;
		case LETTER_Y: return 4;
		case LETTER_Z: return 10;
		default: break;
		}
	}
	return 0;
}

bool canUseDblLet(struct Board *b, struct Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == SQ_DBL_LET;
}

bool canUseTrpLet(struct Board *b, struct Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == SQ_TRP_LET;
}

bool canUseDblWrd(struct Board *b, struct Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == SQ_DBL_WRD;
}
	

bool canUseTrpWrd(struct Board *b, struct Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == SQ_TRP_WRD;
}

int dirScore(struct Board *b, struct Dir *d)
{
	int dw, tw, x, y, s, p, i, t;

	NOT(b);
	NOT(d);

	dw = 0;
	tw = 0;
	x = d->x;
	y = d->y;
	s = 0;
	switch (d->type) {
	case DIR_RIGHT: {
		for (p = 0, i = d->x; i < d->len + d->x; p++, i++) {
			t = tileScore(&b->tile[y][i]);
			t *= canUseDblLet(b, d, p, i, y) ? 2 : 1;
			t *= canUseTrpLet(b, d, p, i, y) ? 3 : 1;
			dw += canUseDblWrd(b, d, p, i, y);
			tw += canUseTrpWrd(b, d, p, i, y);
			s += t;
		}
		break;
	}
	case DIR_DOWN: {
		for (p = 0, i = d->y; i < d->len + d->y; p++, i++) {
			t = tileScore(&b->tile[i][x]);
			t *= canUseDblLet(b, d, p, x, i) ? 2 : 1;
			t *= canUseTrpLet(b, d, p, x, i) ? 3 : 1;
			dw += canUseDblWrd(b, d, p, x, i);
			tw += canUseTrpWrd(b, d, p, x, i);
			s += t;
		} break;
	}
	case DIR_INVALID: /* fall through */
	default: return 0;
	}
	if (dw > 0) {
		s *= dw * 2;
	}
	if (tw > 0) {
		s *= tw * 3;
	}
	return s;
}

int metaPathScore(struct Dir *d, struct Dir *adj, int n, struct Board *b)
{
	int i, s;

	NOT(d);
	NOT(adj);
	NOT(b);

	s = 0;
	if (d->type != DIR_INVALID) {
		s = dirScore(b, d);
		for (i = 0; i < n; i++) {
			if (adj[i].type != DIR_INVALID) {
				s += dirScore(b, adj+i);
			}
		}
	}
	return s;
}

int pathScore(struct Path *p)
{
	int s;
	struct Board *b;

	NOT(p);

	s = 0;
	b = &p->board;

	switch (p->type) {
	case PATH_DOT: {
		if (p->data.dot.right.type == DIR_RIGHT) {
			s = dirScore(b, &p->data.dot.right);
		}
		if (p->data.dot.down.type == DIR_DOWN) {
			s += dirScore(b, &p->data.dot.down);
		}
		break;
	}
	case PATH_HORZ: {
		s = metaPathScore(&p->data.horz.right, p->data.horz.down, BOARD_X, b);
		break;
	}
	case PATH_VERT: {
		s = metaPathScore(&p->data.vert.down, p->data.vert.right, BOARD_Y, b);
		break;
	}
	case PATH_INVALID: /* fall through */
	default: return 0;
	}
	return s;
}

bool bagFull(struct Bag *b)
{
	NOT(b);

	return (b->head + 1) % BAG_SIZE == b->tail;
}

bool bagEmpty(struct Bag *b)
{
	NOT(b);

	return b->head == b->tail;
}

int bagSize(struct Bag *b)
{
	NOT(b);

	if (b->head > b->tail) {
		return b->head - b->tail;
	}
	return b->tail - b->head;
}

struct Tile bagPeek(struct Bag *b)
{
	struct Tile next;

	NOT(b);

	next.type = TILE_NONE;
	if (b->head != b->tail) {
		next = b->tile[b->head];
	}
	return next;
}

void bagDrop(struct Bag *b)
{
	NOT(b);

	b->head ++;
	b->head %= BAG_SIZE;
}

void bagAdd(struct Bag *b, struct Tile t)
{
	NOT(b);

	b->tile[b->tail] = t;
	b->tail ++;
	b->tail %= BAG_SIZE;
}

bool adjustOutOfRange(struct Adjust *a)
{
	int i;

	NOT(a);
	assert(a->type == ADJUST_RACK);

	for (i = 0; i < RACK_SIZE; i++) {
		if (a->data.tile[i].idx < 0) {
			return true;
		}
		if (a->data.tile[i].idx >= RACK_SIZE) {
			return true;
		}
	}
	return false;
}

void adjustSwap(struct Adjust *a, int i, int j)
{
	struct TileAdjust tmp;

	NOT(a);
	assert(a->type == ADJUST_RACK);
	assert(i >= 0);
	assert(i < RACK_SIZE);
	assert(j >= 0);
	assert(j < RACK_SIZE);

	tmp = a->data.tile[i];
	a->data.tile[i] = a->data.tile[j];
	a->data.tile[j] = tmp;
}

void mkAdjust(struct Adjust *a, struct Player *p)
{
	int i;

	NOT(a);
	NOT(p);

	a->type = ADJUST_RACK;
	for (i = 0; i < RACK_SIZE; i++) {
		a->data.tile[i].type = p->tile[i].type;
		a->data.tile[i].idx = i;
	}
}

bool adjustDuplicateIndex(struct Adjust *a)
{
	int i, j, idx;

	NOT(a);

	assert(a->type == ADJUST_RACK);

	for (i = 0; i < RACK_SIZE; i++) {
		idx = a->data.tile[i].idx;
		assert(idx >= 0);
		assert(idx < RACK_SIZE);
		for (j = i + 1; j < RACK_SIZE; j++) {
			if (idx == a->data.tile[j].idx) {
				return true;
			}
		}
	}
	return false;
}

AdjustErrType fdAdjustErr(struct Adjust *a, struct Player *p)
{
	NOT(a);
	NOT(p);
	
	assert(a->type == ADJUST_RACK);
	
	if (adjustOutOfRange(a)) {
		return ADJUST_ERR_RACK_OUT_OF_RANGE;
	}
	if (adjustDuplicateIndex(a)) {
		return ADJUST_ERR_RACK_DUPLICATE_INDEX;
	}
	return ADJUST_ERR_NONE;
}

void applyAdjust(struct Player *p, struct Adjust *a)
{
	int i, idx;
	struct Tile tile[RACK_SIZE];

	NOT(p);
	NOT(a);
	assert(a->type == ADJUST_RACK);

	for (i = 0; i < RACK_SIZE; i++) {
		idx = a->data.tile[i].idx;
		tile[i] = p->tile[idx];
	}
	memCpy(p->tile, tile, sizeof(tile));
}

CmpType cmpWord(struct Word *w0, struct Word *w1) 
{
	/*
	w0 > w1 -> CMP_GREATER
	w0 < w1 -> CMP_LESS
	w0 == w1 -> CMP_EQUAL
	*/
	int i;

	NOT(w0);
	NOT(w1);

	for (i = 0; ; i++) {
		if (w0->len  > w1->len && i == w1->len) {
			return CMP_GREATER;
		}
		if (w0->len  < w1->len && i == w0->len) {
			return  CMP_LESS;
		}
		if (w0->len == w1->len && i == w1->len) {
			return CMP_EQUAL;
		}
		if (w0->letter[i] == w1->letter[i]) {
			continue;
		}
		if (w0->letter[i]  > w1->letter[i]) {
			return CMP_GREATER;
		}
		if (w0->letter[i]  < w1->letter[i]) {
			return CMP_LESS;
		}
	}
	return CMP_EQUAL;
}

bool wordValid(struct Word *w, struct Dict *d)
{
	long min, max, mid;
	
	NOT(w);
	NOT(d);
	
	min = 0;
	max = d->num;
	mid = d->num / 2;

	while (min <= max) {
		switch (cmpWord(w, &d->words[mid])) {
		case CMP_EQUAL: return true;
		case CMP_GREATER: min = mid + 1; break;
		case CMP_LESS: max = mid - 1; break;
		default: return false; /* Should never arrive here via cmpWord */
		}
		mid = (min + max) / 2;
	}
	return false;
}

bool dirValid(struct Dir *dir, struct Board *b, struct Dict *dict)
{
	int x, y, i;
	struct Word w;

	NOT(dir);
	NOT(b);
	NOT(dict);

	x = dir->x;
	y = dir->y;
	w.len = dir->len;
	switch (dir->type) {
	case DIR_RIGHT: {
		for (i = 0; i < w.len; i++) {
			if (b->tile[y][x + i].type != TILE_NONE) {
				w.letter[i] = b->tile[y][x + i].letter;
			} else {
				return false;
			}
		}
		break;
	}
	case DIR_DOWN: {
		for (i = 0; i < w.len; i++) {
			if (b->tile[y + i][x].type != TILE_NONE) {
				w.letter[i] = b->tile[y + i][x].letter;
			} else {
				return false;
			}
		}
		break;
	}
	case DIR_INVALID: /* fall through */
	default: return false;
	}
	return wordValid(&w, dict);
}

bool pathValid(struct Path *p, struct Dict *d)
{
	int i;

	NOT(p);
	NOT(d);

	switch (p->type) {
	case PATH_DOT: {
		puts("DOT_START");
		if (!(dirValid(&p->data.dot.right, &p->board, d) || dirValid(&p->data.dot.down, &p->board, d))) {
			return false;
		}
		puts("DOT_END");
		break;
	}
	case PATH_HORZ: {
		puts("HORZ_START");
		if (!dirValid(&p->data.horz.right, &p->board, d)) {
			return false;
		}
		for (i = 0; i < BOARD_X; i++) {
			if (p->data.horz.down[i].type == DIR_DOWN &&
			   (!dirValid(&p->data.horz.down[i], &p->board, d))) {
				return false;
			}
		}
		puts("HORZ_END");
		break;
	}
	case PATH_VERT: {
		puts("VERT_START");
		if (!dirValid(&p->data.vert.down, &p->board, d)) {
			return false;
		}
		for (i = 0; i < BOARD_Y; i++) {
			if (p->data.vert.right[i].type == DIR_RIGHT && 
			   (!dirValid(&p->data.vert.right[i], &p->board, d))) {
				return false;
			}
		}
		puts("VERT_END");
		break;
	}
	case PATH_INVALID: /* fall through */
	default: return false;
	}
	return true;
}

bool tilesAdjacent(struct Board *b, struct MovePlace *mp, struct Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		if (p->tile[r].type != TILE_NONE) {
			if (x > 0 && b->tile[y][x - 1].type != TILE_NONE) {
				return true;
			}
			if (y > 0 && b->tile[y - 1][x].type != TILE_NONE) {
				return true;
			}
			if (x < BOARD_X - 1 &&
					b->tile[y][x + 1].type != TILE_NONE) {
				return true;
			}
			if (y < BOARD_Y - 1 &&
					b->tile[y + 1][x].type != TILE_NONE) {
				return true;
			}
		}
			
	}
	return false;
}

bool onFreeSquares(struct Board *b, struct MovePlace *mp, struct Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		if (p->tile[r].type != TILE_NONE && b->sq[y][x] == SQ_FREE) {
			return true;
		}
	}
	return false;
}

bool placeInRange(struct MovePlace *mp)
{
	int i;
	
	NOT(mp);
	
	for (i = 0; i < mp->num; i++) {
		if (mp->rackIdx[i] < 0 || mp->rackIdx[i] >= RACK_SIZE) {
			return false;
		}
		if (mp->coor[i].x < 0 || mp->coor[i].x >= BOARD_X) {
			return false;
		}
		if (mp->coor[i].y < 0 || mp->coor[i].y >= BOARD_Y) {
			return false;
		}
	}
	return true;
}

bool placeOverlap(struct MovePlace *mp)
{
	int i, j;

	NOT(mp);
	
	for (i = 0; i < mp->num; i++) {
		for (j = i + 1; j < mp->num; j++) {
			if (mp->rackIdx[i] == mp->rackIdx[j]) {
				return false;
			}
			if (mp->coor[i].x == mp->coor[j].x &&
			    mp->coor[i].y == mp->coor[j].y) {
				return true;
			}
		}
	}
	return false;
}

bool placeOverlapBoard(struct MovePlace *mp, struct Board *b)
{
	int i, x, y;

	NOT(mp);
	NOT(b);
	
	for (i = 0; i < mp->num; i++) {
		x = mp->coor[i].x;
		y = mp->coor[i].y;
		if (b->tile[y][x].type != TILE_NONE) {
			return true;
		}
	}
	return false;
}

bool placeRackExist(struct MovePlace *mp, struct Player *p)
{
	int i, r;

	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		if (p->tile[r].type == TILE_NONE) {
			return false;
		}
	}
	return true;
}

void cpyRackBoard(struct Board *b, struct MovePlace *mp, struct Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		memCpy(&b->tile[y][x], &p->tile[r], sizeof(struct Tile));
	}
}

bool isHorz(struct Action *a, struct Move *m)
{
	int i, min, max, y;
	struct Coor *coor;
	struct Board *b;

	NOT(a);
	NOT(m);

	b = &a->data.place.path.board;
	y = m->data.place.coor[0].y;
	min = max = m->data.place.coor[0].x;
	if (m->data.place.num < 2) {
		return false;
	}
	for (i = 1; i < m->data.place.num; i++) {
		coor = &m->data.place.coor[i];
		if (y != coor->y) {
			return false;
		}
		if (min < coor->x) {
			min = coor->x;
		}
		if (max > coor->x) {
			max = coor->x;
		}
	}
	for (i = min; i <= max; i++) {
		if (b->tile[y][i].type == TILE_NONE) {
			return false;
		}
	}
	return true;
}

bool isVert(struct Action *a, struct Move *m)
{
	int i, min, max, x;
	struct Board *board;
	struct Coor *coor;
	
	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	board = &a->data.place.path.board;
	min = max = m->data.place.coor[0].y;

	if (m->data.place.num < 2) {
		return false;
	}
	for (i = 1; i < m->data.place.num; i++) {
		coor = &m->data.place.coor[i];
		if (x != coor->x) {
			return false;
		}
		if (min > coor->y) {
			min = coor->y;
		}
		if (max < coor->y) {
			max = coor->y;
		}
	}
	for (i = min; i <= max; i++) {
		if (board->tile[i][x].type == TILE_NONE) {
			return false;
		}
	}
	return true;
}

void mkRight(struct Dir *d, int x, int y, struct Board *b)
{
	int i;

	NOT(d);
	NOT(b);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	d->type = DIR_RIGHT;
	d->y = y;
	memSet(d->pos, 0, sizeof(int) * BOARD_SIZE);
	d->pos[x] = 1;

	for (i = x; i >= 0 && b->tile[y][i].type != TILE_NONE; i--) {
		d->x = i;
	}
	for (i = x; i < BOARD_X && b->tile[y][i].type != TILE_NONE; i++) {
		d->len = i;
	}
	d->len -= d->x - 1;
	if (d->len == 1) {
		 d->type = DIR_INVALID;
	}
}

void mkDown(struct Dir *d, int x, int y, struct Board *b)
{
	int i;

	NOT(d);
	NOT(b);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	d->type = DIR_DOWN;
	d->x = x;
	memSet(d->pos, 0, sizeof(int) * BOARD_SIZE);
	d->pos[y] = 1;
	for (i = y; i >= 0 && b->tile[i][x].type != TILE_NONE; i--) {
		d->y = i;
	}
	for (i = y; i < BOARD_Y && b->tile[i][x].type != TILE_NONE; i++) {
		d->len = i;
	}
	d->len -= d->y - 1;
	if (d->len == 1) {
		 d->type = DIR_INVALID;
	}
}

void mkDot(struct Action *a, struct Move *m)
{
	int x, y;
	struct Path *p;
	struct Board *b;
	struct Dir *d;

	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	p = &a->data.place.path; 
	b = &a->data.place.path.board;
	d = NULL;

	p->type = PATH_DOT;

	d = &p->data.dot.right;
	mkRight(d, x, y, b);

	d = &p->data.dot.down;
	mkDown(d, x, y, b);
}

void mkHorz(struct Action *a, struct Move *m)
{
	int i, x, y;
	struct Path *p;
	struct Board *b;
	struct Dir *d;

	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	p = &a->data.place.path; 
	b = &a->data.place.path.board;

	p->type = PATH_HORZ;

	d = &p->data.horz.right;
	mkRight(d, x, y, b);

	for (i = 0; i < m->data.place.num; i++) {
		x = m->data.place.coor[i].x;
		d->pos[x] = 1;
	}
	for (i = 0; i < BOARD_X; i++) {
		p->data.horz.down[i].type = DIR_INVALID;
	}
	for (i = 0; i < m->data.place.num; i++) {
		d = &p->data.horz.down[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mkDown(d, x, y, b);
	}
}

void mkVert(struct Action *a, struct Move *m)
{
	int i, x, y;
	struct Path *path;
	struct Board *b;
	struct Dir *d;

	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	b = &a->data.place.path.board;
	d = NULL;

	path = &a->data.place.path; 
	d = &path->data.vert.down;

	path->type = PATH_VERT;
	mkDown(d, x, y, b);

	for (i = 0; i < m->data.place.num; i++) {
		y = m->data.place.coor[i].y;
		d->pos[y] = 1;
	}
	for (i = 0; i < BOARD_Y; i++) {
		path->data.vert.right[i].type = DIR_INVALID;
	}
	for (i = 0; i < m->data.place.num; i++) {
		d = &path->data.vert.right[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mkRight(d, x, y, b);
	}
}

ActionErrType fdPlaceErr(struct MovePlace *mp,struct Player *p, struct Board *b)
{
	NOT(mp);
	NOT(p);
	NOT(b);
	
	if (!placeInRange(mp)) {
		return ACTION_ERR_PLACE_OUT_OF_RANGE;
	}
	if (placeOverlap(mp)) {
		return ACTION_ERR_PLACE_SELF_OVERLAP;
	}
	if (placeOverlapBoard(mp, b)) {
		return ACTION_ERR_PLACE_BOARD_OVERLAP;
	}
	if (!placeRackExist(mp, p)) {
		return ACTION_ERR_PLACE_INVALID_RACK_ID;
	}
	if (!tilesAdjacent(b, mp, p) && !onFreeSquares(b, mp, p)) {
		return ACTION_ERR_PLACE_INVALID_SQ;
	}
	return ACTION_ERR_NONE;
}

void mkPlace(struct Action *a, struct Game *g, struct Move *m)
{
	int num, i;
	struct Path *path;
	struct Player *player;
	ActionErrType err;

	NOT(a);
	NOT(g);
	NOT(m);

	num = m->data.place.num;
	path = &a->data.place.path;
	player = &g->player[m->playerIdx];
	a->type = ACTION_PLACE;

	a->data.place.num = m->data.place.num;
	for (i = 0; i < num; i++) {
		a->data.place.rackIdx[i] = m->data.place.rackIdx[i];
	}
	err = fdPlaceErr(&m->data.place, player, &g->board);
	if (err != ACTION_ERR_NONE) {
		a->type = ACTION_INVALID;
		a->data.err = err;
		return;
	}

	memCpy(&path->board, &g->board, sizeof(struct Board));
	cpyRackBoard(&path->board, &m->data.place, player);

	assert(num >= 0);
	switch (num) {
	case 0: {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_PLACE_NO_RACK;
		return;
	}
	case 1: {
		mkDot(a, m);
		puts("DOOOOTTT");
		break;
	}
	default: {
		assert(num > 1);
		if (isHorz(a, m)) {
			puts("HORRRZZZ");
			mkHorz(a, m);
		} else { 
			if (isVert(a, m)) {
				puts("VERRTTT");
				mkVert(a, m);
			} else {
				puts("WTFF");
				a->type = ACTION_INVALID;
				a->data.err = ACTION_ERR_PLACE_NO_DIR;
				return;
			}
		}
		break;
	}
	}

	if (!pathValid(path, &g->dict)) {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_PLACE_INVALID_PATH;
		return;
	}
	a->data.place.score = pathScore(path);
}

void mkDiscard(struct Action *a, struct Game *g, struct Move *m)
{
	int i;

	NOT(a);
	NOT(g);
	NOT(m);

	a->type = ACTION_DISCARD;
	a->data.discard.num = m->data.discard.num;
	for (i = 0; i < a->data.discard.num; i++) {
		a->data.discard.rackIdx[i] = m->data.discard.rackIdx[i];
	}
	/* memCpy(&a->data.discard, &m->data.discard, sizeof(struct Discard)); */
}

void mkAction(struct Action *a, struct Game *g, struct Move *m)
{
	NOT(a);
	NOT(g);
	NOT(m);

	a->playerIdx = m->playerIdx;
	switch (m->type) {
	case MOVE_PLACE: mkPlace(a, g, m); break;
	case MOVE_DISCARD: mkDiscard(a, g, m); break;
	case MOVE_SKIP: a->type = ACTION_SKIP; break;
	case MOVE_QUIT: a->type = ACTION_QUIT; break;
	case MOVE_INVALID: /* fall through */
	default: a->type = ACTION_INVALID; break;
	}
}

void rackShift(struct Player *p)
{
	int i, j;

	NOT(p);

	j = 0;
	i = 0;
	while (i < RACK_SIZE) {
		while (p->tile[i].type == TILE_NONE && i < RACK_SIZE) {
			i++;
		}
		if (i == RACK_SIZE) {
			break;
		}
		memCpy(&p->tile[j], &p->tile[i], sizeof(struct Tile));
		i++;
		j++;
	}
	while (j < RACK_SIZE) {
		p->tile[j].type = TILE_NONE;
		j++;
	}
}

void rackRefill(struct Player *p, struct Bag *b)
{
	int i;
	
	NOT(p);
	NOT(b);
	
	for (i = 0; i < RACK_SIZE && !bagEmpty(b); i++) {
		if (p->tile[i].type == TILE_NONE) {
			p->tile[i] = bagPeek(b);
			bagDrop(b);
		}
	}
}

bool applyAction(struct Game *g, struct Action *a)
{
	int id, i, r;

	NOT(g);
	NOT(a);

	id = a->playerIdx;
	if (id != g->turn) {
		return false;
	}
	switch (a->type) {
	case ACTION_PLACE: {
		memCpy(&g->board, &a->data.place.path.board,
				sizeof(struct Board));
		g->player[id].score += a->data.place.score;
		for (i = 0; i < a->data.place.num; i++) {
			r = a->data.place.rackIdx[i];
			g->player[id].tile[r].type = TILE_NONE;
		}
		rackRefill(&g->player[id], &g->bag);
		rackShift(&g->player[id]);
		break;
	}
	case ACTION_DISCARD: {
		for (i = 0; i < a->data.discard.num; i++) {
			r = a->data.discard.rackIdx[i];
			g->player[id].tile[r].type = TILE_NONE;
		}
		rackRefill(&g->player[id], &g->bag);
		rackShift(&g->player[id]);
		g->player[id].score -= a->data.discard.num;
		if (g->player[id].score < 0) {
			g->player[id].score = 0;
		}
		break;
	}
	case ACTION_SKIP: {
		break;
	}
	case ACTION_QUIT: {
		g->player[id].active = false;
		break;
	}
	case ACTION_INVALID: /* fall through */
	default: return false;
	}
	return true;
}

void rmRackTile(struct Player *p, int *rackIdx, int n)
{
	int i;

	NOT(p);
	NOT(rackIdx);

	for (i = 0; i < n; i++) {
		p->tile[rackIdx[i]].type = TILE_NONE;
	}
}

void nextTurn(struct Game *g)
{
	NOT(g);

	do {
		g->turn ++;
		g->turn %= g->playerNum;
	} while (!g->player[g->turn].active);
	printf("turn: %d\n", g->turn);
}

void moveClr(struct Move *m)
{
	NOT(m);

	memSet(m, 0, sizeof(struct Move));
	m->type = MOVE_INVALID;
}

void actionClr(struct Action *a)
{
	NOT(a);

	memSet(a, 0, sizeof(struct Action));
	a->type = ACTION_INVALID;
}

bool endGame(struct Game *g)
{
	int i, j, k;

	NOT(g);

	/* 2 active players min. */	
	j = 0;
	for (i = 0; i < g->playerNum; i++) {
		if (g->player[i].active) {
			j++;
		}
	}
	if (j < 2) {
			return true;
	}
	/* 1 player wth tiles on the rack */
	j = 0;
	for (i = 0; i < g->playerNum; i++) {
		if (g->player[i].active) {
			for (k = 0; k < RACK_SIZE; k++) {
				if (g->player[i].tile[k].type != TILE_NONE) {
					j++;
				}
			}
		}
	}
	if (j == 0) {
		return true;
	}
	return false;
}

int fdWinner(struct Game *g)
{
	int max, idx, i, j;

	NOT(g);

	/* if exist winner then winnerIdx else -1 */

	idx = -1;
	j = 0;

	for (i = 0; i < g->playerNum; i++) {
		if (g->player[i].active) {
			idx = i;
			j++;
		}
	} 
	if (j <= 1) {
		return idx;
	}
	idx = -1;
	max = 0;
	for (i = 0; i < g->playerNum; i++) {
		if (g->player[i].active) {
			if (max < g->player[i].score) {
				idx = i;
				max = g->player[i].score;
			}
		}
	} 
	return idx;
}

bool validRackIdx(int id)
{
	return id >= 0 && id < RACK_SIZE;
}

bool validBoardIdx(struct Coor c)
{
	return c.x >= 0 && c.y >= 0 && c.x < BOARD_X && c.y < BOARD_Y;
}

