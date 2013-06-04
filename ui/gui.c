#include "gui.h"
#include "init.h"


#define TILE_SPACING_X	2
#define TILE_SPACING_Y	0


void initBoardWidget(struct boardWidget *bw)
{
	int y, x;

	NOT(bw);
	
	bw->focus.x = 0;
	bw->focus.y = 0;

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {	
			bw->locWidget[y][x].tile.type = TILE_NONE;
		}
	}
}


void guiInit(struct gui *g)
{
	NOT(g);
	
	initBoardWidget(&g->gameWidget.boardWidget);
}

void rackDraw(struct io *io, struct player *p)
{
	int xOffset, yOffset, w, i, letter, type;
	SDL_Surface *t;

	NOT(io), NOT(p), NOT(io->wild);

	xOffset = 162;
	yOffset = 222;
	w = io->wild->w + TILE_SPACING_X;
	for (i = 0; i < RACK_SIZE; i++) {
		type = p->tile[i].type;
		t = io->wild;
		if (type == TILE_NONE) {
			continue;
		}
		if (type == TILE_LETTER) {
			letter = p->tile[i].letter;
			assert(letter > LETTER_INVALID && letter < LETTER_COUNT);
			t = io->tile[TILE_LETTER][letter];
		}
		surfaceDraw(io->screen, t, xOffset + i * w, yOffset);
	}
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
	e->io.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
			SDL_SWSURFACE);

	if (e->io.screen == NULL) {
		return false;
	}

	if (!dictInit(&e->game.dict, RES_PATH "dict.txt")) {
		return false;
	}
	
	e->io.back = surfaceLoad(RES_PATH "back.png");
	if (!e->io.back) {
		return false;
	}

	e->io.lockon = surfaceLoad(RES_PATH "lockon.png");
	if (!e->io.lockon) {
		return false;
	}

	if (!fontmapInit(&e->io.white_font, 6, 12, RES_PATH "white_font.png")) {
		return false;
	}

	if (!fontmapInit(&e->io.black_font, 6, 12, RES_PATH "black_font.png")) {
		return false;
	}
	
	tile = surfaceLoad(RES_PATH "tile.png");
	if (!tile) {
		return false;
	}
	e->io.wild = surfaceCpy(tile);
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

/*
bool solid_move(struct keystate *ks)
{
	NOT(ks);

	return ks->type == KEYSTATE_PRESSED || (ks->type == KEYSTATE_HELD && ks->time >= 0.3f);
}
*/

/*
void selection_rack(struct selection *s, struct controls *c)
{
	NOT(s), NOT(c);
	assert(s->type == SELECTioN_RACK);

	if (solid_move(&c->up)) {
		int x = s->data.rack + 7;
		s->type = SELECTioN_BOARD;
		s->data.board.y = BOARD_Y - 1;
		s->data.board.x = x;
		return;
	}
	if (solid_move(&c->left)) {
		s->data.rack --;
	}
	if (solid_move(&c->right)) {
		s->data.rack ++;
	}
	if (s->data.rack < 0) {
		s->type = SELECTioN_CHOICE;
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
	assert(s->type == SELECTioN_CHOICE);

	if (solid_move(&c->up)) {
		int x = s->data.choice + 1;
		s->type = SELECTioN_BOARD;
		s->data.board.y = BOARD_Y - 1;
		s->data.board.x = x;
		return;
	}
	if (solid_move(&c->left)) {
		s->data.choice --;
	}
	if (solid_move(&c->right)) {
		s->data.choice ++;
	}
	if (s->data.choice < 0) {
		s->data.choice = 0;
		return;
	}
	if (s->data.choice >= CHOICE_COUNT) {
		s->type = SELECTioN_RACK;
		s->data.rack = 0;
		return;
	}
}

*/


void cmdQueuePush(struct cmdQueue *cq, struct gui *g)
{
	NOT(cq);
}


void gameWidgetSyncGame(struct gameWidget *gw, struct game *g)
{
	int x, y;
	struct boardWidget *bw;

	NOT(gw), NOT(g);
	
	bw = &gw->boardWidget;

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			bw->locWidget[y][x].sq = g->board.sq[y][x];
			if (g->board.tile[y][x].type == TILE_NONE) {
				bw->locWidget[y][x].enabled = true;
				bw->locWidget[y][x].tile.type = TILE_NONE;
			} else {
				bw->locWidget[y][x].enabled = false;
				bw->locWidget[y][x].tile = g->board.tile[y][x];
			}
		}
	}
}

