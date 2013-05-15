#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


#define NOT(t)	assert((t))
#define RANGE(a,b,c)	assert((a) >= (b) && (a) <= (c))
#define XRANGE(a,b,c)	assert((a) > (b) && (a) < (c))


#define BOARD_X	15
#define BOARD_Y	15
#define BOARD_SIZE	BOARD_Y	/* Max among BOARD_X and BOARD_Y */
#define RACK_SIZE	7
#define BAG_SIZE	(100+1)	/* needs extra element for size check */
#define MAX_PLAYER	4

#define VALID_BOARD_X(x)	RANGE(x,0,BOARD_X-1)
#define VALID_BOARD_Y(y)	RANGE(y,0,BOARD_Y-1)
#define VALID_BOARD_SIZE(s)	RANGE(s,0,BOARD_SIZE-1)

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define SCREEN_BPP	32


typedef enum
{
	FALSE = 0,
	TRUE = 1
} bool;


typedef enum
{
	TILE_NONE = -1,
	TILE_WILD = 0,
	TILE_LETTER,
	TILE_COUNT
} tile_t;


typedef enum
{
	SQ_NORMAL = 0,
	SQ_DBL_LET,
	SQ_DBL_WRD,
	SQ_TRP_LET,
	SQ_TRP_WRD,
	SQ_COUNT
} sq_t;


typedef enum
{
	LETTER_A = 0,
	LETTER_B,
	LETTER_C,
	LETTER_D,
	LETTER_E,
	LETTER_F,
	LETTER_G,
	LETTER_H,
	LETTER_I,
	LETTER_J,
	LETTER_K,
	LETTER_L,
	LETTER_M,
	LETTER_N,
	LETTER_O,
	LETTER_P,
	LETTER_Q,
	LETTER_R,
	LETTER_S,
	LETTER_T, 
	LETTER_U,
	LETTER_V,
	LETTER_W,
	LETTER_X,
	LETTER_Y,
	LETTER_Z,
	LETTER_COUNT
} letter_t;


typedef enum
{
	MOVE_INVALID = -1,
	MOVE_PLACE = 0,
	MOVE_SWAP,
	MOVE_SKIP,
	MOVE_COUNT
} move_t;


typedef enum
{
	DIR_INVALID = -1,
	DIR_RIGHT = 0,
	DIR_DOWN,
	DIR_COUNT
} dir_t;


typedef enum
{
	PATH_INVALID = -1,
	PATH_DOT = 0,
	PATH_HORZ,
	PATH_VERT,
	PATH_COUNT
} path_t;


typedef enum
{
	ACTION_INVALID = -1,
	ACTION_PLACE = 0,
	ACTION_SWAP,
	ACTION_SKIP,
	ACTION_COUNT
} action_t;


struct tile
{
	tile_t type;
	letter_t letter;
};


struct board
{
	struct tile tile[BOARD_Y][BOARD_X];
	sq_t sq[BOARD_Y][BOARD_X];
};


struct player
{
	int score;
	struct tile tile[RACK_SIZE];
};


struct coor
{
	int x;
	int y;
};


struct place
{
	int num;
	int rack_id[RACK_SIZE];
	struct coor coor[RACK_SIZE];
};


struct swap
{
	int num;
	int rack_id[RACK_SIZE];
};


struct move
{
	move_t type;
	int player_id;
	union {
		struct place place;
		struct swap swap;
	} data;
};


struct bag
{
	int head;
	int tail;
	struct tile tile[BAG_SIZE];
};


struct dir
{
	dir_t type;
	int x;
	int y;
	int length;
	int pos[BOARD_SIZE];
};


struct path
{
	path_t type;
	union {
		struct {
			struct dir right;
			struct dir down; 
		} dot;
		struct {
			struct dir right;
			struct dir down[BOARD_X];
		} horz;
		struct {
			struct dir right[BOARD_Y];
			struct dir down;
		} vert;
	} data;
	struct board board;
};


struct action
{
	action_t type;
	int player_id;
	union {
		struct {
			int score;
			int num;
			int rack_id[RACK_SIZE];
			struct path path;
		} place;
		struct swap swap;
		
	} data;
};


