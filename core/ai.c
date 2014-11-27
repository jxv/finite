# include "common.h"
#include "print.h"
#include <time.h>

#define MIN_LEN 2

enum placement_tag
{
	PLACEMENT_INVALID = -1,
	PLACEMENT_HORIZONTAL = 0,
	PLACEMENT_VERTICAL,
	PLACEMENT_COUNT
};

typedef enum placement_tag placement_tag_t;

struct cont
{
	int offset;
	int len;
	bool taken[BOARD_SIZE];
	int num;
	int openIdx[RACK_SIZE];
};

struct combo
{
	int rack_count;
	int pathCount;
	int rIdx[RACK_SIZE];
	int pIdx[RACK_SIZE];
};

struct placement_tile
{
	int rack_idx;
	int board_idx;
};

struct placement
{
	placement_tag_t type;
	int idx;
	int num;
	struct placement_tile tile[RACK_SIZE];
};

typedef struct cont cont_t;
typedef struct combo combo_t;
typedef struct placement_tile placement_tile_t;
typedef struct placement placement_t;

static bool next_cont(cont_t *c)
{
	if (c->offset + c->len < BOARD_SIZE) {
		c->len++;
	} else {
		c->offset++;
		c->len = MIN_LEN;
	}
	if (c->offset >= BOARD_SIZE - MIN_LEN)
		return false;
	return true;
}

static void sync_taken(cont_t *c, const board_t *b, dir_tag_t dt, int idx)
{
	if (dt == DIR_DOWN)
		for (int i = 0; i < BOARD_Y; i++)
			c->taken[i] = b->tile[i][idx].type != TILE_NONE;
	else
		for (int i = 0; i < BOARD_X; i++)
			c->taken[i] = b->tile[idx][i].type != TILE_NONE;
}

static bool find_open_idx(cont_t *c)
{
	c->num = 0;
	for (int i = c->offset; i < c->len + c->offset; i++) {
		if (!c->taken[i]) {
			if (c->num == RACK_SIZE)
				return false;
			c->openIdx[c->num] = i;
			c->num++;
		}
	}
	return true && c->num > 0;
}

static void init_cont(cont_t *c)
{
	c->offset = 0;
	c->len = MIN_LEN;
}

static bool next_combo(int *c, int k, int n)
{
	assert(k <= n);
	assert(k > 0);
	if (k > n)
		return true;
	int i = k - 1;
	c[i]++;
	while (i >= 0 && c[i] >= n - k + 1 + i) {
		i--;
		c[i]++;
	}
	if (c[0] > n - k)
		return false;
	for (i = i + 1; i < k; i++)
		c[i] = c[i - 1] + 1;
	return true;
}

static bool next_combo_perm(int *c, int n)
{
	int i = n - 2;
	while (i >= 0 && c[i] > c[i + 1])
		i--;
	if (i < 0)
		return false;
	int k = n - 1;
	while (c[i] > c[k])
		k--;
	const int tmp = c[i];
	c[i] = c[k];
	c[k] = tmp;
	for (int k = 0, j = i + 1; j < (n + i) / 2 + 1; j++, k++) {
		const int tmp = c[j];
		c[j] = c[n - k - 1];
		c[n - k - 1] = tmp;
	}
	return true;
}

/*
static void print_combo(const combo_t *c)
{
	printf("[");
	for (int i = 0; i < c->pathCount; i++)
		printf("%d ", c->pIdx[i]);
	printf("\b]\n");
}
*/

static bool step_combo(combo_t *c)
{
	if (!next_combo_perm(c->pIdx, c->pathCount)) {
		if (!next_combo(c->rIdx, c->pathCount, c->rack_count))
			return false;
		memcpy(c->pIdx, c->rIdx, sizeof(0[c->rIdx]) * c->pathCount);
	}
	return true;
}

