#include "common.h"
#include "print.h"

#define MIN_LEN 2

typedef enum
{
	PLACEMENT_INVALID = -1,
	PLACEMENT_HORIZONTAL = 0,
	PLACEMENT_VERTICAL,
	PLACEMENT_COUNT
} PlacementType;

typedef struct Cont
{
	int offset;
	int len;
	bool taken[BOARD_SIZE];
	int num;
	int openIdx[RACK_SIZE];
} Cont;

typedef struct Combo
{
	int rackCount;
	int pathCount;
	int rIdx[RACK_SIZE];
	int pIdx[RACK_SIZE];
} Combo;

typedef struct PlacementTile
{
	int rIdx;
	int bIdx;
} PlacementTile;

typedef struct Placement
{
	PlacementType type;
	int idx;
	int num;
	struct PlacementTile tile[RACK_SIZE];
} Placement;

bool nextCont(Cont *c)
{
	NOT(c);
	
	if (c->offset + c->len < BOARD_SIZE) {
		c->len++;
	} else {
		c->offset++;
		c->len = MIN_LEN;
	}
	if (c->offset == BOARD_SIZE - MIN_LEN) {
		return false;
	}
	return true;
}

void syncTaken(Cont *c, struct Board *b, DirType dt, int idx)
{
	int i;
	
	NOT(c);
	NOT(b);

	assert(idx >= 0);

	if (dt == DIR_DOWN) {
		assert(idx < BOARD_X);
		for (i = 0; i < BOARD_Y; i++) {
			c->taken[i] = b->tile[i][idx].type != TILE_NONE;
		}
	} else {
		assert(dt == DIR_RIGHT);
		assert(idx < BOARD_X);
		for (i = 0; i < BOARD_X; i++) {
			c->taken[i] = b->tile[idx][i].type != TILE_NONE;
		}
	}
}

bool findOpenIdx(Cont *c)
{
	int i;
	
	NOT(c);

	c->num = 0;
	for (i = c->offset; i < c->len + c->offset; i++) {
		if (!c->taken[i]) {
			if (c->num == RACK_SIZE) {
				return false;
			}
			c->openIdx[c->num] = i;
			c->num++;
		}
	}
	return true;
}

void initCont(Cont *c)
{
	NOT(c);

	c->offset = 0;
	c->len = MIN_LEN;
}

bool nextCombo(Combo *c)
{
	int i;
	
	NOT(c);

	i = c->pathCount - 1;
	c->rIdx[i]++;

	while (i >= 0 && (c->rIdx[i] >= c->rackCount - c->pathCount + 1 + i)) {
		i--;
		c->rIdx[i]++;
	}
	if (c->rIdx[0] > c->rackCount - c->pathCount) {
		return false;
	}
	for (i = i + 1; i < c->pathCount; i++) {
		c->rIdx[i] = c->rIdx[i - 1] + 1;
	}
	return true;
}

bool next(Combo *c)
{
	int i = 0, k = 0, j = 0, tmp;

	NOT(c);
	
	i = c->pathCount - 2;
	while (i >= 0 && c->pIdx[i] > c->pIdx[i + 1]) {
		i--;
	}
	if (i < 0) {
		return false;
	}
	k = c->pathCount - 1;
	while (c->pIdx[i] > c->pIdx[k]) {
		k--;
	}
	tmp = c->pIdx[i];
	c->pIdx[i] = c->pIdx[k];
	c->pIdx[k] = tmp;
	k = 0;
	for (j = i + 1; j < (c->pathCount + i) / 2 + 1; j++, k++) {
		tmp = c->pIdx[j];
		c->pIdx[j] = c->pIdx[c->pathCount - k - 1];
		c->pIdx[c->pathCount - k - 1] = tmp;
	}

	return true;
}

void printv(Combo *c) {
	int i;

	printf("[");
	for (i = 0; i < c->pathCount; i++) {
		printf("%d ", c->pIdx[i]);
	}
	printf("\b]\n");
}

