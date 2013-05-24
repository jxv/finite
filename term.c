#include <string.h>

#include "core.h"
#include "dict.h"
#include "init.h"
#include "term.h"


void print_word(letter_t *word, int len)
{
	char str[BOARD_SIZE];
	int j;

	NOT(word);

	for (j = 0; j < len; j++) {
		str[j] = 'A' + word[j] - LETTER_A;
	}
	str[j] = '\0';
	printf("[%s]\n", str);
}


void print_dict(struct dict *dict)
{
	int i;

	NOT(dict);

	printf("== Size:%ld\n", dict->num);
	for (i = 0; i < dict->num; i++) {
		print_word(dict->word[i], dict->len[i]);
	}
}


void print_action(struct action *a)
{
	NOT(a);

	switch (a->type) {
	case ACTION_INVALID: printf("action invalid\n"); break;
	case ACTION_PLACE: {
		printf("action place\n");
		switch (a->data.place.path.type) {
		case PATH_DOT: printf("path_dot\n"); break;
		case PATH_HORZ: printf("path_horz\n");break;
		case PATH_VERT: printf("path_vert\n"); break;
		default: break;
		}
		printf("score: %d\n", a->data.place.score);
		break;
	}
	default: printf("action default?\n"); break;
	}
}


void print_score(struct game *g)
{
	int i;
	
	NOT(g);
	
	for (i = 0; i < g->player_num; i++) {
		printf("PLAYER_%d: %d\n", i, g->player[i].score);
	}
}


void print_board(struct board *b)
{
	int x, y;
	char c;
	struct tile *t;
	sq_t sq;

	NOT(b);

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			t = &b->tile[y][x];
			sq = b->sq[y][x];
			switch (t->type) {
			case TILE_WILD: c = 'a' + t->letter; break;
			case TILE_LETTER: c = 'A' + t->letter; break;
			case TILE_NONE: /* fall through */
			default: {
				switch (sq) {
				case SQ_DBL_LET: c = '-'; break;
				case SQ_TRP_LET: c = '='; break;
				case SQ_DBL_WRD: c = '+'; break;
				case SQ_TRP_WRD: c = '#'; break;
				case SQ_FREE: c = '$'; break;
				case SQ_NORMAL: /* fall through */
				default: c = '.'; break;
				}
			}
			}
			printf("%c",c);
		}
		printf("\n");
	}
}


void print_rack(struct player *p)
{
	int i;
	char c;
	struct tile *t;

	NOT(p);

	for (i = 0; i < RACK_SIZE; i++) {
		t = &p->tile[i];
		if (t->type != TILE_NONE) {
			switch(t->type) {
			case TILE_WILD: c = '*'; break;
			case TILE_LETTER: c = 'A' + t->letter; break;
			default: c = ' '; break;
			}
			printf("(%d:%c)", i, c);
		}
	}
}


void print_place(struct place *p)
{
	int i;
	
	NOT(p);

	for (i = 0; i < p->num; i++) {
		printf("(%d,%d,%d) ", p->coor[i].x, p->coor[i].y, p->rack_id[i]);
	}
	printf("\n");
}


void print_action_error(action_error_t error)
{
	switch(error) {
	case ACTION_ERROR_PLACE_OUT_OF_RANGE: printf("[error: out of range]"); break;
	case ACTION_ERROR_PLACE_SELF_OVERLAP: printf("[error: self overlap]"); break;
	case ACTION_ERROR_PLACE_BOARD_OVERLAP: printf("[error: board overlap]"); break;
	case ACTION_ERROR_PLACE_INVALID_RACK_ID: printf("[error: invalid rack id]"); break;
	case ACTION_ERROR_PLACE_INVALID_SQ: printf("[error: place on free sq. or adjacent to a tile]");
	case ACTION_ERROR_PLACE_NO_RACK: printf("[error: no tiles placed on the board]");
	case ACTION_ERROR_PLACE_NO_DIR: printf("[error: tiles don't form a continuous line]");
	case ACTION_ERROR_PLACE_INVALID_PATH: printf("[error: invalid path, misspelled words?]");
	case ACTION_ERROR_NONE: /* fall through */
	default: break;
	}
}