struct dictionary
{
	long num;
	letter_t **word;
	int *len;
};


struct game
{
	int turn;
	int player_num;
	struct player player[MAX_PLAYER];
	struct board board;
	struct bag bag;
	struct dictionary dictionary;
};


/* tile_functions */


int tile_score(struct tile *t)
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


/* dir_functions */


bool can_use_dbl_let(struct board *b, struct dir *s, int p, int x, int y)
{
	NOT(b), NOT(s), VALID_BOARD_SIZE(p), VALID_BOARD_X(x), VALID_BOARD_Y(y);
	return s->pos[p] && b->sq[y][x] == SQ_DBL_LET;
}


bool can_use_trp_let(struct board *b, struct dir *s, int p, int x, int y)
{
	NOT(b), NOT(s), VALID_BOARD_SIZE(p), VALID_BOARD_X(x), VALID_BOARD_Y(y);
	return s->pos[p] && b->sq[y][x] == SQ_TRP_LET;
}


bool can_use_dbl_wrd(struct board *b, struct dir *s, int p, int x, int y)
{
	NOT(b), NOT(s), VALID_BOARD_SIZE(p), VALID_BOARD_X(x), VALID_BOARD_Y(y);
	return s->pos[p] && b->sq[y][x] == SQ_DBL_WRD;
}
	

bool can_use_trp_wrd(struct board *b, struct dir *s, int p, int x, int y)
{
	NOT(b), NOT(s), VALID_BOARD_SIZE(p), VALID_BOARD_X(x), VALID_BOARD_Y(y);
	return s->pos[p] && b->sq[y][x] == SQ_TRP_WRD;
}


int score_dir(struct board *b, struct dir *d)
{
	int dw, tw, x, y, score;
	NOT(b), NOT(d);
	dw = 0;
	tw = 0;
	x = d->x;
	y = d->y;
	score = 0;
	switch (d->type) {
	case DIR_RIGHT: {
		int p, i, t;
		for (p = 0, i = d->x; i < d->length + d->x; p++, i++) {
			t   = tile_score(&b->tile[y][i]);
			t  *= can_use_dbl_let(b, d, p, i, y) ? 2 : 1;
			t  *= can_use_trp_let(b, d, p, i, y) ? 3 : 1;
			dw += can_use_dbl_wrd(b, d, p, i, y);
			tw += can_use_trp_wrd(b, d, p, i, y);
			score += t;
		}
		break;
	}
	case DIR_DOWN: {
		int p, i, t;
		for (p = 0, i = d->y; i < d->length + d->y; p++, i++) {
			t   = tile_score(&b->tile[i][x]);
			t  *= can_use_dbl_let(b, d, p, x, i) ? 2 : 1;
			t  *= can_use_trp_let(b, d, p, x, i) ? 3 : 1;
			dw += can_use_dbl_wrd(b, d, p, x, i);
			tw += can_use_trp_wrd(b, d, p, x, i);
			score += t;
		}
		break;
	}
	default: break;
	}
	if (dw > 0)
		score *= dw * 2;
	if (tw > 0)
		score *= tw * 3;
	return score;
}


/* path_functions */


int score_meta_path(struct dir *d, struct dir *adj, int n, struct board *b)
{
	int i, s;
	NOT(d), NOT(adj), NOT(b);
	s = 0;
	if (d->type != DIR_INVALID) {
		s = score_dir(b, d);
		for (i = 0; i < n; i++)
			if (adj[i].type != DIR_INVALID)
				s += score_dir(b, adj+i);
		
	}
	return s;
}


int score_path(struct path *p)
{
	int score;
	struct board *b;
	NOT(p);
	score = 0;
	b = &p->board;
	switch (p->type) {
	case PATH_DOT:
		if (p->data.dot.right.type == DIR_RIGHT)
			score  = score_dir(b, &p->data.dot.right);
		if (p->data.dot.down.type == DIR_DOWN)
			score += score_dir(b, &p->data.dot.down);
		break;
	case PATH_HORZ:
		score = score_meta_path(&p->data.horz.right, p->data.horz.down, BOARD_X, b);
		break;
	case PATH_VERT:
		score = score_meta_path(&p->data.vert.down, p->data.vert.right, BOARD_Y, b);
		break;
	default: break;
	}
	return score;
}


