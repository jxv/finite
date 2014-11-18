#include "common.h"
#include "print.h"

int defaultLetterScore(LetterType l)
{
	switch (l) {
	case letterA: return 1;
	case letterB: return 3;
	case letterC: return 3;
	case letterD: return 2;
	case letterE: return 1;
	case letterF: return 4;
	case letterG: return 2;
	case letterH: return 4;
	case letterI: return 1;
	case letterJ: return 8;
	case letterK: return 5;
	case letterL: return 1;
	case letterM: return 3;
	case letterN: return 1;
	case letterO: return 1;
	case letterP: return 3;
	case letterQ: return 10;
	case letterR: return 1;
	case letterS: return 1;
	case letterT: return 1;
	case letterU: return 1;
	case letterV: return 4;
	case letterW: return 4;
	case letterX: return 8;
	case letterY: return 4;
	case letterZ: return 10;
	default: break;
	}
	return 0;
}

int tileScore(Tile *t)
{
	NOT(t);

	if (t->type == tileLetter) {
		return defaultLetterScore(t->letter);
	}
	return 0;
}

bool canUseDblLet(Board *b, Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == sqDblLet;
}

bool canUseTrpLet(Board *b, Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == sqTrpLet;
}

bool canUseDblWrd(Board *b, Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == sqDblWrd;
}
	

bool canUseTrpWrd(Board *b, Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == sqTrpWrd;
}

