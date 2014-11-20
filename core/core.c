#include "common.h"
#include "print.h"

static int letter_score(letter_t l)
{
	switch (l) {
	case LETTER_A:
		return 1;
	case LETTER_B:
		return 3;
	case LETTER_C:
		return 3;
	case LETTER_D:
		return 2;
	case LETTER_E:
		return 1;
	case LETTER_F:
		return 4;
	case LETTER_G:
		return 2;
	case LETTER_H:
		return 4;
	case LETTER_I:
		return 1;
	case LETTER_J:
		return 8;
	case LETTER_K:
		return 5;
	case LETTER_L:
		return 1;
	case LETTER_M:
		return 3;
	case LETTER_N:
		return 1;
	case LETTER_O:
		return 1;
	case LETTER_P:
		return 3;
	case LETTER_Q:
		return 10;
	case LETTER_R:
		return 1;
	case LETTER_S:
		return 1;
	case LETTER_T:
		return 1;
	case LETTER_U:
		return 1;
	case LETTER_V:
		return 4;
	case LETTER_W:
		return 4;
	case LETTER_X:
		return 8;
	case LETTER_Y:
		return 4;
	case LETTER_Z:
		return 10;
	default:
		assert(false);
		break;
	}
	return 0;
}

int tile_score(const tile_t *t)
{
	if (t->type == TILE_LETTER)
		return letter_score(t->letter);
	return 0;
}

static bool can_use_dbl_let(const board_t *b, const dir_t *d, int p, int x,
			    int y)
{
	return d->pos[p] && b->sq[y][x] == SQ_DBL_LET;
}

static bool can_use_trp_let(const board_t *b, const dir_t *d, int p, int x,
		int y)
{
	return d->pos[p] && b->sq[y][x] == SQ_TRP_LET;
}

static bool can_use_dbl_wrd(const board_t *b, const dir_t *d, int p, int x,
		int y)
{
	return d->pos[p] && b->sq[y][x] == SQ_DBL_WRD;
}

static bool can_use_trp_wrd(const board_t *b, const dir_t *d, int p, int x,
		int y)
{
	return d->pos[p] && b->sq[y][x] == SQ_TRP_WRD;
}

static int dir_score(const board_t *b, const dir_t *d)
{
	const int x = d->x;
	const int y = d->y;
	int dbl_wrd = 0;
	int trp_wrd = 0;
	int score = 0;
	switch (d->type) {
	case DIR_RIGHT:
		for (int i = d->x; i < d->len + d->x; i++) {
			int tile = tile_score(&b->tile[y][i]);
			tile *= can_use_dbl_let(b, d, i, i, y) ? 2 : 1;
			tile *= can_use_trp_let(b, d, i, i, y) ? 3 : 1;
			dbl_wrd += can_use_dbl_wrd(b, d, i, i, y);
			trp_wrd += can_use_trp_wrd(b, d, i, i, y);
			score += tile;
		}
		break;
	case DIR_DOWN:
		for (int i = d->y; i < d->len + d->y; i++) {
			int tile = tile_score(&b->tile[i][x]);
			tile *= can_use_dbl_let(b, d, i, x, i) ? 2 : 1;
			tile *= can_use_trp_let(b, d, i, x, i) ? 3 : 1;
			dbl_wrd += can_use_dbl_wrd(b, d, i, x, i);
			trp_wrd += can_use_trp_wrd(b, d, i, x, i);
			score += tile;
		}
		break;
	case DIR_INVALID:
		return 0;
	default:
		assert(false);
		return 0;
	}
	if (dbl_wrd > 0)
		score *= dbl_wrd * 2;
	if (trp_wrd > 0)
		score *= trp_wrd * 3;
	return score;
}

static int meta_path_dot_score(const path_dot_t *dot, const board_t *b)
{
	int score = 0;
	if (dot->right.type == DIR_RIGHT)
		score = dir_score(b, &dot->right);
	if (dot->down.type == DIR_DOWN)
		score += dir_score(b, &dot->down);
	return score;
}

static int meta_path_score(const dir_t *d, const dir_t *adj, int n,
		const board_t *b)
{
	int score = 0;
	if (d->type != DIR_INVALID) {
		score = dir_score(b, d);
		for (int i = 0; i < n; i++)
			if (adj[i].type != DIR_INVALID)
				score += dir_score(b, &adj[i]);
	}
	return score;
}