/* bag_functions 
 * `struct bag' is implemented as a fixed-sized, circular queue.
 * Be careful not to add more tiles than BAG_SIZE-1.
 */


int bag_full(struct bag *b)
{
	NOT(b);
	return (b->head + 1) % BAG_SIZE == b->tail;
}


int bag_empty(struct bag *b)
{
	NOT(b);
	return b->head == b->tail;
}


int bag_size(struct bag *b)
{
	NOT(b);
	if (b->head > b->tail)
		return b->head - b->tail;
	return b->tail - b->head;
}


struct tile bag_peek(struct bag *b)
{
	struct tile next;
	NOT(b);
	next.type = TILE_NONE;
	if (b->head != b->tail)
		next = b->tile[b->head];
	return next;
}


void bag_drop(struct bag *b)
{
	NOT(b);
	b->head ++;
	b->head %= BAG_SIZE;
}


void bag_add(struct bag *b, struct tile t)
{
	NOT(b);
	b->tile[b->tail] = t;
	b->tail ++;
	b->tail %= BAG_SIZE;
}


void shake_bag(struct bag *b)
{
	/* todo: implement TOMORROW! @ procrastinators conference */
}


int cmp_word(letter_t w0[], int len0, letter_t w1[], int len1) 
{
	/* returns | w0 >  w1 = -1
	 *         | w0 <  w1 =  1
	 *         | w0 == w1 =  0
	 */
	int i;
	for (i = 0; ; i++) {
		if      (len0 >  len1 && i == len1)
			return -1;
		else if (len0 <  len1 && i == len0)
			return  1;
		else if (len0 == len1 && i == len0)
			return  0;
		if (w0[i] == w1[i])
			continue;
		else if (w0[i] > w1[i])
			return -1;
		else
			return  1;
	}
	return 0;
}


int valid_word(letter_t word[], int len, struct dictionary *dict)
{
	/* Glorified binary search */
	long min = 0;
	long max = dict->num;
	long mid = dict->num / 2;
	for (;;) {
		switch (cmp_word(word, len, dict->word[mid], dict->len[mid])) {
		case 0: return 1;
		case -1: min = mid + 1; break;
		case  1: max = mid - 1; break;
		default: return 0;
		}
		mid = (min + max) / 2;
		if (min > max)
			return 0;
	}
	/* Should never reach to this point */
	return 0;
}


int valid_dir(struct dir *dir, struct board *b, struct dictionary *dict)
{
	int x, y, len;
	letter_t word[BOARD_SIZE];
	NOT(dir), NOT(b), NOT(dict);
	x = dir->x;
	y = dir->y;
	len = dir->length;
	switch (dir->type) {
	case DIR_RIGHT: {
		int i;
		for (i = 0; i < len; i++) {
			if (b->tile[y][x+i].type != TILE_NONE)
				word[i] = b->tile[y][x+i].letter;
			else
				return 0;
		}
		break;
	}
	case DIR_DOWN: {
		int i;
		for (i = 0; i < len; i++) {
			if (b->tile[y+i][x].type != TILE_NONE)
				word[i] = b->tile[y+i][x].letter;
			else
				return 0;
		}
		break;
	}
	default: return 0;
	}
	return valid_word(word, len, dict);
}


/* valid_meta_path */

int valid_path(struct path *p, struct dictionary *d)
{
	int i;
	switch (p->type) {
	case PATH_DOT:
		if (!valid_dir(&p->data.dot.right, &p->board, d))
			return 0;
		if (!valid_dir(&p->data.dot.down, &p->board, d))
			return 0;
		break;
	case PATH_HORZ:
		if (!valid_dir(&p->data.horz.right, &p->board, d))
			return 0;
		for (i = 0; i < BOARD_X; i++)
			if (p->data.horz.down[i].type == DIR_DOWN &&
			   (!valid_dir(&p->data.horz.down[i], &p->board, d)))
				return 0;
		break;
	case PATH_VERT:
		if (!valid_dir(&p->data.vert.down, &p->board, d)) 
			return 0;
		for (i = 0; i < BOARD_Y; i++)
			if (p->data.vert.right[i].type == DIR_RIGHT &&
			   (!valid_dir(&p->data.vert.right[i], &p->board, d))) 
				return 0;
		break;
	default: return 0;
	}
	return 1;
}


