#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "core.h"
#include "dict.h"
#include "init.h"
#include "terminal.h"


#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define SCREEN_BPP	32


int scabs();


int main()
{
	terminal_ui();
	return 0;
}


void unload_dict(struct dict *dict)
{
	long i;
	free_mem(dict->len);
	for (i = 0; i < dict->num; i++) {
		free_mem(dict->word[i]);
	}
	free_mem(dict->word);
}


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


void free_surface(SDL_Surface *s)
{
	if (s) {
		 SDL_FreeSurface(s);
	}
}


SDL_Surface *load_surface(const char *filename)
{
	/* Magenta represents transparency */
	SDL_Surface *s, *tmp;
	Uint32 a;
	NOT(filename);
	s = NULL;
	tmp = IMG_Load(filename);
	if (!tmp)
		return NULL;
	s = SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);
	if (!s)
		return NULL;
	a = SDL_MapRGB(s->format, 0xff, 0x00, 0xff);
	SDL_SetColorKey(s, SDL_SRCCOLORKEY, a);
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


struct controls
{
	KeyState up;
	KeyState down;
	KeyState left;
	KeyState right;
	KeyState a;
	KeyState b;
	KeyState x;
	KeyState y;
};


struct env
{
	struct io io;
	struct game game;
	struct controls controls;
};


/* */


#define TILE_SPACING_X	2
#define TILE_SPACING_Y	0


void draw_board(struct io *io, struct board *b)
{
	int off_x, off_y, x, y, w, h, letter, type;
	NOT(io), NOT(b);
	off_x = 106;
	off_y = 6;
	w = io->wild->w + TILE_SPACING_X;
	h = io->wild->h + TILE_SPACING_Y;
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
	NOT(io), NOT(p), NOT(io->wild);
	off_x = 162;
	off_y = 222;
	w = io->wild->w + TILE_SPACING_X;
	for (i = 0; i < RACK_SIZE; i++) {
		type = p->tile[i].type;
		t = NULL;
		if (type == TILE_NONE) {
			continue;
		}
		letter = p->tile[i].letter;
		if (type == TILE_LETTER) {
			assert(letter > LETTER_INVALID && letter < LETTER_COUNT);
			t = io->tile[1][letter];
		}
		if (type == TILE_WILD) {
			t = io->wild;
		}
		draw_surface(io->screen, t, off_x + i * w, off_y);
	}
}


int load_fontmap(struct font *f, int w, int h, const char *filename)
{
	NOT(f), NOT(filename);
	f->w = w;
	f->h = h;
	f->map = load_surface(filename);
	return f->map != NULL;
}


void unload_fontmap(struct font *f)
{
	NOT(f), NOT(f->map);
	free_surface(f->map);
}


void draw_str(SDL_Surface *s, struct font *f, const char *str, int x, int y)
{
	int i;
	char c;
	SDL_Rect offset, clip;
	NOT(s), NOT(f), NOT(f->map), NOT(str);
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


/*  */


int init_io(struct io *io)
{
	NOT(io);
	return 0;
}


int init(struct env *e)
{
	int i;
	char str[32];
	SDL_Surface *tile;
	NOT(e);
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return 0;
	}
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("scabs", NULL);
	e->io.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
			SDL_SWSURFACE);
	if (e->io.screen == NULL) {
		return 0;
	}

	if (!load_dict(&e->game.dict, RES_PATH "dict.txt")) {
		return 0;
	}
	
	e->io.back = load_surface(RES_PATH "back.png");
	if (!e->io.back) {
		return 0;
	}

	e->io.lockon = load_surface(RES_PATH "lockon.png");
	if (!e->io.lockon) {
		return 0;
	}

	if (!load_fontmap(&e->io.white_font, 6, 12, RES_PATH "white_font.png")) {
		return 0;
	}

	if (!load_fontmap(&e->io.black_font, 6, 12, RES_PATH "black_font.png")) {
		return 0;
	}

	tile = load_surface(RES_PATH "tile.png");
	if (!tile) {
		return 0;
	}

	e->io.wild = cpy_surface(tile);
	for (i = 0; i < 26; i++) {
		e->io.tile[0][i] = cpy_surface(tile);
		if (!e->io.tile[0][i]) {
			return 0;
		}
		e->io.tile[1][i] = cpy_surface(tile);
		if (!e->io.tile[0][i]) {
			return 0;
		}
		sprintf(str,"%c", i + 'a');
		draw_str(e->io.tile[0][i], &e->io.black_font, str, 3, 0);
		sprintf(str,"%c", i + 'A');
		draw_str(e->io.tile[1][i], &e->io.black_font, str, 3, 0);
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
	unload_dict(&e->game.dict);
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
	e->game.turn = 0;
	NOT(e);
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