static int path_score(const path_t *p)
{
	const board_t *b = &p->board;
	switch (p->type) {
	case PATH_DOT:
		return meta_path_dot_score(&p->data.dot, b);
	case PATH_HORZ:
		return meta_path_score(&p->data.horz.right, p->data.horz.down,
				       BOARD_X, b);
	case PATH_VERT:
		return meta_path_score(&p->data.vert.down, p->data.vert.right,
				       BOARD_Y, b);
	case PATH_INVALID:
		return 0;
	default:
		assert(false);
		return 0;
	}
}

bool bag_full(const bag_t *b)
{
	return (b->head + 1) % BAG_SIZE == b->tail;
}

bool bag_empty(const bag_t *b)
{
	return b->head == b->tail;
}

int bag_size(const bag_t *b)
{
	if (b->head > b->tail)
		return b->head - b->tail;
	return b->tail - b->head;
}

static tile_t bag_peek(const bag_t *b)
{
	tile_t next = { .type = TILE_NONE };
	if (b->head != b->tail)
		next = b->tile[b->head];
	return next;
}

static void bag_drop(bag_t *b)
{
	b->head++;
	b->head %= BAG_SIZE;
}

/* unused */
/*
static void bag_add(tile_t t, bag_t *b)
{
	b->tile[b->tail] = t;
	b->tail++;
	b->tail %= BAG_SIZE;
}
*/

bool adjust_out_of_range(const tile_adjusts_t tile)
{
	for (int i = 0; i < RACK_SIZE; i++) {
		if (tile[i].idx < 0)
			return true;
		if (tile[i].idx >= RACK_SIZE)
			return true;
	}
	return false;
}

void adjust_swap(int i, int j, tile_adjusts_t tile)
{
	const tile_adjust_t tmp = tile[i];
	tile[i] = tile[j];
	tile[j] = tmp;
}

void mk_adjust(const player_t *p, adjust_t *a)
{
	a->type = ADJUST_RACK;
	for (int i = 0; i < RACK_SIZE; i++) {
		a->data.tile[i].type = p->tile[i].type;
		a->data.tile[i].idx = i;
	}
}

static bool adjust_duplicate_index(const tile_adjusts_t tile)
{
	for (int i = 0; i < RACK_SIZE; i++) {
		int idx = tile[i].idx;
		for (int j = i + 1; j < RACK_SIZE; j++)
			if (idx == tile[j].idx)
				return true;
	}
	return false;
}

adjust_err_t find_adjust_err(const adjust_t *a, const player_t *p)
{
	assert(a->type == ADJUST_RACK);
	if (adjust_out_of_range(a->data.tile))
		return ADJUST_ERR_RACK_OUT_OF_RANGE;
	if (adjust_duplicate_index(a->data.tile))
		return ADJUST_ERR_RACK_DUP_IDX;
	return ADJUST_ERR_NONE;
}

void apply_adjust(const tile_adjusts_t tile, player_t *p)
{
	tile_t p_tile[RACK_SIZE];
	for (int i = 0; i < RACK_SIZE; i++)
		p_tile[i] = p->tile[tile[i].idx];
	for (int i = 0; i < RACK_SIZE; i++)
		p->tile[i] = p_tile[i];
}

int cmp_word(const word_t *w0, const word_t *w1)
{
	/*
	 * w0 > w1 -> 1
	 * w0 < w1 -> -1
	 * w0 == w1 -> 0
	 */
	for (int i = 0; ; i++) {
		if (w0->len  > w1->len && i == w1->len)
			return 1;
		if (w0->len  < w1->len && i == w0->len)
			return -1;
		if (w0->len == w1->len && i == w1->len)
			return 0;
		if (w0->letter[i] == w1->letter[i])
			continue;
		if (w0->letter[i]  > w1->letter[i])
			return 1;
		if (w0->letter[i]  < w1->letter[i])
			return -1;
	}
	return 0;
}

bool word_valid(const word_t *w, const dict_t *d)
{
	long min = 0;
	long max = d->num;
	long mid = d->num / 2;
	while (min <= max) {
		switch (cmp_word(w, &d->words[mid])) {
		case 0:
			return true;
		case 1:
			min = mid + 1;
			break;
		case -1:
			max = mid - 1;
			break;
		default:
			assert(false);
			return false;
		}
		mid = (min + max) / 2;
	}
	return false;
}