/* act_move_functions */


int cpy_rack_board(struct board *b, struct place *place, struct player *player)
{
	int x, y, i, r;
	for (i = 0; i < place->num; i++) {
		r = place->rack_id[i];
		y = place->coor[i].y;
		x = place->coor[i].x;
		if (player->tile[r].type != TILE_NONE) {
			if (b->tile[y][x].type != TILE_NONE)
				return 1;
			memcpy(&b->tile[y][x], &player->tile[r],
					sizeof(struct tile));
		}
	}
	return 0;
}


int is_horz(struct action *a, struct move *m)
{
	int i, min, max, y;
	struct coor *coor;
	struct board *board = &a->data.place.path.board;
	y = m->data.place.coor[0].y;
	min = max = m->data.place.coor[0].x;
	if (m->data.place.num < 2)
		return 0;
	for (i = 1; i < m->data.place.num; i++) {
		coor = &m->data.place.coor[i];
		if (y != coor->y)
			return 0;
		if (min < coor->x)
			min = coor->x;
		if (max > coor->x)
			max = coor->x;
	}
	for (i = min; i <= max; i++)
		if (board->tile[y][i].type == TILE_NONE)
			return 0;
	return 1;
}


int is_vert(struct action *a, struct move *m)
{
	int i, min, max, x;
	struct coor *coor;
	struct board *board = &a->data.place.path.board;
	x = m->data.place.coor[0].x;
	min = max = m->data.place.coor[0].y;
	if (m->data.place.num < 2)
		return 0;
	for (i = 1; i < m->data.place.num; i++) {
		coor = &m->data.place.coor[i];
		if (x != coor->x)
			return 0;
		if (min > coor->y)
			min = coor->y;
		if (max < coor->y)
			max = coor->y;
	}
	for (i = min; i <= max; i++)
		if (board->tile[i][x].type == TILE_NONE)
			return 0;
	return 1;
}


void mk_right(struct dir *d, int x, int y, struct board *b)
{
	int i;
	d->type = DIR_RIGHT;
	d->y = y;
	memset(d->pos, 0, sizeof(int) * BOARD_SIZE);
	d->pos[x] = 1;
	for (i = x; i >= 0 && b->tile[y][i].type != TILE_NONE; i--)
		d->x = i;
	for (i = x; i < BOARD_X && b->tile[y][i].type != TILE_NONE; i++)
		d->length = i;
	d->length -= d->x - 1;
	if (d->length == 1)
		 d->type = DIR_INVALID;
}


void mk_down(struct dir *d, int x, int y, struct board *b)
{
	int i;
	d->type = DIR_DOWN;
	d->x = x;
	memset(d->pos, 0, sizeof(int) * BOARD_SIZE);
	d->pos[y] = 1;
	for (i = y; i >= 0 && b->tile[i][x].type != TILE_NONE; i--)
		d->y = i;
	for (i = y; i < BOARD_Y && b->tile[i][x].type != TILE_NONE; i++)
		d->length = i;
	d->length -= d->y - 1;
	if (d->length == 1)
		 d->type = DIR_INVALID;
}


void mk_dot(struct action *a, struct move *m)
{
	int x = m->data.place.coor[0].x;
	int y = m->data.place.coor[0].y;
	struct path *path = &a->data.place.path; 
	struct board *b = &a->data.place.path.board;
	struct dir *d = NULL;
	path->type = PATH_DOT;
	d = &path->data.dot.right;
	mk_right(d, x, y, b);
	d = &path->data.dot.down;
	mk_down(d, x, y, b);
}


