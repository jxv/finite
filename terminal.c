#include "core.h"
#include "dict.h"
#include "init.h"
#include "terminal.h"


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


void terminal_ui()
{
	struct game g;
	struct move m;
	struct action a;
	g.turn = 0;
	g.player_num = 1;
	load_dict(&g.dict, RES_PATH "dict.txt");
	init_board(&g.board);
	init_bag(&g.bag);
	init_player(&g.player[0]);
	init_move(&m);
	mk_action(&a, &g, &m);
	apply_action(&g, &a);
	print_board(&g.board);
	print_action(&a);
	printf("\n==========\nSCABS\n==========\n");
	do {
		break;
	} while (1);
	unload_dict(&g.dict);
}