int get_line(char *line, size_t s)
{
	int c, len;

	NOT(line);

	fgets(line, s, stdin);
	line[strcspn(line, "\n")] = '\0';
	len = strlen(line);
	if (len == s - 1) {
	        while ((c = getchar()) != '\n' && c != EOF);
	}
	return len;
}


bool term_init(struct game *g)
{
	NOT(g);

	g->turn = 0;
	g->player_num = 2;
	if (!load_dict(&g->dict, RES_PATH "dict.txt")) {
		return false;
	}
	init_board(&g->board);
	init_bag(&g->bag);
	init_player(&g->player[0]);
	init_player(&g->player[1]);
	return true;
}


int find_next_char(const char *str, int len, const char c)
{
	int i;

	NOT(str);

	for (i = 0; i < len; i++) {
		if (str[i] == c) {
			return i;
		}
	}
	return -1;
}


bool parse_to_place(struct place *p, const char *str, int len)
{
	int x, y, r;
	int i, j, k;
	size_t  n;

	NOT(p), NOT(str);
	
	n = 0;
	i = 0;
	k = 0;
	for (;;) {
		if (k == RACK_SIZE) {
			break;
		}
		j = find_next_char(str + i, len - i, '(');
		if (j == -1) {
			break;	/* n should be 3 */
		} else {
			i += j;
		}
		n = sscanf(str + i, "(%d,%d,%d)", &x, &y, &r);
		if (n == 3) {
			p->rack_id[k] = r;
			p->coor[k].x  = x;
			p->coor[k].y  = y;
			k++;
		} else {
			break;	/* n should NOT be 3 */
		}
		i++;
	}
	p->num = k;
	return n == 3 && k < RACK_SIZE;	/* n == 3 && k < RACK_SIZE, on success */
}


void term_get_move_type(struct move *m)
{
	char line[256];

	NOT(m);

	clr_move(m);
	printf("\n0: MOVE_PLACE\n1: MOVE_SKIP\n2: MOVE_DISCARD\n3: MOVE_QUIT\n");
	do {
		int i;
		m->type = MOVE_INVALID;
		get_line(line, sizeof(line));
		if (sscanf(line, "%d", &i) == 1) {
			switch (i) {
			case  0: m->type = MOVE_PLACE; break;
			case  1: m->type = MOVE_SKIP; break;
			case  2: m->type = MOVE_DISCARD; break;
			case  3: m->type = MOVE_QUIT; break;
			default: m->type = MOVE_INVALID; break;
			}
		}
	} while (m->type == MOVE_INVALID);
}


void term_move(struct move *m)
{
	NOT(m);

	switch (m->type) {
	case MOVE_PLACE: {
		char line[256];
		printf("Enter the rack index of tiles to place (x,y,rack-index):\n");
		get_line(line, sizeof(line));
		if (!parse_to_place(&m->data.place, line, strlen(line))) {
			printf("[error: bad input format]\n");
			m->type = MOVE_INVALID;
		} 
		break;
	}
	case MOVE_SKIP: {
		break;
	}
	case MOVE_DISCARD: {
		break;
	}
	case MOVE_QUIT: {
		break;
	}
	default: break;
	}
}


int term_ui()
{
	struct game g;
	struct move m;
	struct action a;

	if (!term_init(&g)) {
		return 1;
	}
	puts("======");
	puts("SCBAS");
	puts("======");
	for (;;) {
		print_score(&g);
		puts("===============");
		printf("Turn: PLAYER_%d\n", g.turn);
		puts("===============");
		print_board(&g.board);
		print_rack(&g.player[g.turn]);
		do {
			do {
				m.player_id = g.turn;
				term_get_move_type(&m);
				term_move(&m);
			} while (m.type == MOVE_INVALID);
			clr_action(&a);
			m.player_id = g.turn;
			mk_action(&a, &g, &m);
			if (a.type == ACTION_INVALID) {
				print_action_error(a.data.error);
			}
		} while(a.type == ACTION_INVALID);
		a.player_id = g.turn;
		if (apply_action(&g, &a)) {
			next_turn(&g);
		}
	}
	unload_dict(&g.dict);
	return 0;
}