void mk_horz(struct action *a, struct move *m)
{
	int i;
	int x = m->data.place.coor[0].x;
	int y = m->data.place.coor[0].y;
	struct path *path = &a->data.place.path; 
	struct board *b = &a->data.place.path.board;
	struct dir *d = NULL;
	path->type = PATH_HORZ;
	d = &path->data.horz.right;
	mk_right(d, x, y, b);
	for (i = 0; i < m->data.place.num; i++) {
		x = m->data.place.coor[i].x;
		d->pos[x] = 1;
	}
	for (i = 0; i < BOARD_X; i++)
		path->data.horz.down[i].type = DIR_INVALID;
	for (i = 0; i < m->data.place.num; i++) {
		d = &path->data.horz.down[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mk_down(d, x, y, b);
	}
}


void mk_vert(struct action *a, struct move *m)
{
	int i;
	int x = m->data.place.coor[0].x;
	int y = m->data.place.coor[0].y;
	struct path *path = &a->data.place.path; 
	struct board *b = &a->data.place.path.board;
	struct dir *d = NULL;
	path->type = PATH_VERT;
	d = &path->data.vert.down;
	mk_down(d, x, y, b);
	for (i = 0; i < m->data.place.num; i++) {
		y = m->data.place.coor[i].y;
		d->pos[y] = 1;
	}
	for (i = 0; i < BOARD_Y; i++)
		path->data.vert.right[i].type = DIR_INVALID;
	for (i = 0; i < m->data.place.num; i++) {
		d = &path->data.vert.right[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mk_right(d, x, y, b);
	}
}


void mk_place(struct action *a, struct game *g, struct move *m)
{
	/* copy current board and recently placed tiles */
	/* note: not-TILE_NONE impiles TILE_WILD or TILE_LETTER */
	struct path *path = &a->data.place.path;
	a->type = ACTION_PLACE;
	a->data.place.num = m->data.place.num;
	memcpy(&path->board, &g->board, sizeof(struct board));
	if (cpy_rack_board(&path->board, &m->data.place,
			&g->player[m->player_id])) {
		a->type = ACTION_INVALID;
		return;
	}
	switch (m->data.place.num) {
	case 0:
		a->type = ACTION_INVALID;
		return;
	case 1:
		mk_dot(a, m);
		break;
	default:
		if (is_horz(a, m)) {
			mk_horz(a, m);
		} else if (is_vert(a, m)) {
			mk_vert(a, m);
		} else {
			a->type = ACTION_INVALID;
			return;
		}
		break;
	}
	if (!valid_path(path, &g->dictionary)) {
		a->type = ACTION_INVALID;
		return;
	}
	a->data.place.score = score_path(path);
}


void mk_action(struct action *a, struct game *g, struct move *m)
{
	a->player_id = m->player_id;
	switch (m->type) {
	case MOVE_INVALID:
		a->type = ACTION_INVALID;
		break;
	case MOVE_PLACE:
		mk_place(a, g, m);
		break;
	case MOVE_SWAP:
		a->type = ACTION_INVALID;
		if (bag_size(&g->bag) >= m->data.swap.num) {
			a->type = ACTION_SWAP;
			memcpy(&a->data.swap, &m->data.swap,
					sizeof(struct swap));
		}
		break;
	case MOVE_SKIP:
		a->type = ACTION_SKIP;
		break;
	default:break;
	}
}


/* */


void apply_action(struct game *g, struct action *a)
{
	int pid = a->player_id;
	if (pid != g->turn)
		return;
	switch (a->type) {
	case ACTION_PLACE: {
		int i, r;
		memcpy(&g->board, &a->data.place.path.board,
				sizeof(struct board));
		g->player[pid].score += a->data.place.score;
		for (i = 0; i < a->data.place.num; i++) {
			r = a->data.place.rack_id[i];
			g->player[pid].tile[r].type = TILE_NONE;
		}
		break;
	}
	case ACTION_SWAP: {
		struct tile *t;
		int i, r;
		for (i = 0; i < a->data.swap.num; i++) {
			r = a->data.swap.rack_id[i];
			t = &g->player[pid].tile[r];
			bag_add(&g->bag, *t);
			*t = bag_peek(&g->bag);
			bag_drop(&g->bag);
		}
		shake_bag(&g->bag);
		break;
	}
	case ACTION_SKIP:
	default: break;
	}
}


void next_turn(struct game *g)
{
	g->turn ++;
	g->turn %= g->player_num;
}





int strlen_as_word(char str[])
{
	int i, len = 0;
	char c;
	for (i = 0; str[i] != '\0'; i++) {
		c = toupper(str[i]);
		if (c >= 'A' && c <= 'Z') 
			len++;
	}
	return len;
}


void word_from_str(letter_t word[], char *str)
{
	int i, j;
	char c;
	for (i = 0, j = 0; str[i] != '\0'; i++) {
		c = toupper(str[i]);
		if (c >= 'A' && c <= 'Z') {
			word[j] = LETTER_A + c - 'A';
			j++;
		}
	}
}


void swap_words(letter_t **w0, int *len0, letter_t **w1, int *len1)
{
	int tmp_len = *len0;
	letter_t *tmp_w = *w0;
	*len0 = *len1;
	*w0 = *w1;
	*len1 = tmp_len; 
	*w1 = tmp_w;
}


int load_dictionary(struct dictionary *dict, const char *filename)
{
	long i, j;
	FILE *f = NULL;
	char buf[BOARD_SIZE+1];
	f = fopen(filename,"r");
	if (f == NULL)
		return 0;
	/* count */
	dict->num = 0;
	while(fgets(buf, BOARD_SIZE+1, f)) {
		if (strlen_as_word(buf) > 1)
			dict->num ++;
	}
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return 0;
	}
	rewind(f);
	/* alloc */
	dict->word = malloc(sizeof(letter_t*) * dict->num);
	dict->len = malloc(sizeof(long) * dict->num);
	for (i = 0; i < dict->num; i++)
		dict->word[i] = malloc(sizeof(letter_t) * BOARD_SIZE);
	/* assign */
	i = 0;
	for(i = 0; i < dict->num && fgets(buf, BOARD_SIZE+1, f); i++) {
		j = strlen_as_word(buf);
		if (j <= 1) {
			i--;
			continue;
		}
		word_from_str(dict->word[i], buf);
		dict->len[i] = j;
	}
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return 0;
	}
	fclose(f);
	/* sort */
	return 1;
	for (i = 0; i < dict->num; i++) {
		for (j = 0; j < dict->num; j++) {
			if (cmp_word(dict->word[i], dict->len[i],
				     dict->word[j], dict->len[j]) == 1) {
				swap_words(&dict->word[i], &dict->len[i],
					  &dict->word[j], &dict->len[j]);
			}
		}
	}
	return 1;
}


