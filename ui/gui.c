#include "gui.h"
#include "init.h"
#include "widget.h"


void guiInit(struct gui *g)
{
	NOT(g);
	
	mkRackWidget(&g->gameGui.rackWidget);
	mkChoiceWidget(&g->gameGui.choiceWidget);
	mkBoardWidget(&g->gameGui.boardWidget);
}


bool fontmapInit(struct font *f, int w, int h, const char *filename)
{
	NOT(f), NOT(filename);

	f->width = w;
	f->height = h;
	f->map = surfaceLoad(filename);
	return f->map != NULL;
}


void fontmapQuit(struct font *f)
{
	NOT(f), NOT(f->map);

	surfaceFree(f->map);
}


void strDraw(SDL_Surface *s, struct font *f, const char *str, int x, int y)
{
	int i;
	char c;
	SDL_Rect offset, clip;

	NOT(s), NOT(f), NOT(f->map), NOT(str);

	offset.x = x;
	offset.y = y;
	clip.y = 0;
	clip.h = f->height;
	clip.w = f->width;
	for (i = 0; str[i] != '\0'; i++) {
		c = str[i];
		/* [32..126] are drawable ASCII chars */
		if (c >= 32 && c <= 126) {
			clip.x = f->width * (c - 32);
			SDL_BlitSurface(f->map, &clip, s, &offset);
		}
		offset.x += f->width;
	}
}


bool ioInit(struct io *io)
{
	NOT(io);

	return false;
}


void keystateInit(struct keystate *ks)
{
	NOT(ks);
	
	ks->type = KEYSTATE_UNTOUCHED;
	ks->time = 0.0f;
}


void controlsInit(struct controls *c)
{
	NOT(c);
	
	keystateInit(&c->up);
	keystateInit(&c->down);
	keystateInit(&c->right);
	keystateInit(&c->left);
	keystateInit(&c->a);
	keystateInit(&c->b);
	keystateInit(&c->x);
	keystateInit(&c->y);
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
	SDL_WM_SetCaption("finite", NULL);
	if ((e->io.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE)) == NULL) {
		return false;
	}
	if (!dictInit(&e->game.dict, RES_PATH "dict.txt")) {
		return false;
	}
	if ((e->io.back = surfaceLoad(RES_PATH "back.png")) == NULL) {
		return false;
	}
	if ((e->io.lockon = surfaceLoad(RES_PATH "lockon.png")) == NULL) {
		return false;
	}
	if ((e->io.recall = surfaceLoad(RES_PATH "recall.png")) == NULL) {
		return false;
	}
	if ((e->io.mode = surfaceLoad(RES_PATH "mode.png")) == NULL) {
		return false;
	}
	if ((e->io.place = surfaceLoad(RES_PATH "place.png")) == NULL) {
		return false;
	}
	if ((e->io.discard = surfaceLoad(RES_PATH "discard.png")) == NULL) {
		return false;
	}
	if ((e->io.skip = surfaceLoad(RES_PATH "skip.png")) == NULL) {
		return false;
	}
	if ((e->io.play = surfaceLoad(RES_PATH "play.png")) == NULL) {
		return false;
	}
	if (!fontmapInit(&e->io.white_font, 6, 12, RES_PATH "white_font.png")) {
		return false;
	}
	if (!fontmapInit(&e->io.black_font, 6, 12, RES_PATH "black_font.png")) {
		return false;
	}
	if ((tile = surfaceLoad(RES_PATH "tile.png")) == NULL) {
		return false;
	} else {
		e->io.wild = surfaceCpy(tile);
	}
	for (i = 0; i < LETTER_COUNT; i++) {
		e->io.tile[TILE_WILD][i] = surfaceCpy(tile);
		if (!e->io.tile[TILE_WILD][i]) {
			return false;
		}
		e->io.tile[TILE_LETTER][i] = surfaceCpy(tile);
		if (!e->io.tile[TILE_LETTER][i]) {
			return false;
		}
		sprintf(str,"%c", i + 'a');
		strDraw(e->io.tile[TILE_WILD][i], &e->io.black_font, str, 3, 0);
		sprintf(str,"%c", i + 'A');
		strDraw(e->io.tile[TILE_LETTER][i], &e->io.black_font, str, 3, 0);
	}
	surfaceFree(tile);
	boardInit(&e->game.board);
	bagInit(&e->game.bag);
	guiInit(&e->gui);
	playerInit(&e->game.player[0], &e->game.bag);
	controlsInit(&e->controls);
	guiInit(&e->gui);
	return true;
}


void quit(struct env *e)
{
	int i;

	NOT(e);

	dictQuit(&e->game.dict);
	surfaceFree(e->io.screen);
	surfaceFree(e->io.back);
	surfaceFree(e->io.lockon);
	surfaceFree(e->io.recall);
	surfaceFree(e->io.mode);
	surfaceFree(e->io.place);
	surfaceFree(e->io.discard);
	surfaceFree(e->io.skip);
	surfaceFree(e->io.play);
	surfaceFree(e->io.wild);
	for (i = 0; i < LETTER_COUNT; i++) {
		surfaceFree(e->io.tile[TILE_WILD][i]);
		surfaceFree(e->io.tile[TILE_LETTER][i]);
	}
	fontmapQuit(&e->io.white_font);
	fontmapQuit(&e->io.black_font);
	SDL_Quit();
}