int dirScore(Board *b, Dir *d)
{
	int dw, tw, x, y, s, i, t;

	NOT(b);
	NOT(d);

	dw = 0;
	tw = 0;
	x = d->x;
	y = d->y;
	s = 0;
	switch (d->type) {
	case dirRight: {
		for (i = d->x; i < d->len + d->x; i++) {
			t = tileScore(&b->tile[y][i]);
			t *= canUseDblLet(b, d, i, i, y) ? 2 : 1;
			t *= canUseTrpLet(b, d, i, i, y) ? 3 : 1;
			dw += canUseDblWrd(b, d, i, i, y);
			tw += canUseTrpWrd(b, d, i, i, y);
			s += t;
		}
		break;
	}
	case dirDown: {
		for (i = d->y; i < d->len + d->y; i++) {
			t = tileScore(&b->tile[i][x]);
			t *= canUseDblLet(b, d, i, x, i) ? 2 : 1;
			t *= canUseTrpLet(b, d, i, x, i) ? 3 : 1;
			dw += canUseDblWrd(b, d, i, x, i);
			tw += canUseTrpWrd(b, d, i, x, i);
			s += t;
		}
		break;
	}
	case dirInvalid: /* fall through */
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

int metaPathScore(Dir *d, Dir *adj, int n, Board *b)
{
	int i, s;

	NOT(d);
	NOT(adj);
	NOT(b);

	s = 0;
	if (d->type != dirInvalid) {
		s = dirScore(b, d);
		for (i = 0; i < n; i++) {
			if (adj[i].type != dirInvalid) {
				s += dirScore(b, &adj[i]);
			}
		}
	}
	return s;
}

int pathScore(Path *p)
{
	int s;
	Board *b;

	NOT(p);

	s = 0;
	b = &p->board;

	switch (p->type) {
	case pathDot: {
		if (p->data.dot.right.type == dirRight) {
			s = dirScore(b, &p->data.dot.right);
		}
		if (p->data.dot.down.type == dirDown) {
			s += dirScore(b, &p->data.dot.down);
		}
		break;
	}
	case pathHorz: {
		s = metaPathScore(&p->data.horz.right, p->data.horz.down, BOARD_X, b);
		break;
	}
	case pathVert: {
		s = metaPathScore(&p->data.vert.down, p->data.vert.right, BOARD_Y, b);
		break;
	}
	case pathInvalid: /* fall through */
	default: return 0;
	}
	return s;
}

bool bagFull(Bag *b)
{
	NOT(b);

	return (b->head + 1) % BAG_SIZE == b->tail;
}

bool bagEmpty(Bag *b)
{
	NOT(b);

	return b->head == b->tail;
}

int bagSize(Bag *b)
{
	NOT(b);

	if (b->head > b->tail) {
		return b->head - b->tail;
	}
	return b->tail - b->head;
}

Tile bagPeek(Bag *b)
{
	Tile next;

	NOT(b);

	next.type = tileNone;
	if (b->head != b->tail) {
		next = b->tile[b->head];
	}
	return next;
}

void bagDrop(Bag *b)
{
	NOT(b);

	b->head++;
	b->head %= BAG_SIZE;
}

void bagAdd(Bag *b, Tile t)
{
	NOT(b);

	b->tile[b->tail] = t;
	b->tail++;
	b->tail %= BAG_SIZE;
}

bool adjustOutOfRange(Adjust *a)
{
	int i;

	NOT(a);
	assert(a->type == adjustRack);

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

void adjustSwap(Adjust *a, int i, int j)
{
	TileAdjust tmp;

	NOT(a);
	assert(a->type == adjustRack);
	VALID_RACK_SIZE(i);
	VALID_RACK_SIZE(j);

	tmp = a->data.tile[i];
	a->data.tile[i] = a->data.tile[j];
	a->data.tile[j] = tmp;
}

void mkAdjust(Adjust *a, Player *p)
{
	int i;

	NOT(a);
	NOT(p);

	a->type = adjustRack;
	for (i = 0; i < RACK_SIZE; i++) {
		a->data.tile[i].type = p->tile[i].type;
		a->data.tile[i].idx = i;
	}
}

bool adjustDuplicateIndex(Adjust *a)
{
	int i, j, idx;

	NOT(a);

	assert(a->type == adjustRack);

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

AdjustErrType fdAdjustErr(Adjust *a, Player *p)
{
	NOT(a);
	NOT(p);
	
	assert(a->type == adjustRack);
	
	if (adjustOutOfRange(a)) {
		return adjustErrRackOutOfRange;
	}
	if (adjustDuplicateIndex(a)) {
		return adjustErrRackDuplicateIdx;
	}
	return adjustErrNone;
}

void applyAdjust(Player *p, Adjust *a)
{
	int i, idx;
	Tile tile[RACK_SIZE];

	NOT(p);
	NOT(a);
	assert(a->type == adjustRack);

	for (i = 0; i < RACK_SIZE; i++) {
		idx = a->data.tile[i].idx;
		tile[i] = p->tile[idx];
	}
	memCpy(p->tile, tile, sizeof(tile));
}

CmpType cmpWord(Word *w0, Word *w1) 
{
	/*
	w0 > w1 -> cmpGreater
	w0 < w1 -> cmpLess
	w0 == w1 -> cmpEqual
	*/
	int i;

	NOT(w0);
	NOT(w1);

	for (i = 0; ; i++) {
		if (w0->len  > w1->len && i == w1->len) {
			return cmpGreater;
		}
		if (w0->len  < w1->len && i == w0->len) {
			return  cmpLess;
		}
		if (w0->len == w1->len && i == w1->len) {
			return cmpEqual;
		}
		if (w0->letter[i] == w1->letter[i]) {
			continue;
		}
		if (w0->letter[i]  > w1->letter[i]) {
			return cmpGreater;
		}
		if (w0->letter[i]  < w1->letter[i]) {
			return cmpLess;
		}
	}
	return cmpEqual;
}

bool wordValid(Word *w, Dict *d)
{
	long min, max, mid;
	
	NOT(w);
	NOT(d);
	
	min = 0;
	max = d->num;
	mid = d->num / 2;

	while (min <= max) {
		switch (cmpWord(w, &d->words[mid])) {
		case cmpEqual: return true;
		case cmpGreater: min = mid + 1; break;
		case cmpLess: max = mid - 1; break;
		default: return false; /* Should never arrive here via cmpWord */
		}
		mid = (min + max) / 2;
	}
	return false;
}

bool dirValid(Dir *dir, Board *b, Dict *dict, Word *w)
{
	int x, y, i;

	NOT(dir);
	NOT(b);
	NOT(dict);

	x = dir->x;
	y = dir->y;
	w->len = dir->len;
	switch (dir->type) {
	case dirRight: {
		for (i = 0; i < w->len; i++) {
			if (b->tile[y][x + i].type != tileNone) {
				w->letter[i] = b->tile[y][x + i].letter;
			} else {
				return false;
			}
		}
		break;
	}
	case dirDown: {
		for (i = 0; i < w->len; i++) {
			if (b->tile[y + i][x].type != tileNone) {
				w->letter[i] = b->tile[y + i][x].letter;
			} else {
				return false;
			}
		}
		break;
	}
	case dirInvalid: /* fall through */
	default: return false;
	}
	return true;
}

bool pathValid(Path *p, Dict *d, bool (*rule)(Word *, PathType, DirType))
{
	int i;
	Word w0, w1;
	bool result;

	NOT(p);
	NOT(d);

	result = true;
	switch (p->type) {
	case pathDot: {
		bool a0, a1, b0, b1;
		a0 = dirValid(&p->data.dot.right, &p->board, d, &w0);
		b0 = dirValid(&p->data.dot.down, &p->board, d, &w1);

		a0 = a0 ? wordValid(&w0, d) : a0;
		b0 = b0 ? wordValid(&w1, d) : b0;

		a0 = a0 && rule ? rule(&w0, p->type, dirRight) : a0;
		b0 = b0 && rule ? rule(&w1, p->type, dirDown) : b0;

		a1 = p->data.dot.right.len > 1;
		b1 = p->data.dot.down.len > 1;

		if (!((a0 && !(b0 || b1)) || (b0 && !(a0 || a1)) || (a0 && b0))) {
			result = false;	
		}
		break;
	}
	case pathHorz: {
		if (!dirValid(&p->data.horz.right, &p->board, d, &w0)) {
			result = false;
		} else if (!wordValid(&w0, d)) {
			result = false;
		} else if (rule && !rule(&w0, p->type, dirRight)) {
			result = false;
		}
		for (i = 0; i < BOARD_X; i++) {
			if (p->data.horz.down[i].type == dirDown) {
				if (!dirValid(&p->data.horz.down[i], &p->board, d, &w1)) {
					result = false;
				} else if (w1.len > 1 && !wordValid(&w1, d)) {
					result = false;
				} else if (w1.len > 1 && rule && !rule(&w1, p->type, dirDown)) {
					result = false;
				}
			}
		}
		break;
	}
	case pathVert: {
		if (!dirValid(&p->data.vert.down, &p->board, d, &w0)) {
			result = false;
		} else if (!wordValid(&w0, d)) {
			result = false;
		} else if (rule && !rule(&w0, p->type, dirDown)) {
			result = false;
		}
		for (i = 0; i < BOARD_Y; i++) {
			if (p->data.vert.right[i].type == dirRight) {
				if(!dirValid(&p->data.vert.right[i], &p->board, d, &w1)) {
					result = false;
				} else if (w1.len > 1 && !wordValid(&w1, d)) {
					result = false;
				} else if (w1.len > 1 && rule && !rule(&w1, p->type, dirRight)) {
					result = false;
				}
			}
		}
		break;
	}
	case pathInvalid: /* fall through */
	default: result = false; break; 
	}
	return result; 
}

/*
PathErrType pathValidWithErr(Path *p, Dict *d)
{
	int i;
	Word w0, w1;

	NOT(p);
	NOT(d);

	switch (p->type) {
	case pathDot: {
		bool a0, a1, b0, b1;
		a0 = dirValid(&p->data.dot.right, &p->board, d, &w0);
		b0 = dirValid(&p->data.dot.down, &p->board, d, &w1);
		a0 = a0 ? wordValid(&w0, d) : a0;
		b0 = b0 ? wordValid(&w1, d) : b0;
		a1 = p->data.dot.right.len > 1;
		b1 = p->data.dot.down.len > 1;

		if (!((a0 && !(b0 || b1)) || (b0 && !(a0 || a1)) || (a0 && b0))) {
			return false;
		}
		break;
	}
	case pathHorz: {
		if (!dirValid(&p->data.horz.right, &p->board, d, &w0)) {
			return pathErrNonCont;
		}
		if (!wordValid(&w0, d)) {
			return pathErrInvalidWord;
		}
		for (i = 0; i < BOARD_X; i++) {
			if (p->data.horz.down[i].type == dirDown) {
				if (!dirValid(&p->data.horz.down[i], &p->board, d, &w1)) {
					return pathErrNonCont;
				}
				if (!wordValid(&w1, d)) {
					return pathErrInvalidWord;
				}
			}
		}
		break;
	}
	case pathVert: {
		if (!dirValid(&p->data.vert.down, &p->board, d, &w0)) {
			return pathErrNonCont; 
		}
		if (!wordValid(&w0, d)) {
			return pathErrInvalidWord;
		}
		for (i = 0; i < BOARD_Y; i++) {
			if (p->data.vert.right[i].type == dirRight) {
				if(!dirValid(&p->data.vert.right[i], &p->board, d, &w1)) {
					return pathErrNonCont;
				}
				if (!wordValid(&w1, d)) {
					return pathErrInvalidWord;
				}
			}
		}
		break;
	}
	case pathInvalid:
	default: return pathErrInvalidPath;
	}
	return pathErrNone;
}
*/

bool tilesAdjacent(Board *b, MovePlace *mp, Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		if (p->tile[r].type != tileNone) {
			if (x > 0 && b->tile[y][x - 1].type != tileNone) {
				return true;
			}
			if (y > 0 && b->tile[y - 1][x].type != tileNone) {
				return true;
			}
			if (x < BOARD_X - 1 &&
					b->tile[y][x + 1].type != tileNone) {
				return true;
			}
			if (y < BOARD_Y - 1 &&
					b->tile[y + 1][x].type != tileNone) {
				return true;
			}
		}
			
	}
	return false;
}

bool onFreeSquares(Board *b, MovePlace *mp, Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		if (p->tile[r].type != tileNone && b->sq[y][x] == sqFree) {
			return true;
		}
	}
	return false;
}

bool onVowels(Board *b, MovePlace *mp, Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		if (b->sq[y][x] == sqNoVowel && p->tile[r].type != tileNone && constant(p->tile[r].letter)) {
			return false;
		}
	}
	return true;
}

bool onBlocks(Board *b, MovePlace *mp, Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		if (b->sq[y][x] == sqBlock && p->tile[r].type != tileNone) {
			return true;
		}
	}
	return false;
}