static bool dir_valid(const dir_t *dir, const board_t *b, const dict_t *dict,
			word_t *w)
{
	const int x = dir->x;
	const int y = dir->y;
	w->len = dir->len;
	switch (dir->type) {
	case DIR_RIGHT:
		for (int i = 0; i < w->len; i++)
			if (b->tile[y][x + i].type != TILE_NONE)
				w->letter[i] = b->tile[y][x + i].letter;
			else
				return false;
		return true;
	case DIR_DOWN:
		for (int i = 0; i < w->len; i++)
			if (b->tile[y + i][x].type != TILE_NONE)
				w->letter[i] = b->tile[y + i][x].letter;
			else
				return false;
		return true;
	case DIR_INVALID:
		return false;
	default:
		assert(false);
		return false;
	}
}

static bool path_valid(const path_t *p, const dict_t *d,
		bool (*rule)(word_t *, path_tag_t, dir_tag_t))
{
	word_t w0, w1;
	bool result = true;
	switch (p->type) {
	case PATH_DOT: {
		bool a0 = dir_valid(&p->data.dot.right, &p->board, d, &w0);
		bool b0 = dir_valid(&p->data.dot.down, &p->board, d, &w1);
		a0 = a0 ? word_valid(&w0, d) : a0;
		b0 = b0 ? word_valid(&w1, d) : b0;
		a0 = a0 && rule ? rule(&w0, p->type, DIR_RIGHT) : a0;
		b0 = b0 && rule ? rule(&w1, p->type, DIR_DOWN) : b0;
		const bool a1 = p->data.dot.right.len > 1;
		const bool b1 = p->data.dot.down.len > 1;
		if (!((a0 && !(b0 || b1)) || (b0 && !(a0 || a1)) || (a0 && b0)))
			result = false;
		break;
	}
	case PATH_HORZ:
		if (!dir_valid(&p->data.horz.right, &p->board, d, &w0)) {
			result = false;
		} else if (!word_valid(&w0, d)) {
			result = false;
		} else if (rule && !rule(&w0, p->type, DIR_RIGHT)) {
			result = false;
		}
		for (int i = 0; i < BOARD_X; i++)
			if (p->data.horz.down[i].type == DIR_DOWN) {
				if (!dir_valid(&p->data.horz.down[i],
						&p->board, d, &w1)) {
					result = false;
				} else if (w1.len > 1 && !word_valid(&w1, d)) {
					result = false;
				} else if (w1.len > 1 && rule &&
						!rule(&w1,p->type, DIR_DOWN)) {
					result = false;
				}
			}
		break;
	case PATH_VERT:
		if (!dir_valid(&p->data.vert.down, &p->board, d, &w0)) {
			result = false;
		} else if (!word_valid(&w0, d)) {
			result = false;
		} else if (rule && !rule(&w0, p->type, DIR_DOWN)) {
			result = false;
		}
		for (int i = 0; i < BOARD_Y; i++)
			if (p->data.vert.right[i].type == DIR_RIGHT) {
				if(!dir_valid(&p->data.vert.right[i], &p->board,
					      d, &w1)) {
					result = false;
				} else if (w1.len > 1 && !word_valid(&w1, d)) {
					result = false;
				} else if (w1.len > 1 && rule &&
					   !rule(&w1, p->type, DIR_RIGHT)) {
					result = false;
				}
			}
		break;
	case PATH_INVALID:
		result = false;
		break;
	default:
		assert(false);
		result = false;
		break;
	}
	return result;
}

static bool tiles_adjacent(const board_t *b, const move_place_t *mp,
		const player_t *p)
{
	for (int i = 0; i < mp->num; i++) {
		const int r = mp->rackIdx[i];
		const int y = mp->coor[i].y;
		const int x = mp->coor[i].x;
		if (p->tile[r].type != TILE_NONE) {
			if (x > 0 && b->tile[y][x - 1].type != TILE_NONE)
				return true;
			if (y > 0 && b->tile[y - 1][x].type != TILE_NONE)
				return true;
			if (x < BOARD_X - 1 &&
			    b->tile[y][x + 1].type != TILE_NONE)
				return true;
			if (y < BOARD_Y - 1 &&
			    b->tile[y + 1][x].type != TILE_NONE)
				return true;
		}
	}
	return false;
}