/* end of core functionality */


void print_word(letter_t word[], int len) {
	char str[BOARD_SIZE];
	int j;
	for (j = 0; j < len; j++) 
		str[j] = 'A' + word[j] - LETTER_A;
	str[j] = '\0';
	puts(str);
}


void print_dictionary(struct dictionary *dict)
{
	int i;
	printf("== Size:%ld\n", dict->num);
	for (i = 0; i < dict->num; i++)
		print_word(dict->word[i], dict->len[i]);
}


/* */


void test();

int scabs();


int main()
{
	return scabs();
}


/** example **/


void unload_dictionary(struct dictionary *dict)
{
	long i;
	free(dict->len);
	for (i = 0; i < dict->num; i++)
		free(dict->word[i]);
	free(dict->word);
}


void init_board(struct board *b)
{
	int x, y;
	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			b->tile[y][x].type = TILE_NONE;
			b->sq[y][x] = SQ_NORMAL;
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


void print_action(struct action *a)
{
	switch (a->type) {
	case ACTION_INVALID:
		printf("action invalid\n");
		break;
	case ACTION_PLACE:
		printf("action place\n");
		switch (a->data.place.path.type) {
		case PATH_DOT:
			printf("path_dot\n");
			break;
		case PATH_HORZ:
			printf("path_horz\n");
			break;
		case PATH_VERT:
			printf("path_vert\n");
			break;
		default: break;
		}
		printf("score: %d\n", a->data.place.score);
		break;
	default: printf("action default?\n"); break;
	}
}


void print_board(struct board *b)
{
	int x, y;
	char c;
	struct tile *t;
	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			t = &b->tile[y][x];
			switch (t->type) {
			case TILE_WILD:   c = 'a' + t->letter; break;
			case TILE_LETTER: c = 'A' + t->letter; break;
			case TILE_NONE:
			default: c = '_'; break;
			}
			printf("%c",c);
		}
		printf("\n");
	}
}


