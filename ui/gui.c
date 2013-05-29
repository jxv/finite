#include "gui.h"
#include "init.h"


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


bool load_fontmap(struct font *f, int w, int h, const char *filename)
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


bool init_io(struct io *io)
{
	NOT(io);

	return false;
}


bool init(struct env *e)
{
	int i;
	char str[32];
	SDL_Surface *tile;

	NOT(e);

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return false;
	}
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("scabs", NULL);
	e->io.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
			SDL_SWSURFACE);

	if (e->io.screen == NULL) {
		return false;
	}

	if (!init_dict(&e->game.dict, RES_PATH "dict.txt")) {
		return false;
	}
	
	e->io.back = load_surface(RES_PATH "back.png");
	if (!e->io.back) {
		return false;
	}

	e->io.lockon = load_surface(RES_PATH "lockon.png");
	if (!e->io.lockon) {
		return false;
	}

	if (!load_fontmap(&e->io.white_font, 6, 12, RES_PATH "white_font.png")) {
		return false;
	}

	if (!load_fontmap(&e->io.black_font, 6, 12, RES_PATH "black_font.png")) {
		return false;
	}
	
	tile = load_surface(RES_PATH "tile.png");
	if (!tile) {
		return false;
	}
	e->io.wild = cpy_surface(tile);
	for (i = 0; i < LETTER_COUNT; i++) {
		e->io.tile[TILE_WILD][i] = cpy_surface(tile);
		if (!e->io.tile[TILE_WILD][i]) {
			return false;
		}
		e->io.tile[TILE_LETTER][i] = cpy_surface(tile);
		if (!e->io.tile[TILE_LETTER][i]) {
			return false;
		}
		sprintf(str,"%c", i + 'a');
		draw_str(e->io.tile[TILE_WILD][i], &e->io.black_font, str, 3, 0);
		sprintf(str,"%c", i + 'A');
		draw_str(e->io.tile[TILE_LETTER][i], &e->io.black_font, str, 3, 0);
	}
	free_surface(tile);
	init_board(&e->game.board);
	init_bag(&e->game.bag);
	init_player(&e->game.player[0], &e->game.bag);
	e->selection.type = SELECTION_BOARD;
	return true;
}


void quit(struct env *e)
{
	int i;

	NOT(e);

	quit_dict(&e->game.dict);
	free_surface(e->io.screen);
	free_surface(e->io.back);
	free_surface(e->io.lockon);
	free_surface(e->io.wild);
	for (i = 0; i < LETTER_COUNT; i++) {
		free_surface(e->io.tile[TILE_WILD][i]);
		free_surface(e->io.tile[TILE_LETTER][i]);
	}
	unload_fontmap(&e->io.white_font);
	unload_fontmap(&e->io.black_font);
	SDL_Quit();
}


bool handle_event(struct controls *c)
{
	SDL_Event event;
	Uint8 *ks;

	NOT(c);

	ks = SDL_GetKeyState(NULL);
	NOT(ks);
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: return true;
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				return true;
			}
			ks[event.key.keysym.sym] = true;
			break;
		}
		case SDL_KEYUP: ks[event.key.keysym.sym] = false; break;
		default: break;
		}
	}
	c->up    = ks[SDLK_UP];
	c->down  = ks[SDLK_DOWN];
	c->left  = ks[SDLK_LEFT];
	c->right = ks[SDLK_RIGHT];
	c->a = ks[SDLK_a];
	c->b = ks[SDLK_b];
	c->x = ks[SDLK_x];
	c->y = ks[SDLK_y];
	return false;
}