static bool on_free_squares(const board_t *b, const move_place_t *mp,
		const player_t *p)
{
	for (int i = 0; i < mp->num; i++) {
		const int r = mp->rackIdx[i];
		const int y = mp->coor[i].y;
		const int x = mp->coor[i].x;
		if (p->tile[r].type != TILE_NONE && b->sq[y][x] == SQ_FREE)
			return true;
	}
	return false;
}

static bool on_vowels(const board_t *b, const move_place_t *mp,
		const player_t *p)
{
	for (int i = 0; i < mp->num; i++) {
		const int r = mp->rackIdx[i];
		const int y = mp->coor[i].y;
		const int x = mp->coor[i].x;
		if (b->sq[y][x] == SQ_NO_VOWEL && p->tile[r].type != TILE_NONE &&
		    constant(p->tile[r].letter))
			return false;
	}
	return true;
}

static bool on_blocks(const board_t *b, const move_place_t *mp,
		const player_t *p)
{
	for (int i = 0; i < mp->num; i++) {
		const int r = mp->rackIdx[i];
		const int y = mp->coor[i].y;
		const int x = mp->coor[i].x;
		if (b->sq[y][x] == SQ_BLOCK && p->tile[r].type != TILE_NONE)
			return true;
	}
	return false;
}

static bool place_in_range(const move_place_t *mp)
{
	for (int i = 0; i < mp->num; i++) {
		if (mp->rackIdx[i] < 0 || mp->rackIdx[i] >= RACK_SIZE)
			return false;
		if (mp->coor[i].x < 0 || mp->coor[i].x >= BOARD_X)
			return false;
		if (mp->coor[i].y < 0 || mp->coor[i].y >= BOARD_Y)
			return false;
	}
	return true;
}

static bool place_overlap(const move_place_t *mp)
{
	for (int i = 0; i < mp->num && i < RACK_SIZE; i++)
		for (int j = i + 1; j < mp->num && j < RACK_SIZE; j++) {
			if (mp->rackIdx[i] == mp->rackIdx[j])
				return false;
			if (mp->coor[i].x == mp->coor[j].x &&
			    mp->coor[i].y == mp->coor[j].y)
				return true;
		}
	return false;
}

static bool place_overlap_board(const move_place_t *mp, const board_t *b)
{
	for (int i = 0; i < mp->num; i++) {
		const int x = mp->coor[i].x;
		const int y = mp->coor[i].y;
		if (b->tile[y][x].type != TILE_NONE)
			return true;
	}
	return false;
}

static bool place_rack_exist(const move_place_t *mp, const player_t *p)
{
	for (int i = 0; i < mp->num; i++)
		if (p->tile[mp->rackIdx[i]].type == TILE_NONE)
			return false;
	return true;
}

static void cpy_rack_board(const move_place_t *mp, const player_t *p,
		board_t *b)
{
	for (int i = 0; i < mp->num; i++) {
		const int r = mp->rackIdx[i];
		const int y = mp->coor[i].y;
		const int x = mp->coor[i].x;
		memcpy(&b->tile[y][x], &p->tile[r], sizeof(tile_t));
	}
}

static bool is_horz(const action_t *a, const move_t *m)
{
	const board_t *b = &a->data.place.path.board;
	const int y = m->data.place.coor[0].y;
        int min = m->data.place.coor[0].x;
        int max = m->data.place.coor[0].x;
	if (m->data.place.num < 2)
		return false;
	for (int i = 1; i < m->data.place.num; i++) {
	        const coor_t *coor = &m->data.place.coor[i];
		if (y != coor->y)
			return false;
		if (min < coor->x)
			min = coor->x;
		if (max > coor->x)
			max = coor->x;
	}
	for (int i = min; i <= max; i++)
		if (b->tile[y][i].type == TILE_NONE)
			return false;
	return true;
}

static bool is_vert(const action_t *a, const move_t *m)
{
	const int x = m->data.place.coor[0].x;
	const board_t *board = &a->data.place.path.board;
	int min = m->data.place.coor[0].y;
	int max = m->data.place.coor[0].y;
	if (m->data.place.num < 2)
		return false;
	for (int i = 1; i < m->data.place.num; i++) {
		const coor_t *coor = &m->data.place.coor[i];
		if (x != coor->x)
			return false;
		if (min > coor->y)
			min = coor->y;
		if (max < coor->y)
			max = coor->y;
	}
	for (int i = min; i <= max; i++)
		if (board->tile[i][x].type == TILE_NONE)
			return false;
	return true;
}