static void init_combo(combo_t *c)
{
	for (int i = 0; i < RACK_SIZE; i++)
		c->rIdx[i] = c->pIdx[i] = i;
	c->pathCount = 0;
}

static bool eligible_cont(const board_t *b, dir_tag_t dt, int idx,
			  bool first_move, cont_t *c)
{
	if (c->num == 0)
		return false;
	switch (dt) {
	case DIR_DOWN:
		if (c->offset > 0 &&
				b->tile[c->offset - 1][idx].type != TILE_NONE)
			return true;
		if (c->offset + c->len < BOARD_Y &&
				b->tile[c->offset + c->len][idx].type !=
					TILE_NONE)
			return true;
		if (idx > 0)
			for (int i = c->offset; i < c->offset + c->len; i++)
				if (b->tile[i][idx - 1].type != TILE_NONE)
					return true;
		if (idx < BOARD_X - 1)
			for (int i = c->offset; i < c->offset + c->len; i++)
				if (b->tile[i][idx + 1].type != TILE_NONE)
					return true;
		if (first_move)
			for (int i = c->offset; i < c->offset + c->len; i++)
				if (b->sq[i][idx] == SQ_FREE)
					return true;
		break;
	case DIR_RIGHT:
		if (c->offset > 0 && b->tile[idx][c->offset - 1].type !=
		    TILE_NONE)
			return true;
		if (c->offset + c->len < BOARD_X &&
		    b->tile[idx][c->offset + c->len].type != TILE_NONE)
			return true;
		if (idx > 0)
			for (int i = c->offset; i < c->offset + c->len; i++)
				if (b->tile[idx - 1][i].type != TILE_NONE)
					return true;
		if (idx < BOARD_Y - 1)
			for (int i = c->offset; i < c->offset + c->len; i++)
				if (b->tile[idx + 1][i].type != TILE_NONE)
					return true;
		if (first_move)
			for (int i = c->offset; i < c->offset + c->len; i++)
				if (b->sq[idx][i] == SQ_FREE)
					return true;
		break;
	default:
		break;
	}
	return false;
}

void print_cont(const cont_t *c)
{
	printf("[%d-%d] [", c->offset, c->len);
	for (int i = 0; i < c->num; i++)
		printf("%d,", c->openIdx[i]);
	printf("\b-%d]\n", c->num);
}

bool placement_to_move_place(move_place_t *mp, const placement_t *p)
{
	switch (p->type) {
	case PLACEMENT_HORIZONTAL:
		mp->num = p->num;
		for (int i = 0; i < p->num; i++) {
			mp->rackIdx[i] = p->tile[i].rack_idx;
			mp->coor[i].x = p->tile[i].board_idx;
			mp->coor[i].y = p->idx;
		}
		break;
	case PLACEMENT_VERTICAL:
		mp->num = p->num;
		for (int i = 0; i < p->num; i++) {
			mp->rackIdx[i] = p->tile[i].rack_idx;
			mp->coor[i].x = p->idx;
			mp->coor[i].y = p->tile[i].board_idx;
		}
		break;
	case PLACEMENT_INVALID:
	default:
		return false;
	}

	return true;
}

void mk_placement(const combo_t *cb, const cont_t *cn, dir_tag_t dt, int idx,
		  placement_t *p)
{
	if (cb->pathCount == 0) {
		p->type = PLACEMENT_INVALID;
		return;
	}
	switch (dt) {
	case DIR_DOWN:
		p->type = PLACEMENT_VERTICAL;
		assert(idx >= 0 && idx < BOARD_X);
		p->idx = idx;
		p->num = cb->pathCount;
		for (int i = 0; i < p->num; i++) {
			p->tile[i].rack_idx = cb->pIdx[i];
			p->tile[i].board_idx = cn->openIdx[i];
		}
		break;
	case DIR_RIGHT:
		p->type = PLACEMENT_HORIZONTAL;
		assert(idx >= 0 && idx < BOARD_Y);
		p->idx = idx;
		p->num = cb->pathCount;
		for (int i = 0; i < p->num; i++) {
			p->tile[i].rack_idx = cb->pIdx[i];
			p->tile[i].board_idx = cn->openIdx[i];
		}
		break;
	default:
		p->type = PLACEMENT_INVALID;
		break;
	}
}