void selection_board(struct selection *s, struct controls *c)
{	
	NOT(s), NOT(c);
	assert(s->type == SELECTION_BOARD);

	if (c->up == KEYSTATE_PRESSED) {
		s->data.board.y--;
	}
	if (c->down == KEYSTATE_PRESSED) {
		s->data.board.y++;
	}
	if (c->left == KEYSTATE_PRESSED) {
		s->data.board.x--;
		s->data.board.x += BOARD_X;
		s->data.board.x %= BOARD_X; 
	}
	if (c->right == KEYSTATE_PRESSED) {
		s->data.board.x++;
		s->data.board.x %= BOARD_X; 
	}
	if (s->data.board.y < 0) {
		s->data.board.y = 0;
	}
	if (s->data.board.y >= BOARD_Y) {
		/* note: find constants for 0, 5, 6, 14? */
		if (s->data.board.x <=  5) {
			int choice = s->data.board.x - (0+1);
			s->type = SELECTION_CHOICE;
			if (choice < 0) {
				choice = 0;
			}
			if (choice >= CHOICE_COUNT) {
				choice = CHOICE_COUNT - 1;
			}
			s->data.choice = choice;
		}
		if (s->data.board.x >= 6) {
			int rack = s->data.board.x - (6+1);
			s->type = SELECTION_RACK;
			if (rack < 0) {
				rack = 0;
			}
			if (rack >= RACK_SIZE) {
				rack = RACK_SIZE - 1;
			}
			s->data.rack = rack;
		}
	}
}


void selection_rack(struct selection *s, struct controls *c)
{
	NOT(s), NOT(c);
	assert(s->type == SELECTION_RACK);

	if (c->up == KEYSTATE_PRESSED) {
		int x = s->data.rack + 7;
		s->type = SELECTION_BOARD;
		s->data.board.y = BOARD_Y - 1;
		s->data.board.x = x;
		return;
	}
	if (c->left == KEYSTATE_PRESSED) {
		s->data.rack --;
	}
	if (c->right == KEYSTATE_PRESSED) {
		s->data.rack ++;
	}
	if (s->data.rack < 0) {
		s->type = SELECTION_CHOICE;
		s->data.choice = CHOICE_COUNT - 1;
		return;
	}
	if (s->data.rack >= RACK_SIZE) {
		s->data.rack = RACK_SIZE - 1;
		return;
	}
}


void selection_choice(struct selection *s, struct controls *c)
{
	NOT(s), NOT(c);
	assert(s->type == SELECTION_CHOICE);

	if (c->up == KEYSTATE_PRESSED) {
		int x = s->data.choice + 1;
		s->type = SELECTION_BOARD;
		s->data.board.y = BOARD_Y - 1;
		s->data.board.x = x;
		return;
	}
	if (c->left == KEYSTATE_PRESSED) {
		s->data.choice --;
	}
	if (c->right == KEYSTATE_PRESSED) {
		s->data.choice ++;
	}
	if (s->data.choice < 0) {
		s->data.choice = 0;
		return;
	}
	if (s->data.choice >= CHOICE_COUNT) {
		s->type = SELECTION_RACK;
		s->data.rack = 0;
		return;
	}
}


void update(struct env *e)
{
	NOT(e);

	switch (e->selection.type) {
	case SELECTION_BOARD : selection_board(&e->selection, &e->controls); break;
	case SELECTION_RACK:   selection_rack(&e->selection, &e->controls); break;
	case SELECTION_CHOICE: selection_choice(&e->selection, &e->controls); break;
	default: break;
	}
}


void draw_selection(struct io *io, struct selection *s)
{
	int x, y;

	NOT(io), NOT(s);

	x = 0;
	y = 0;
	switch (s->type) {
	case SELECTION_BOARD: {
		x = 106 + 14 * s->data.board.x;
		y = 6 + 14 * s->data.board.y;
		break;
	}
	case SELECTION_RACK: {
		x = 162 + 14 * s->data.rack;
		y = 222;
		break;
	}
	case SELECTION_CHOICE: {
		x = 80 + 14 * s->data.choice;
		y = 222;
		break;
	}
	default: break;
	}
	draw_surface(io->screen, io->lockon, x + -2, y + -2);
}


void draw(struct env *e)
{
	NOT(e);

	SDL_FillRect(e->io.screen, NULL, 0);
	draw_surface(e->io.screen, e->io.back, 0, 0);
	draw_board(&e->io, &e->game.board);
	draw_rack(&e->io, e->game.player + e->game.turn);
	draw_selection(&e->io, &e->selection);
	SDL_Flip(e->io.screen);
}


void exec(struct env *e)
{
	int st, q = 0;
	e->game.turn = 0;

	NOT(e);

	do {
		st = SDL_GetTicks();
		q = handle_event(&e->controls);
		update(e);
		draw(e);
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