static void mk_right(int x, int y, const board_t *b, dir_t *d)
{
	d->type = DIR_RIGHT;
	d->x = x;
	d->y = y;
	memset(d->pos, false, sizeof(bool) * BOARD_SIZE);
	d->pos[x] = true;
	for (int i = x; i >= 0 && b->tile[y][i].type != TILE_NONE; i--)
		d->x = i;
	for (int i = x; i < BOARD_X && b->tile[y][i].type != TILE_NONE; i++)
		d->len = i;
	d->len -= d->x - 1;
	/* A word cannot be 1 letter long. */
	if (d->len == 1)
		 d->type = DIR_INVALID;
}

static void mk_down(int x, int y, const board_t *b, dir_t *d)
{
	d->type = DIR_DOWN;
	d->x = x;
	d->y = y;
	memset(d->pos, false, sizeof(bool) * BOARD_SIZE);
	d->pos[y] = true;
	for (int i = y; i >= 0 && b->tile[i][x].type != TILE_NONE; i--)
		d->y = i;
	for (int i = y; i < BOARD_Y && b->tile[i][x].type != TILE_NONE; i++)
		d->len = i;
	d->len -= d->y - 1;
	/* A word cannot be 1 letter long. */
	if (d->len == 1)
		d->type = DIR_INVALID;
}

static void mk_dot(const move_t *m, action_t *a)
{
	const int x = m->data.place.coor[0].x;
	const int y = m->data.place.coor[0].y;
	const board_t *b = &a->data.place.path.board;
	path_t *p = &a->data.place.path;
	dir_t *d = &p->data.dot.right;
	p->type = PATH_DOT;
	mk_right(x, y, b, d);
	d = &p->data.dot.down;
	mk_down(x, y, b, d);
}

static void mk_horz(const move_t *m, action_t *a)
{
        const move_place_t *mp = &m->data.place;
	const board_t *b = &a->data.place.path.board;
	path_t *p = &a->data.place.path;
	dir_t *d = &p->data.horz.right;
	p->type = PATH_HORZ;
	mk_right(mp->coor[0].x, mp->coor[0].y, b, d);
	for (int i = 0; i < mp->num; i++)
		d->pos[mp->coor[i].x] = true;
	for (int i = 0; i < BOARD_X; i++)
		p->data.horz.down[i].type = DIR_INVALID;
	for (int i = 0; i < mp->num; i++) {
		d = &p->data.horz.down[i];
		mk_down(mp->coor[i].x, mp->coor[i].y, b, d);
	}
}

static void mk_vert(const move_t *m, action_t *a)
{
        const move_place_t *mp = &m->data.place;
	const int x = m->data.place.coor[0].x;
	const int y = m->data.place.coor[0].y;
	const board_t *b = &a->data.place.path.board;
	path_t *path = &a->data.place.path;
	dir_t *d = &path->data.vert.down;
	path->type = PATH_VERT;
	mk_down(x, y, b, d);
	for (int i = 0; i < mp->num; i++)
		d->pos[mp->coor[i].y] = true;
	for (int i = 0; i < BOARD_Y; i++)
		path->data.vert.right[i].type = DIR_INVALID;
	for (int i = 0; i < mp->num; i++) {
		d = &path->data.vert.right[i];
		const int x = m->data.place.coor[i].x;
		const int y = m->data.place.coor[i].y;
		mk_right(x, y, b, d);
	}
}

static action_err_t find_place_err(const move_place_t *mp, const player_t *p,
                const board_t *b)
{
	if (!place_in_range(mp))
		return ACTION_ERR_PLACE_OUT_OF_RANGE;
	if (place_overlap(mp))
		return ACTION_ERR_PLACE_SELF_OVERLAP;
	if (place_overlap_board(mp, b))
		return ACTION_ERR_PLACE_BOARD_OVERLAP;
	if (!place_rack_exist(mp, p))
		return ACTION_ERR_PLACE_INVALID_RACK_ID;
	if ((!tiles_adjacent(b, mp, p) && !on_free_squares(b, mp, p)) ||
	    !on_vowels(b, mp, p) || on_blocks(b, mp, p))
		return ACTION_ERR_PLACE_INVALID_SQ;
	return ACTION_ERR_NONE;
}