bool stepCombo(Combo *c)
{
	int i;

	NOT(c);

	if (!next(c)) {
		if (!nextCombo(c)) {
			return false;
		}
		for (i = 0; i < c->pathCount; i++) {
			c->pIdx[i] = c->rIdx[i];
		}
	}

	return true;
}

void initCombo(Combo *c)
{
	int i;

	NOT(c);
	
	for (i = 0; i < RACK_SIZE; i++) {
		c->rIdx[i] = i;
		c->pIdx[i] = i;
	}
	c->pathCount = 0;
}

bool eligibleCont(Cont *c, struct Board *b, DirType dt, int idx, bool firstMove)
{
	int i;

	NOT(c);
	NOT(b);
	assert(dt >= 0 && dt < DIR_COUNT);

	switch (dt) {
	case DIR_DOWN: {
		if (c->offset > 0 && b->tile[c->offset - 1][idx].type != TILE_NONE) {
			return true;
		}
		if (c->offset + c->len < BOARD_Y && b->tile[c->offset + c->len][idx].type != TILE_NONE) {
			return true;
		}
		if (idx > 0) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->tile[i][idx - 1].type != TILE_NONE) {
					return true;
				}
			}
		}
		if (idx < BOARD_X - 1) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->tile[i][idx + 1].type != TILE_NONE) {
					return true;
				}
			}
		}
		if (firstMove) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->sq[i][idx] == SQ_FREE) {
					return true;
				}
			}
		}
		break;
	}
	case DIR_RIGHT: {
		if (c->offset > 0 && b->tile[idx][c->offset - 1].type != TILE_NONE) {
			return true;
		}
		if (c->offset + c->len < BOARD_X && b->tile[idx][c->offset + c->len].type != TILE_NONE) {
			return true;
		}
		if (idx > 0) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->tile[idx - 1][i].type != TILE_NONE) {
					return true;
				}
			}
		}
		if (idx < BOARD_Y - 1) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->tile[idx + 1][i].type != TILE_NONE) {
					return true;
				}
			}
		}
		if (firstMove) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->sq[idx][i] == SQ_FREE) {
					return true;
				}
			}
		}
		break;
	}
	default: break;
	}
	
	return false;
}

void printCont(Cont *c)
{
	int i;

	NOT(c);
	
	printf("[%d-%d] [", c->offset, c->len);
	for (i = 0; i < c->num; i++) {
		printf("%d,", c->openIdx[i]);
	}
	printf("\b-%d]\n", c->num);
}

void placementToMovePlace(struct MovePlace *mp, Placement *p)
{
	int i;

	NOT(mp);
	NOT(p);
	assert(p->type == PLACEMENT_HORIZONTAL || p->type == PLACEMENT_VERTICAL);

	switch (p->type) {
	case PLACEMENT_HORIZONTAL: {
		assert(p->num > 0); 
		assert(p->num <= RACK_SIZE);
		mp->num = p->num;
		for (i = 0; i < p->num; i++) {
			mp->rackIdx[i] = p->tile[i].rIdx;
			mp->coor[i].x = p->tile[i].bIdx;
			mp->coor[i].y = p->idx;
		}
		break;
	}
	case PLACEMENT_VERTICAL: {
		assert(p->num > 0);
	       	assert(p->num <= RACK_SIZE);
		mp->num = p->num;
		for (i = 0; i < p->num; i++) {
			mp->rackIdx[i] = p->tile[i].rIdx;
			mp->coor[i].x = p->idx;
			mp->coor[i].y = p->tile[i].bIdx;
		}
		break;
	}
	default: break;
	}
}