bool placeInRange(MovePlace *mp)
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

bool placeOverlap(MovePlace *mp)
{
	for (int i = 0; i < mp->num && i < RACK_SIZE; i++) {
		for (int j = i + 1; j < mp->num && j < RACK_SIZE; j++) {
			if (mp->rackIdx[i] == mp->rackIdx[j])
				return false;
			if (mp->coor[i].x == mp->coor[j].x &&
			                mp->coor[i].y == mp->coor[j].y)
				return true;
		}
	}
	return false;
}

bool placeOverlapBoard(MovePlace *mp, Board *b)
{
	int i, x, y;

	NOT(mp);
	NOT(b);
	
	for (i = 0; i < mp->num; i++) {
		x = mp->coor[i].x;
		y = mp->coor[i].y;
		if (b->tile[y][x].type != tileNone) {
			return true;
		}
	}
	return false;
}

bool placeRackExist(MovePlace *mp, Player *p)
{
	int i, r;

	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		if (p->tile[r].type == tileNone) {
			return false;
		}
	}
	return true;
}

void cpyRackBoard(Board *b, MovePlace *mp, Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		memCpy(&b->tile[y][x], &p->tile[r], sizeof(Tile));
	}
}

bool isHorz(Action *a, Move *m)
{
	int i, min, max, y;
	Coor *coor;
	Board *b;

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
		if (b->tile[y][i].type == tileNone) {
			return false;
		}
	}
	return true;
}