/* unused */
/*
static bool rule_z4_char(const word_t *w, path_tag_t pt, dir_tag_t dt)
{
	return w->len == 4 && w->letter[0] == LETTER_Z;
}
*/

static void mk_place(const game_t *g, const move_t *m, action_t *a)
{
	const int num = m->data.place.num;
	path_t *path = &a->data.place.path;
	const player_t *player = &g->player[m->playerIdx];
	a->type = ACTION_PLACE;
	a->data.place.num = m->data.place.num;
	for (int i = 0; i < num; i++)
		a->data.place.rackIdx[i] = m->data.place.rackIdx[i];
	action_err_t err = find_place_err(&m->data.place, player, &g->board);
	if (err != ACTION_ERR_NONE) {
		a->type = ACTION_INVALID;
		a->data.err = err;
		return;
	}
	memcpy(&path->board, &g->board, sizeof(board_t));
	cpy_rack_board(&m->data.place, player, &path->board);
	assert(num >= 0);
	switch (num) {
	case 0:
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_PLACE_NO_RACK;
		return;
	case 1:
		mk_dot(m, a);
		break;
	default:
		assert(num > 1);
		if (is_horz(a, m)) {
			mk_horz(m, a);
		} else {
			if (is_vert(a, m)) {
				mk_vert(m, a);
			} else {
				a->type = ACTION_INVALID;
				a->data.err = ACTION_ERR_PLACE_NO_DIR;
				return;
			}
		}
		break;
	}
	if (!path_valid(path, &g->dict, NULL)) {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_PLACE_INVALID_PATH;
		return;
	}
	a->data.place.score = path_score(path);
}

void mk_discard(const game_t *g, const move_t *m, action_t *a)
{
	if (m->data.discard.num == 0) {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_DISCARD_EMPTY;
		return;
	}
	assert(m->data.discard.num > 0);
	a->type = ACTION_DISCARD;
	a->data.discard.score = -m->data.discard.num;
	if (a->data.discard.score + g->player[a->playerIdx].score < 0) {
		a->data.discard.score +=
			g->player[a->playerIdx].score -
			a->data.discard.score;
	}
	a->data.discard.num = m->data.discard.num;
	for (int i = 0; i < a->data.discard.num; i++) {
		a->data.discard.rackIdx[i] = m->data.discard.rackIdx[i];
	}
}

void mk_skip(const game_t *g, action_t *a)
{
	if (g->rule.skip && !g->rule.skip(g)) {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_SKIP_RULE;
		return;
	}
	a->type = ACTION_SKIP;
}

void mk_quit(const game_t *g, action_t *a)
{
	if (g->rule.quit && !g->rule.quit(g)) {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_QUIT_RULE;
		return;
	}
	a->type = ACTION_QUIT;
}

void mk_action(const game_t *g, const move_t *m, action_t *a)
{
	a->playerIdx = m->playerIdx;
	switch (m->type) {
	case MOVE_PLACE:
                mk_place(g, m, a);
                break;
	case MOVE_DISCARD:
                mk_discard(g, m, a);
                break;
	case MOVE_SKIP:
                mk_skip(g, a);
                break;
	case MOVE_QUIT:
                mk_quit(g, a);
                break;
	case MOVE_INVALID:
	        a->type = ACTION_INVALID;
                break;
        default:
                assert(false);
	        a->type = ACTION_INVALID;
                break;
	}
}

void rack_shift(player_t *p)
{
	int j = 0;
	int i = 0;
	while (i < RACK_SIZE) {
		while (p->tile[i].type == TILE_NONE && i < RACK_SIZE)
			i++;
		if (i == RACK_SIZE)
			break;
		p->tile[j] = p->tile[i];
		i++;
		j++;
	}
	while (j < RACK_SIZE) {
		p->tile[j].type = TILE_NONE;
		j++;
	}
}

void rack_refill(bag_t *b, player_t *p)
{
	for (int i = 0; i < RACK_SIZE && !bag_empty(b); i++)
		if (p->tile[i].type == TILE_NONE) {
			p->tile[i] = bag_peek(b);
			bag_drop(b);
		}
}