void mkPlacement(Placement *p, Combo *cb, Cont *cn, DirType dt, int idx)
{
	int i;

	NOT(p);
	NOT(cb);
	NOT(cn);

	switch (dt) {
	case DIR_DOWN: {
		p->type = PLACEMENT_VERTICAL;
		assert(idx >= 0 && idx < BOARD_X);
		p->idx = idx;
		p->num = cn->num; 
		for (i = 0; i < p->num; i++) {
			p->tile[i].rIdx = cb->pIdx[i];
			p->tile[i].bIdx = cn->openIdx[i];
		}
		break;
	}
	case DIR_RIGHT: {
		p->type = PLACEMENT_HORIZONTAL;
		assert(idx >= 0 && idx < BOARD_Y);
		p->num = idx;
		p->num = cn->num;
		for (i = 0; i < p->num; i++) {
			p->tile[i].rIdx = cb->pIdx[i];
			p->tile[i].bIdx = cn->openIdx[i];
		}
		break;
	}
	default: p->type = PLACEMENT_INVALID; break;
	}
}

void printPlacement(Placement *p)
{
	int i;

	NOT(p);
	
	switch (p->type) {
	case PLACEMENT_HORIZONTAL: 
	case PLACEMENT_VERTICAL: {
		printf("[type:%s, idx:%d, num:%d] [", p->type == PLACEMENT_HORIZONTAL ? "horz" : "vert", p->idx, p->num);
		for (i = 0; i < p->num; i++) {
			printf("(%d-%d),", p->tile[i].rIdx, p->tile[i].bIdx);
		}
		printf("\b]\n");
		break;
	}
	default: break;
	}
}

void aiFindMove(struct Move *m, struct Player *p, struct Board *b, struct Game *g, struct Rule *r)
{
	int i;
	int maxScore;
	Cont cont;
	Combo combo;
	Placement placement;
	Move move;
	Action action;

	NOT(m);
	NOT(p);
	NOT(b);
	NOT(g);

	if (0)
	{
		initCont(&cont);
		syncTaken(&cont, b, DIR_DOWN, 7);
		findOpenIdx(&cont);
		initCombo(&combo);
		combo.rackCount = rackCount(p);
		combo.pathCount = 7;
		mkPlacement(&placement, &combo, &cont, DIR_DOWN, 7);
		printPlacement(&placement);
		m->playerIdx = 1;
		m->type = MOVE_PLACE;
		placementToMovePlace(&m->data.place, &placement);
		return;
	}



	maxScore = 0;
	
	initCont(&cont);
	cont.offset = 0;
	cont.len = MIN_LEN;
	combo.rackCount = rackCount(p);

	for (i = 0; i < BOARD_X; i++) {
		printf("[%d]idx\n", i);
		initCont(&cont);
		syncTaken(&cont, b, DIR_DOWN, i);
		do {
			findOpenIdx(&cont);
			if (!eligibleCont(&cont, b, DIR_DOWN, i, true)) {
				continue;
			}
			initCombo(&combo);
			combo.pathCount = cont.num;
			do {
				/*printv(&combo);*/
				mkPlacement(&placement, &combo, &cont, DIR_DOWN, i);
				
				placementToMovePlace(&move.data.place, &placement);
				move.type = MOVE_PLACE;
				move.playerIdx = 1;
				mkAction(&action, g, &move, r);
				if (action.type == ACTION_PLACE) {
					if (action.data.place.score > maxScore) {
						maxScore = action.data.place.score;
						placementToMovePlace(&m->data.place, &placement);
						puts("yes");
					} else {
						puts("almost");
					}
				}
				else
				if (action.type == ACTION_INVALID) {
					printf("[%d]\n", action.data.err);
					printActionErr(action.data.err);
				}
				/* printPlacement(&placement); */
			} while (stepCombo(&combo));
		} while (nextCont(&cont));
	}

	m->playerIdx = 1;
	m->type = MOVE_PLACE;

	/* dummy function */
	/*
	int i;
	struct move *itni;
	for each possible move, it:
		
	if (better_move(it, m)) {
		m := it;
	}
	*/
}

/* TODO: move log, doc the core, ai */