bool isVert(Action *a, Move *m)
{
	int i, min, max, x;
	Board *board;
	Coor *coor;
	
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
		if (board->tile[i][x].type == tileNone) {
			return false;
		}
	}
	return true;
}

void mkRight(Dir *d, int x, int y, Board *b)
{
	int i;

	NOT(d);
	NOT(b);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	d->type = dirRight;
	d->x = x;
	d->y = y;
	memSet(d->pos, false, sizeof(bool) * BOARD_SIZE);
	d->pos[x] = true;

	for (i = x; i >= 0 && b->tile[y][i].type != tileNone; i--) {
		d->x = i;
	}

	for (i = x; i < BOARD_X && b->tile[y][i].type != tileNone; i++) {
		d->len = i;
	}
	d->len -= d->x - 1;

	/* a word cannot be 1 letter long */
	if (d->len == 1) {
		 d->type = dirInvalid;
	}
}

void mkDown(Dir *d, int x, int y, Board *b)
{
	int i;

	NOT(d);
	NOT(b);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	d->type = dirDown;
	d->x = x;
	d->y = y;
	memSet(d->pos, false, sizeof(bool) * BOARD_SIZE);
	d->pos[y] = true;

	for (i = y; i >= 0 && b->tile[i][x].type != tileNone; i--) {
		d->y = i;
	}

	for (i = y; i < BOARD_Y && b->tile[i][x].type != tileNone; i++) {
		d->len = i;
	}
	d->len -= d->y - 1;

	/* a word cannot be 1 letter long */
	if (d->len == 1) {	
		d->type = dirInvalid;
	}
}