void test()
{
	struct game g;
	struct move m;
	struct action a;
	g.turn = 0;
	g.player_num = 1;
	load_dictionary(&g.dictionary, "data/dictionary.txt");
	init_board(&g.board);
	init_bag(&g.bag);
	init_player(&g.player[0]);
	init_move(&m);
	mk_action(&a, &g, &m);
	print_action(&a);
	apply_action(&g, &a);
	print_board(&g.board);
	/*print_dictionary(&g.dictionary); */
	unload_dictionary(&g.dictionary);
}

/* end example */

/* aux_sdl_enum && aux_sdl_structs */


typedef enum
{
	KEY_STATE_UNTOUCHED = 0,
	KEY_STATE_PRESSED,
	KEY_STATE_HELD,
	KEY_STATE_RELEASED,
	KEY_STATE_COUNT
} KeyState;


struct font
{
	int w;
	int h;
	SDL_Surface *map;
};

/* aux_sdl_functions */


void free_surface(SDL_Surface *s)
{
	if (s) SDL_FreeSurface(s);
}

SDL_Surface *load_surface(const char *filename)
{
	/* Magenta represents transparency */
	SDL_Surface *s = NULL;
	SDL_Surface *tmp = IMG_Load(filename);
	if (tmp) {
		s = SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
	}
	if (s) {
		Uint32 a = SDL_MapRGB(s->format, 0xff, 0x00, 0xff);
		SDL_SetColorKey(s, SDL_SRCCOLORKEY, a);
	}
	return s;
} 


void delay(int st, int et, int fps)
{
	int ms = (1000 / fps) - (et - st);
	if (ms > 0)
		SDL_Delay(ms);
}


void draw_surface(SDL_Surface *s0, SDL_Surface *s1, int x, int y)
{
	SDL_Rect offset;
	NOT(s0), NOT(s1);
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface(s1, NULL, s0, &offset);
}


SDL_Surface *cpy_surface(SDL_Surface *s)
{
	SDL_Surface *cpy;
	NOT(s);	
	cpy = SDL_CreateRGBSurface(0, s->w, s->h, SCREEN_BPP, 0, 0, 0, 0);
	draw_surface(cpy,s,0,0);
	return cpy;
}


/* */


struct io
{
	SDL_Surface *screen;
	SDL_Surface *back;
	SDL_Surface *tile[2][26];
	SDL_Surface *wild;
	SDL_Surface *lockon;
	struct font white_font;
	struct font black_font;
};


struct env
{
	struct io io;
	struct game game;
};


/* */


void draw_board(struct io *io, struct board *b)
{
	int off_x, off_y, x, y, w, h, letter, type;
	NOT(io), NOT(b);
	off_x = 106;
	off_y = 6;
	w = 14;
	h = 14;
	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			type = 1;
			switch (b->tile[y][x].type) {
			case TILE_WILD: 
				type = 0;
			case TILE_LETTER:
				letter = (int)b->tile[y][x].letter;
				draw_surface(io->screen,
					     io->tile[type][letter],
					     off_x + x * w,
					     off_y + y * h);
				break;
			default: break;
			}
		}
	}
}


void draw_rack(struct io *io, struct player *p)
{
	int off_x, off_y, w, i, letter, type;
	SDL_Surface *t;
	NOT(io), NOT(p);
	off_x = 162;
	off_y = 222;
	w = 14;
	for (i = 0; i < RACK_SIZE; i++) {
		type = p->tile[i].type;
		letter = p->tile[i].letter;
		if (type == TILE_LETTER)
			t = io->tile[type][letter];
		if (type == TILE_WILD)
			t = io->wild;
		if (type != TILE_NONE)
			draw_surface(io->screen, t, off_x + i * w, off_y);
	}
}


int load_fontmap(struct font *f, int w, int h, const char *filename)
{
	NOT(f);
	f->w = w;
	f->h = h;
	f->map = load_surface(filename);
	return f->map != NULL;
}