void print_placement(const placement_t *p)
{
	switch (p->type) {
	case PLACEMENT_HORIZONTAL:
	case PLACEMENT_VERTICAL:
		printf("[type:%s, idx:%d, num:%d] [",
		       p->type == PLACEMENT_HORIZONTAL ? "horz" : "vert",
		       p->idx, p->num);
		for (int i = 0; i < p->num; i++)
			printf("(%d-%d),", p->tile[i].rack_idx,
			       p->tile[i].board_idx);
		printf("\b]\n");
		break;
	default:
		break;
	}
}

int hueristic(const action_place_t *ap, const player_t *p)
{
	int score = ap->score;
	int constants = 0;
	for (int i = 0; i < ap->num; i++) {
		const int j = ap->rackIdx[i];
		if (p->tile[j].type == TILE_LETTER)
			constants += constant(p->tile[j].letter);
	}
	score += (ap->score * constants) / RACK_SIZE;
	return score;
}

void ai_find_move(move_t *m, int pIdx, game_t *g, rule_t *r, float *loading)
{
	const int dir[2] = {
		[0] = DIR_DOWN,
		[1] = DIR_RIGHT,
	};
	const int bd[2] = {
		[0] = BOARD_X,
		[1] = BOARD_Y,
	};
	const board_t *b = &g->board;
	const player_t *p = &g->player[pIdx];
	const bool first_move = board_empty(b);
	placement_t placement = {.type = PLACEMENT_INVALID, .idx = 0, .num = 0};
	move_t move = {.type = MOVE_PLACE, .playerIdx = pIdx};
	bool first = true;
	combo_t combo;
	combo.rack_count = rack_count(p);
	if (combo.rack_count == 0) {
		m->playerIdx = pIdx;
		m->type = MOVE_SKIP;
		return;
	}
	m->playerIdx = pIdx;
	int hi_score = 0;
	int low_score = 0;
	int mid_score = 0;
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < bd[j]; i++) {
			cont_t cont;
			init_cont(&cont);
			sync_taken(&cont, b, dir[j], i);
			do {
				if (!find_open_idx(&cont))
					continue;
				assert(cont.num <= bd[j]);
				if (!eligible_cont(b, dir[j], i, first_move,
						   &cont))
					continue;
				init_combo(&combo);
				combo.pathCount = cont.num;
				if (combo.pathCount > combo.rack_count)
					continue;
				do {
					mk_placement(&combo, &cont, dir[j], i,
						     &placement);
					if (!placement_to_move_place(&move.data.place,
								     &placement))
						continue;
					action_t action;
					mk_action(g, &move, &action);
					if (action.type == ACTION_PLACE) {
						int diff, h;
						diff = abs(hi_score - low_score) * p->aiShare.difficulty / 10;
						h = hueristic(&action.data.place, p);
						if (first || h > hi_score)
							hi_score = h;
						if (first || h < low_score)
							low_score = h;
						if (first || abs(mid_score - diff) > abs(h - diff)) {
							mid_score = h;
							memcpy(&m->data.place, &move.data.place, sizeof(move.data.place));
						}
						first = false;
					}
				} while (step_combo(&combo));
			} while (next_cont(&cont));
			*loading += 1.f / (float)(bd[j] * 2);
		}
	}

	m->playerIdx = pIdx;

	if (mid_score > 0) {
		m->type = MOVE_PLACE;
	} else {
		assert(rack_count(p) > 0);
		m->type = MOVE_DISCARD;
		m->data.discard.num = 1;
		m->data.discard.rackIdx[0] = 0;
	}
}

