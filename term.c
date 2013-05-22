#include <string.h>

#include "core.h"
#include "dict.h"
#include "init.h"
#include "term.h"


void print_word(letter_t *word, int len) {
	char str[BOARD_SIZE];
	int j;
	NOT(word);
	for (j = 0; j < len; j++) {
		str[j] = 'A' + word[j] - LETTER_A;
	}
	str[j] = '\0';
	puts(str);
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


void print_board(struct board *b)
{
	int x, y;
	char c;
	struct tile *t;

	NOT(b);

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


void print_rack(struct player *p)
{
	NOT(p);
}


int get_line(char *line, size_t s)
{
	int c, len;

	fgets(line, s, stdin);
	line[strcspn(line, "\n")] = '\0';
	len = strlen(line);
	if (len == s - 1) {
	        while ((c = getchar()) != '\n' && c != EOF);
	}
	return len;
}


void term_init(struct game *g)
{
	NOT(g);

	g->turn = 0;
	g->player_num = 2;
	load_dict(&g->dict, RES_PATH "dict.txt");
	init_board(&g->board);
	init_bag(&g->bag);
	init_player(&g->player[0]);
	init_player(&g->player[1]);
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
		printf("Enter the rack index of tiles to discard:\n");
		{
			get_line(line, sizeof(line));
			printf("[%s]\n",line);
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

	term_init(&g);
	printf("\n==========\nSCABS\n==========\n");
	do {
		print_board(&g.board);
		print_rack(&g.player[g.turn]);
		do {
			term_get_move_type(&m);
			term_move(&m);
		} while (m.type != MOVE_INVALID);
		clr_action(&a);
		mk_action(&a, &g, &m);
		apply_action(&g, &a);
		next_turn(&g);
		break;
	} while (1);
	unload_dict(&g.dict);
	return 0;
}