void unload_fontmap(struct font *f)
{
	NOT(f);
	free_surface(f->map);
}


void draw_str(SDL_Surface *s, struct font *f, const char *str, int x, int y)
{
	int i;
	char c;
	SDL_Rect offset, clip;
	offset.x = x;
	offset.y = y;
	clip.y = 0;
	clip.h = f->h;
	clip.w = f->w;
	for (i = 0; str[i] != '\0'; i++) {
		c = str[i];
		if (c >= 32 && c <= 126) {
			clip.x = f->w * (c - 32);
			SDL_BlitSurface(f->map, &clip, s, &offset);
		}
		offset.x += f->w;
	}
}

/***/


#define RES_PATH "data/"

int init_io(struct io *io)
{
	return 0;
}


int init(struct env *e)
{
	int i;
	char str[32];
	SDL_Surface *tile;
	NOT(e);
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
		return 0;
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("scabs", NULL);
	e->io.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
			SDL_SWSURFACE);
	if (e->io.screen == NULL)
		return 0;
	if (!load_dictionary(&e->game.dictionary, RES_PATH "dictionary.txt"))
		return 0;
	e->io.back = load_surface(RES_PATH "back.png");
	if (!e->io.back)
		return 0;
	e->io.lockon = load_surface(RES_PATH "lockon.png");
		return 0;
	if (!load_fontmap(&e->io.white_font, 6, 12, RES_PATH "white_font.png"))
		return 0;
	if (!load_fontmap(&e->io.black_font, 6, 12, RES_PATH "black_font.png"))
		return 0;
	tile = load_surface(RES_PATH "tile.png");
	if (!tile)
		return 0;
	e->io.wild = cpy_surface(tile);
	for (i = 0; i < 26; i++) {
		e->io.tile[0][i] = cpy_surface(tile);
		e->io.tile[1][i] = cpy_surface(tile);
		sprintf(str,"%c",i+'a');
		draw_str(e->io.tile[0][i], &e->io.black_font,str, 3, 0);
		sprintf(str,"%c",i+'A');
		draw_str(e->io.tile[1][i], &e->io.black_font,str, 3, 0);
	}
	free_surface(tile);
	init_board(&e->game.board);
	init_player(&e->game.player[0]);
	return 1;
}


void quit(struct env *e)
{
	int i;
	NOT(e);
	unload_dictionary(&e->game.dictionary);
	free_surface(e->io.screen);
	free_surface(e->io.back);
	free_surface(e->io.lockon);
	free_surface(e->io.wild);
	for (i = 0; i < 26; i++) {
		free_surface(e->io.tile[0][i]);
		free_surface(e->io.tile[1][i]);
	}
	unload_fontmap(&e->io.white_font);
	unload_fontmap(&e->io.black_font);
	SDL_Quit();
}


int handle_event(struct env *e)
{
	SDL_Event event;
	Uint8 *ks;
	NOT(e);
	ks = SDL_GetKeyState(NULL);
	NOT(ks);
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: 
			return 1;
		case SDL_KEYDOWN: 
			if (event.key.keysym.sym == SDLK_ESCAPE)
				return 1;
			ks[event.key.keysym.sym] = 1;
			break;
		case SDL_KEYUP:
			ks[event.key.keysym.sym] = 0;
			break;
		default:
			break;
		}
	}
	return 0;
}


void exec(struct env *e)
{
	int st, q = 0;
	do {
		st = SDL_GetTicks();
		q = handle_event(e);
		SDL_FillRect(e->io.screen, NULL, 0);
		draw_surface(e->io.screen, e->io.back, 0, 0);
		draw_board(&e->io, &e->game.board);
		draw_rack(&e->io, e->game.player + e->game.turn);
		SDL_Flip(e->io.screen);
		delay(st, SDL_GetTicks(), 60);
	} while (!q);
}


int scabs()
{
	int exit_status = EXIT_FAILURE;
	struct env e;
	if (init(&e)) {
		exec(&e);
		exit_status = EXIT_SUCCESS;
	}
	quit(&e);
	return exit_status;
}