void mkDot(Action *a, Move *m)
{
	int x, y;
	Path *p;
	Board *b;
	Dir *d;

	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	p = &a->data.place.path; 
	b = &a->data.place.path.board;
	d = NULL;

	p->type = pathDot;

	d = &p->data.dot.right;
	mkRight(d, x, y, b);

	d = &p->data.dot.down;
	mkDown(d, x, y, b);
}

void mkHorz(Action *a, Move *m)
{
	int i, x, y;
	Path *p;
	Board *b;
	Dir *d;

	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	p = &a->data.place.path; 
	b = &a->data.place.path.board;

	p->type = pathHorz;

	d = &p->data.horz.right;
	mkRight(d, x, y, b);

	for (i = 0; i < m->data.place.num; i++) {
		x = m->data.place.coor[i].x;
		d->pos[x] = true;
	}
	for (i = 0; i < BOARD_X; i++) {
		p->data.horz.down[i].type = dirInvalid;
	}
	for (i = 0; i < m->data.place.num; i++) {
		d = &p->data.horz.down[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mkDown(d, x, y, b);
	}
}

void mkVert(Action *a, Move *m)
{
	int i, x, y;
	Path *path;
	Board *b;
	Dir *d;

	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	b = &a->data.place.path.board;
	d = NULL;

	path = &a->data.place.path; 
	d = &path->data.vert.down;

	path->type = pathVert;
	mkDown(d, x, y, b);

	for (i = 0; i < m->data.place.num; i++) {
		y = m->data.place.coor[i].y;
		d->pos[y] = true;
	}
	for (i = 0; i < BOARD_Y; i++) {
		path->data.vert.right[i].type = dirInvalid;
	}
	for (i = 0; i < m->data.place.num; i++) {
		d = &path->data.vert.right[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mkRight(d, x, y, b);
	}
}

ActionErrType fdPlaceErr(MovePlace *mp,Player *p, Board *b)
{
	NOT(mp);
	NOT(p);
	NOT(b);
	
	if (!placeInRange(mp)) {
		return actionErrPlaceOutOfRange;
	}
	if (placeOverlap(mp)) {
		return actionErrPlaceSelfOverlap;
	}
	if (placeOverlapBoard(mp, b)) {
		return actionErrPlaceBoardOverlap;
	}
	if (!placeRackExist(mp, p)) {
		return actionErrPlaceInvalidRackId;
	}
	if ((!tilesAdjacent(b, mp, p) && !onFreeSquares(b, mp, p)) || !onVowels(b, mp, p) || onBlocks(b, mp, p)) {
		return actionErrPlaceInvalidSq;
	}
	return actionErrNone;
}

bool ruleZ4Char(Word *w, PathType pt, DirType dt)
{
	NOT(w);
	printWord(w);
	putchar('\n');
	return w->len == 4 && w->letter[0] == letterZ;
}

void mkPlace(Action *a, Game *g, Move *m)
{
	int num, i;
	Path *path;
	Player *player;
	ActionErrType err;

	NOT(a);
	NOT(g);
	NOT(m);

	num = m->data.place.num;
	path = &a->data.place.path;
	player = &g->player[m->playerIdx];
	a->type = actionPlace;

	a->data.place.num = m->data.place.num;
	for (i = 0; i < num; i++) {
		a->data.place.rackIdx[i] = m->data.place.rackIdx[i];
	}
	err = fdPlaceErr(&m->data.place, player, &g->board);
	if (err != actionErrNone) {
		a->type = actionInvalid;
		a->data.err = err;
		return;
	}

	memCpy(&path->board, &g->board, sizeof(Board));
	cpyRackBoard(&path->board, &m->data.place, player);

	assert(num >= 0);
	switch (num) {
	case 0: {
		a->type = actionInvalid;
		a->data.err = actionErrPlaceNoRack;
		return;
	}
	case 1: {
		mkDot(a, m);
		break;
	}
	default: {
		assert(num > 1);
		if (isHorz(a, m)) {
			mkHorz(a, m);
		} else { 
			if (isVert(a, m)) {
				mkVert(a, m);
			} else {
				a->type = actionInvalid;
				a->data.err = actionErrPlaceNoDir;
				return;
			}
		}
		break;
	}
	}

	if (!pathValid(path, &g->dict, NULL)) {
		a->type = actionInvalid;
		a->data.err = actionErrPlaceInvalidPath;
		return;
	}
	a->data.place.score = pathScore(path);
}

void mkDiscard(Action *a, Game *g, Move *m)
{
	int i;

	NOT(a);
	NOT(g);
	NOT(m);

	if (m->data.discard.num == 0) {
		a->type = actionInvalid;
		a->data.err = actionErrDiscardEmpty;
		return;
	}

	assert(m->data.discard.num > 0);

	a->type = actionDiscard;
	a->data.discard.score = -m->data.discard.num;
	if (a->data.discard.score + g->player[a->playerIdx].score < 0) {
		a->data.discard.score += g->player[a->playerIdx].score - a->data.discard.score;
	}
	a->data.discard.num = m->data.discard.num;

	for (i = 0; i < a->data.discard.num; i++) {
		a->data.discard.rackIdx[i] = m->data.discard.rackIdx[i];
	}
}

void mkSkip(Action *a, Game *g)
{
	NOT(a);
	NOT(g);
	
	if (g->rule.skip && !g->rule.skip(g)) {
		a->type = actionInvalid;
		a->data.err = actionErrSkipRule;
		return;
	}
	a->type = actionSkip;
}

void mkQuit(Action *a, Game *g)
{
	NOT(a);
	NOT(g);
	
	if (g->rule.quit && !g->rule.quit(g)) {
		a->type = actionInvalid;
		a->data.err = actionErrQuitRule;
		return;
	}
	a->type = actionQuit;
}

void mkAction(Action *a, Game *g, Move *m)
{
	NOT(a);
	NOT(g);
	NOT(m);

	a->playerIdx = m->playerIdx;
	switch (m->type) {
	case movePlace: mkPlace(a, g, m); break;
	case moveDiscard: mkDiscard(a, g, m); break;
	case moveSkip: mkSkip(a, g); break;
	case moveQuit: mkQuit(a, g); break;
	case moveInvalid: /* fall through */
	default: a->type = actionInvalid; break;
	}
}

void rackShift(Player *p)
{
	int i, j;

	NOT(p);

	j = 0;
	i = 0;
	while (i < RACK_SIZE) {
		while (p->tile[i].type == tileNone && i < RACK_SIZE) {
			i++;
		}
		if (i == RACK_SIZE) {
			break;
		}
		p->tile[j] = p->tile[i];
		i++;
		j++;
	}
	while (j < RACK_SIZE) {
		p->tile[j].type = tileNone;
		j++;
	}
}

void rackRefill(Player *p, Bag *b)
{
	int i;
	
	NOT(p);
	NOT(b);

	for (i = 0; i < RACK_SIZE && !bagEmpty(b); i++) {
		if (p->tile[i].type == tileNone) {
			p->tile[i] = bagPeek(b);
			bagDrop(b);
		}
	}
}

bool applyAction(Game *g, Action *a)
{
	int id, i, r;

	NOT(g);
	NOT(a);

	id = a->playerIdx;
	VALID_TILES(g->player[id]);
	if (id != g->turn) {
		return false;
	}
	switch (a->type) {
	case actionPlace: {
		VALID_TILES(g->player[id]);
		memCpy(&g->board, &a->data.place.path.board, sizeof(a->data.place.path.board));
		for (i = 0; i < a->data.place.num; i++) {
			r = a->data.place.rackIdx[i];
			g->player[id].tile[r].type = tileNone;
		}
		VALID_TILES(g->player[id]);
		rackRefill(&g->player[id], &g->bag);
		VALID_TILES(g->player[id]);
		rackShift(&g->player[id]);
		g->player[id].score += a->data.place.score;
		VALID_TILES(g->player[id]);
		break;
	}
	case actionDiscard: {
		for (i = 0; i < a->data.discard.num; i++) {
			r = a->data.discard.rackIdx[i];
			g->player[id].tile[r].type = tileNone;
		}
		rackRefill(&g->player[id], &g->bag);
		rackShift(&g->player[id]);
		g->player[id].score += a->data.discard.score;
		break;
	}
	case actionSkip: {
		break;
	}
	case actionQuit: {
		g->player[id].active = false;
		break;
	}
	case actionInvalid: /* fall through */
	default: return false;
	}

	return true;
}

void rmRackTile(Player *p, int *rackIdx, int n)
{
	int i;

	NOT(p);
	NOT(rackIdx);

	for (i = 0; i < n; i++) {
		p->tile[rackIdx[i]].type = tileNone;
	}
}

void nextTurn(Game *g)
{
	NOT(g);

	do {
		g->turn ++;
		g->turn %= g->playerNum;
	} while (!g->player[g->turn].active);
}

void moveClr(Move *m)
{
	NOT(m);

	memSet(m, 0, sizeof(Move));
	m->type = moveInvalid;
}

void actionClr(Action *a)
{
	NOT(a);

	memSet(a, 0, sizeof(Action));
	a->type = actionInvalid;
}

bool endGame(Game *g)
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
				if (g->player[i].tile[k].type != tileNone) {
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

int fdWinner(Game *g)
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

bool validBoardIdx(Coor c)
{
	return c.x >= 0 && c.y >= 0 && c.x < BOARD_X && c.y < BOARD_Y;
}

int rackCount(Player *p)
{
	int i, count;

	NOT(p);
	
	count = 0;
	for (i = 0; i < RACK_SIZE; i++) {
		if (p->tile[i].type != tileNone) {
			count++;
		}
	}
	return count;
}

int adjustTileCount(Adjust *a)
{
	int i, count;

	NOT(a);
	
	count = 0;
	for (i = 0; i < RACK_SIZE; i++) {
		assert(a->data.tile[i].type == tileNone || a->data.tile[i].type == tileWild || a->data.tile[i].type == tileLetter);
		if (a->data.tile[i].type != tileNone) {
			count++;
		}
	}
	return count;
}

bool boardEmpty(Board *b)
{
	int y, x;

	NOT(b);

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			if (b->tile[y][x].type != tileNone) {
				return false;
			}
		}
	}

	return true;
}

int bagCount(Bag *b)
{
	NOT(b);
	return b->tail > b->head ? b->tail - b->head : b->head - b-> tail;

}

bool vowel(LetterType lt)
{
	return 
		lt == letterA ||
		lt == letterE ||
		lt == letterI ||
		lt == letterO ||
		lt == letterU ||
		lt == letterY;
}

bool constant(LetterType lt)
{
	return !vowel(lt);
}

