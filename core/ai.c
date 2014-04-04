#include <time.h>

#include "common.h"
#include "print.h"

#define MIN_LEN 2

typedef enum
{
	placementInvalid = -1,
	placementHorizontal = 0,
	placementVertical,
	placementCount
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
	if (c->offset >= BOARD_SIZE - MIN_LEN) {
		return false;
	}
	return true;
}

void syncTaken(Cont *c, Board *b, DirType dt, int idx)
{
	int i;
	
	NOT(c);
	NOT(b);

	assert(idx >= 0);

	if (dt == dirDown) {
		assert(idx < BOARD_X);
		for (i = 0; i < BOARD_Y; i++) {
			c->taken[i] = b->tile[i][idx].type != tileNone;
		}
	} else {
		assert(dt == dirRight);
		assert(idx < BOARD_Y);
		for (i = 0; i < BOARD_X; i++) {
			c->taken[i] = b->tile[idx][i].type != tileNone;
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
	return true && c->num > 0;
}

void initCont(Cont *c)
{
	NOT(c);

	c->offset = 0;
	c->len = MIN_LEN;
}

bool nextCombo(int *c, int k, int n)
{
	int i;

	NOT(c);
	assert(k <= n);
	assert(k > 0);

	if (k > n) {
		return true;
	}

	i = k - 1;
	c[i]++;
	while (i >= 0 && c[i] >= n - k + 1 + i) {
		i--;
		c[i]++;
	}

	if (c[0] > n - k) {
		return false;
	}

	for (i = i + 1; i < k; i++) {
		c[i] = c[i - 1] + 1;
	}

	return true;
}

bool nextComboPerm(int *c, int n)
{
	int i, j, k, tmp;

	NOT(c);

	i = n - 2;
	while (i >= 0 && c[i] > c[i + 1]) {
		i--;
	}
	if (i < 0) {
		return false;
	}
	
	k = n - 1; 
	while (c[i] > c[k]) {
		k--;
		VALID_RACK_SIZE(k);
	}
	
	VALID_RACK_SIZE(i);
	VALID_RACK_SIZE(k);

	tmp = c[i];
	c[i] = c[k];
	c[k] = tmp;

	k = 0;
	for (k = 0, j = i + 1; j < (n + i) / 2 + 1; j++, k++) {
		VALID_RACK_SIZE(j);
		VALID_RACK_SIZE(n - k - 1);

		tmp = c[j];
		c[j] = c[n - k - 1];
		c[n - k - 1] = tmp;
	}

	return true;
}

void printCombo(Combo *c)
{
	int i;

	printf("[");
	for (i = 0; i < c->pathCount; i++) {
		printf("%d ", c->pIdx[i]);
	}
	printf("\b]\n");
}

bool stepCombo(Combo *c)
{
	NOT(c);

	if (!nextComboPerm(c->pIdx, c->pathCount)) {
		if (!nextCombo(c->rIdx, c->pathCount, c->rackCount)) {
			return false;
		}
		memCpy(c->pIdx, c->rIdx, sizeof(0[c->rIdx]) * c->pathCount); 
	}

	return true;
}

void initCombo(Combo *c)
{
	int i;

	NOT(c);
	
	for (i = 0; i < RACK_SIZE; i++) {
		c->rIdx[i] = c->pIdx[i] = i;
	}
	c->pathCount = 0;
}

bool eligibleCont(Cont *c, Board *b, DirType dt, int idx, bool firstMove)
{
	int i;

	NOT(c);
	NOT(b);
	assert(dt >= 0 && dt < dirCount);

	if (c->num == 0) {
		return false;
	}

	switch (dt) {
	case dirDown: {
		if (c->offset > 0 && b->tile[c->offset - 1][idx].type != tileNone) {
			return true;
		}
		if (c->offset + c->len < BOARD_Y && b->tile[c->offset + c->len][idx].type != tileNone) {
			return true;
		}
		if (idx > 0) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->tile[i][idx - 1].type != tileNone) {
					return true;
				}
			}
		}
		if (idx < BOARD_X - 1) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->tile[i][idx + 1].type != tileNone) {
					return true;
				}
			}
		}
		if (firstMove) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->sq[i][idx] == sqFree) {
					return true;
				}
			}
		}
		break;
	}
	case dirRight: {
		if (c->offset > 0 && b->tile[idx][c->offset - 1].type != tileNone) {
			return true;
		}
		if (c->offset + c->len < BOARD_X && b->tile[idx][c->offset + c->len].type != tileNone) {
			return true;
		}
		if (idx > 0) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->tile[idx - 1][i].type != tileNone) {
					return true;
				}
			}
		}
		if (idx < BOARD_Y - 1) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->tile[idx + 1][i].type != tileNone) {
					return true;
				}
			}
		}
		if (firstMove) {
			for (i = c->offset; i < c->offset + c->len; i++) {
				if (b->sq[idx][i] == sqFree) {
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

bool placementToMovePlace(struct MovePlace *mp, const Placement *p)
{
        int i;
	NOT(mp);
	NOT(p);
	assert(p->type == placementHorizontal || p->type == placementVertical || p->type == placementInvalid);

	switch (p->type) {
	case placementHorizontal: {
		assert(p->num > 0); 
		assert(p->num <= RACK_SIZE);
		mp->num = p->num;
		assert(p->idx >= 0);
		assert(p->idx < BOARD_Y);
		for (i = 0; i < p->num; i++) {
			mp->rackIdx[i] = p->tile[i].rIdx;
			mp->coor[i].x = p->tile[i].bIdx;
			mp->coor[i].y = p->idx;
		}
		break;
	}
	case placementVertical: {
		assert(p->num > 0);
	       	assert(p->num <= RACK_SIZE);
		mp->num = p->num;
		assert(p->idx >= 0);
		assert(p->idx < BOARD_X);
		for (i = 0; i < p->num; i++) {
			mp->rackIdx[i] = p->tile[i].rIdx;
			mp->coor[i].x = p->idx;
			mp->coor[i].y = p->tile[i].bIdx;
		}
		break;
	}
	case placementInvalid:
	default: return false;
	}

	return true;
}

void mkPlacement(Placement *p, Combo *cb, Cont *cn, DirType dt, int idx)
{
	int i;

	NOT(p);
	NOT(cb);
	NOT(cn);

	assert(cn->num >= 0);
	assert(cb->pathCount >= 0);
	/* assert(cb->pathCount <= cn->num)
	assert(cb->pathCount <= RACK_SIZE);*/

	if (cb->pathCount == 0) {
		p->type = placementInvalid;
		return;
	}

	switch (dt) {
	case dirDown: {
		p->type = placementVertical;
		assert(idx >= 0 && idx < BOARD_X);
		p->idx = idx;
		p->num = cb->pathCount; 
		for (i = 0; i < p->num; i++) {
			p->tile[i].rIdx = cb->pIdx[i];
			p->tile[i].bIdx = cn->openIdx[i];
		}
		break;
	}
	case dirRight: {
		p->type = placementHorizontal;
		assert(idx >= 0 && idx < BOARD_Y);
		p->idx = idx;
		p->num = cb->pathCount;
		for (i = 0; i < p->num; i++) {
			p->tile[i].rIdx = cb->pIdx[i];
			p->tile[i].bIdx = cn->openIdx[i];
		}
		break;
	}
	default: p->type = placementInvalid; break;
	}
}

void printPlacement(Placement *p)
{
	int i;

	NOT(p);
	
	switch (p->type) {
	case placementHorizontal: 
	case placementVertical: {
		printf("[type:%s, idx:%d, num:%d] [", p->type == placementHorizontal ? "horz" : "vert", p->idx, p->num);
		for (i = 0; i < p->num; i++) {
			printf("(%d-%d),", p->tile[i].rIdx, p->tile[i].bIdx);
		}
		printf("\b]\n");
		break;
	}
	default: break;
	}
}

int hueristic(ActionPlace *ap, Player *p)
{
	int i, s, c;

	NOT(ap);
	NOT(p);

	s = ap->score;
	c = 0;
	for (i = 0; i < ap->num; i++) {
		int j = ap->rackIdx[i];
		if (p->tile[j].type == tileLetter) {
			c += constant(p->tile[j].letter);
		}
	}
	s += (ap->score * c) / RACK_SIZE;
	return s;
}

void aiFindMove(Move *m, int pIdx, Game *g, Rule *r, float *loading)
{
	int i, j;
	int hiScore;
	int midScore;
	int lowScore;
	int dir[2];
	int bd[2];
	bool firstMove;
	Cont cont;
	Combo combo;
	Placement placement;
	Move move;
	Action action;
	Board *b;
	Player *p;
	bool first = true;

	NOT(m);
	assert(pIdx >= 0 && pIdx < MAX_PLAYER);
	NOT(g);
	NOT(loading);

	b = &g->board;
	p = &g->player[pIdx];

	bd[0] = BOARD_X;
	bd[1] = BOARD_Y;

	dir[0] = dirDown;
	dir[1] = dirRight;

	hiScore = 0;
	lowScore = 0;
	midScore = 0;
	
	combo.rackCount = rackCount(p);

	if (combo.rackCount == 0) {
		m->playerIdx = pIdx;
		m->type = moveSkip;
		return;
	}
	
	move.type = movePlace;
	move.playerIdx = pIdx; 
	m->playerIdx = pIdx;

	firstMove = boardEmpty(b);

	for (j = 0; j < 2; j++) {
		for (i = 0; i < bd[j]; i++) {
			initCont(&cont);
			syncTaken(&cont, b, dir[j], i);
			do {
				if (!findOpenIdx(&cont)) {
					continue;
				}
				assert(cont.num <= bd[j]);
				if (!eligibleCont(&cont, b, dir[j], i, firstMove)) {
					continue;
				}
				initCombo(&combo);
				combo.pathCount = cont.num;
				if (combo.pathCount > combo.rackCount) {
					continue;
				}
				do {
					assert(combo.pathCount <= RACK_SIZE);
					mkPlacement(&placement, &combo, &cont, dir[j], i);
					
					if (!placementToMovePlace(&move.data.place, &placement)) {
						continue;
					}
					mkAction(&action, g, &move);
					if (action.type == actionPlace) {
						int diff, h;
						diff = abs(hiScore - lowScore) * p->aiShare.difficulty / 10;
						h = hueristic(&action.data.place, p);
						if (first || h > hiScore) {
							hiScore = h;
						}
						if (first || h < lowScore) {
							lowScore = h;
						}
						if (first || abs(midScore - diff) > abs(h - diff)) {
							midScore = h;
							memCpy(&m->data.place, &move.data.place, sizeof(move.data.place));
						}
						first = false;
					}
				} while (stepCombo(&combo));
			} while (nextCont(&cont));
			*loading += 1.f / (float)(bd[j] * 2);
		}
	}

	m->playerIdx = pIdx;

	if (midScore > 0) {
		m->type = movePlace;
	} else {
		assert(rackCount(p) > 0);
		m->type = moveDiscard;
		m->data.discard.num = 1;
		m->data.discard.rackIdx[0] = 0;
	}
}