bool apply_action(const action_t *a, game_t *g)
{
	const int id = a->playerIdx;
	if (id != g->turn)
		return false;
	switch (a->type) {
	case ACTION_PLACE:
		memcpy(&g->board, &a->data.place.path.board,
                        sizeof(a->data.place.path.board));
		for (int i = 0; i < a->data.place.num; i++) {
			const int r = a->data.place.rackIdx[i];
			g->player[id].tile[r].type = TILE_NONE;
		}
		rack_refill(&g->bag, &g->player[id]);
		rack_shift(&g->player[id]);
		g->player[id].score += a->data.place.score;
		break;
	case ACTION_DISCARD:
		for (int i = 0; i < a->data.discard.num; i++) {
			const int r = a->data.discard.rackIdx[i];
			g->player[id].tile[r].type = TILE_NONE;
		}
		rack_refill(&g->bag, &g->player[id]);
		rack_shift(&g->player[id]);
		g->player[id].score += a->data.discard.score;
		break;
	case ACTION_SKIP:
		break;
	case ACTION_QUIT:
		g->player[id].active = false;
		break;
	case ACTION_INVALID:
                return false;
	default:
                assert(false);
                return false;
	}

	return true;
}

void rm_rack_tile(const int *rack_idx, int n, player_t *p)
{
	for (int i = 0; i < n; i++)
		p->tile[rack_idx[i]].type = TILE_NONE;
}

void next_turn(game_t *g)
{
	do {
		g->turn ++;
		g->turn %= g->playerNum;
	} while (!g->player[g->turn].active);
}

void move_clear(move_t *m)
{
	memset(m, 0x00, sizeof(move_t));
	m->type = MOVE_INVALID;
}

void action_clear(action_t *a)
{
	memset(a, 0x00, sizeof(action_t));
	a->type = ACTION_INVALID;
}

bool end_game(const game_t *g)
{
	/* 2 active players min. */
	int j = 0;
	for (int i = 0; i < g->playerNum; i++)
		if (g->player[i].active)
			j++;
	if (j < 2)
			return true;
	/* 1 player wth tiles on the rack. */
	j = 0;
	for (int i = 0; i < g->playerNum; i++)
		if (g->player[i].active)
			for (int k = 0; k < RACK_SIZE; k++)
				if (g->player[i].tile[k].type != TILE_NONE)
					j++;
	if (j == 0)
		return true;
	return false;
}

/* RETURN VALUE: Winner's index on success and -1 on failure. */
int find_winner(const game_t *g)
{
	int j = 0;
	for (int i = 0; i < g->playerNum; i++)
		if (g->player[i].active)
			j++;
	if (j <= 1)
		return -1;
	int idx = -1;
	int max = 0;
	for (int i = 0; i < g->playerNum; i++)
		if (g->player[i].active && max < g->player[i].score) {
			idx = i;
			max = g->player[i].score;
		}
	return idx;
}

bool valid_rack_idx(int id)
{
	return id >= 0 && id < RACK_SIZE;
}

bool valid_board_idx(coor_t c)
{
	return c.x >= 0 && c.y >= 0 && c.x < BOARD_X && c.y < BOARD_Y;
}

int rack_count(const player_t *p)
{
	int count = 0;
	for (int i = 0; i < RACK_SIZE; i++)
		if (p->tile[i].type != TILE_NONE)
			count++;
	return count;
}

int adjust_tile_count(const tile_adjusts_t tile)
{
	int count = 0;
	for (int i = 0; i < RACK_SIZE; i++)
		if (tile[i].type != TILE_NONE)
			count++;
	return count;
}

bool board_empty(const board_t *b)
{
	for (int y = 0; y < BOARD_Y; y++)
		for (int x = 0; x < BOARD_X; x++)
			if (b->tile[y][x].type != TILE_NONE)
				return false;
	return true;
}

int bag_count(const bag_t *b)
{
	return b->tail > b->head ? b->tail - b->head : b->head - b-> tail;
}

bool vowel(letter_t l)
{
	return l == LETTER_A ||
		l == LETTER_E ||
		l == LETTER_I ||
		l == LETTER_O ||
		l == LETTER_U ||
		l == LETTER_Y;
}

bool constant(letter_t l)
{
	return !vowel(l);
}