void keystateUpdate(struct keystate *ks, bool touched)
{
	NOT(ks);

	if (touched) {
		switch(ks->type) {
		case KEYSTATE_UNTOUCHED: {
			ks->type = KEYSTATE_PRESSED;
			ks->time = 0.0f;
			break;
		}
		case KEYSTATE_PRESSED: {
			ks->type = KEYSTATE_HELD;
			ks->time = 0.0f;
			break;
		}
		case KEYSTATE_HELD: {
			ks->time += 1.0f / 60.0f;
			break;
		}
		case KEYSTATE_RELEASED: {
			ks->type = KEYSTATE_PRESSED;
			ks->time = 0.0f;
			break;
		}
		default: break;
		}
	} else {
		switch(ks->type) {
		case KEYSTATE_UNTOUCHED: {
			ks->time += 1.0f / 60.0f;
			break;
		}
		case KEYSTATE_PRESSED: {
			ks->type = KEYSTATE_RELEASED;
			ks->time = 0.0f;
			break;
		}
		case KEYSTATE_HELD: {
			ks->type = KEYSTATE_RELEASED;
			ks->time = 0.0f;
			break;
		}
		case KEYSTATE_RELEASED: {
			ks->type = KEYSTATE_UNTOUCHED;
			ks->time = 0.0f;
			break;
		}
		default: break;
		}
	}
}


bool handleEvent(struct controls *c)
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
	keystateUpdate(&c->up,    ks[SDLK_UP]);
	keystateUpdate(&c->down,  ks[SDLK_DOWN]);
	keystateUpdate(&c->left,  ks[SDLK_LEFT]);
	keystateUpdate(&c->right, ks[SDLK_RIGHT]);
	keystateUpdate(&c->a, ks[SDLK_a]);
	keystateUpdate(&c->b, ks[SDLK_b]);
	keystateUpdate(&c->x, ks[SDLK_x]);
	keystateUpdate(&c->y, ks[SDLK_y]);
	return false;
}


void update(struct env *e)
{
	/*
	Follows a MVC pattern:
	Game           -> Model
	GUI + IO       -> View
	GUI + Controls -> Controller
	*/
	NOT(e);
}


void guiDraw(struct io *io, struct gui *g)
{
	struct coor pos;
	struct coor dim;

	NOT(io), NOT(g);

	pos.x = 105;
	pos.y = 5;
	
	dim.x = 14;
	dim.y = 14;
	
	gridWidgetDraw(io->screen, &g->gameGui.boardWidget, pos, dim);
}


void draw(struct env *e)
{
	NOT(e);

	SDL_FillRect(e->io.screen, NULL, 0);
	surfaceDraw(e->io.screen, e->io.back, 0, 0);
	surfaceDraw(e->io.screen, e->io.recall, 105, 220);
	surfaceDraw(e->io.screen, e->io.mode,   119, 217);
	surfaceDraw(e->io.screen, e->io.place,   119, 220);
	surfaceDraw(e->io.screen, e->io.discard, 119, 220);
	surfaceDraw(e->io.screen, e->io.discard, 119, 220);
	surfaceDraw(e->io.screen, e->io.play, 133, 220);
	guiDraw(&e->io, &e->gui); 
	SDL_Flip(e->io.screen);
}


void exec(struct env *e)
{
	int st, q = 0;
	e->game.turn = 0; 
	NOT(e);

	do {
		st = SDL_GetTicks();
		q = handleEvent(&e->controls);
		update(e);
		draw(e);
		delay(st, SDL_GetTicks(), 60);
	} while (!q);
}


void adjustTest(struct game *g)
{
	int i;
	struct adjust a;

	a.type = ADJUST_RACK;
	for (i = 0; i < RACK_SIZE; i++) {
		a.data.rack[i] = i;
	}

	switch (fdAdjustErr(&a, g)) {
	case ADJUST_ERR_RACK_OUT_OF_RANGE: puts("[adjust err: out of range]"); break;
	case ADJUST_ERR_RACK_INVALID_TILE: puts("[adjust err: selected tile is invalid]"); break;
	case ADJUST_ERR_RACK_DUPLICATE_INDEX: puts("[adjust err: duplicate tile swap]"); break;
	case ADJUST_ERR_NONE: puts("[adjust err: none]"); break;
	default: break;
	}
}


int gui()
{
	int exit_status = EXIT_FAILURE;
	struct env e;
	if (init(&e)) {
		adjustTest(&e.game);
		exec(&e);
		exit_status = EXIT_SUCCESS;
	}
	quit(&e);
	return exit_status;
}


