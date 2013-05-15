#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "core.h"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define SCREEN_BPP	32


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
	if (!e->io.lockon)
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