void boardWidgetSyncControls(struct gameWidget *gw, struct controls *c)
{	
	struct boardWidget *bw;
	NOT(gw), NOT(c);
	assert(gw->focus == FOCUS_BOARD);

	bw = &gw->boardWidget;
	
	if (c->up.type == KEYSTATE_PRESSED) {
		bw->focus.y--;
	}
	if (c->down.type == KEYSTATE_PRESSED) {
		bw->focus.y++;
	}
	if (c->left.type == KEYSTATE_PRESSED) {
		bw->focus.x--;
		bw->focus.x += BOARD_X;
		bw->focus.x %= BOARD_X;
	}
	if (c->right.type == KEYSTATE_PRESSED) {
		bw->focus.x++;
		bw->focus.x %= BOARD_X;
	}
	if (bw->focus.y < 0) {
		bw->focus.y = 0;
	}
	if (bw->focus.y >= BOARD_Y) {
		if (bw->focus.x <=  5) {
			int choice = bw->focus.x - (0+1);
			gw->focus = FOCUS_CHOICE;
			if (choice < 0) {
				choice = 0;
			}
			if (choice >= CHOICE_COUNT) {
				choice = CHOICE_COUNT - 1;
			}
			gw->choiceWidget.focus = choice;
		}
		if (bw->focus.x >= 6) {
			int rack = bw->focus.x - (6+1);
			gw->focus = FOCUS_RACK;
			if (rack < 0) {
				rack = 0;
			}
			if (rack >= RACK_SIZE) {
				rack = RACK_SIZE - 1;
			}
			gw->rackWidget.focus = rack;
		}
		bw->focus.y = BOARD_Y - 1;
	}
}

void gameWidgetSyncControls(struct gameWidget *gw, struct controls *c)
{
	gw->focus = FOCUS_BOARD;
	boardWidgetSyncControls(gw, c);
	gw->focus = FOCUS_BOARD;
}


void gameSyncGameWidget(struct game *g, struct gameWidget *gw)
{
}


void guiSyncGame(struct gui *gu, struct game *ga)
{
	NOT(gu), NOT(ga);
	
	gameWidgetSyncGame(&gu->gameWidget, ga);
}


void guiSyncControls(struct gui *g, struct controls *c)
{
	NOT(g), NOT(c);
	
	gameWidgetSyncControls(&g->gameWidget, c);
}


void gameSyncGui(struct game *ga, struct gui *gu)
{
	NOT(ga), NOT(gu);
	
	gameSyncGameWidget(ga, &gu->gameWidget);
}


void update(struct env *e)
{
	/*/
	 * Follows a MVC pattern:
	 * Game           -> Model
	 * GUI + IO       -> View
	 * GUI + Controls -> Controller
	/*/
	NOT(e);

	guiSyncGame(&e->gui, &e->game);
	guiSyncControls(&e->gui, &e->controls);
	gameSyncGui(&e->game, &e->gui);
}


/*

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
	surfaceDraw(io->screen, io->lockon, x + -2, y + -2);
}

*/


void boardWidgetDraw(struct io *io, struct boardWidget *bw)
{
	int xOffset, yOffset, x, y, w, h, letter, type;
	
	NOT(io), NOT(bw);
	
	xOffset = 106;
	yOffset = 6;
	w = io->wild->w + TILE_SPACING_X;
	h = io->wild->h + TILE_SPACING_Y;
	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			type = 1;
			switch (bw->locWidget[y][x].tile.type) {
			case TILE_WILD: 
				type = 0;
			case TILE_LETTER:
				letter = (int)bw->locWidget[y][x].tile.letter;
				surfaceDraw(io->screen,
					     io->tile[type][letter],
					     xOffset + x * w,
					     yOffset + y * h);
				break;
			default: break;
			}
		}
	}
}


void gameWidgetDraw(struct io *io, struct gameWidget *gw)
{
	NOT(io), NOT(gw);

	boardWidgetDraw(io, &gw->boardWidget);
}


void guiDraw(struct io *io, struct gui *g)
{
	NOT(io), NOT(g);

	gameWidgetDraw(io, &g->gameWidget);
}


void draw(struct env *e)
{
	NOT(e);

	SDL_FillRect(e->io.screen, NULL, 0);
	surfaceDraw(e->io.screen, e->io.back, 0, 0);
	guiDraw(&e->io, &e->gui);
	rackDraw(&e->io, e->game.player + e->game.turn);
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


int gui()
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


