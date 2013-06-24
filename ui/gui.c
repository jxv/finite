#include "gui.h"
#include "init.h"
#include "widget.h"


void guiInit(struct gui *g)
{
	NOT(g);
	
	mkRackWidget(&g->gameGui.rackWidget);
	mkChoiceWidget(&g->gameGui.choiceWidget);
	mkBoardWidget(&g->gameGui.boardWidget);
	g->gameGui.focus = GUI_FOCUS_BOARD;
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
	if ((e->io.recallDisable = surfaceLoad(RES_PATH "recall_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.mode = surfaceLoad(RES_PATH "mode.png")) == NULL) {
		return false;
	}
	if ((e->io.modeDisable = surfaceLoad(RES_PATH "mode_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.place = surfaceLoad(RES_PATH "place.png")) == NULL) {
		return false;
	}
	if ((e->io.placeDisable = surfaceLoad(RES_PATH "place_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.discard = surfaceLoad(RES_PATH "discard.png")) == NULL) {
		return false;
	}
	if ((e->io.discardDisable = surfaceLoad(RES_PATH "discard_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.skip = surfaceLoad(RES_PATH "skip.png")) == NULL) {
		return false;
	}
	if ((e->io.skipDisable = surfaceLoad(RES_PATH "skip_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.play = surfaceLoad(RES_PATH "play.png")) == NULL) {
		return false;
	}
	if ((e->io.playDisable = surfaceLoad(RES_PATH "play_disable.png")) == NULL) {
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
	e->transMove.type = TRANS_MOVE_INVALID;
	e->gui.gameGui.focus = GUI_FOCUS_CHOICE;
	e->gui.gameGui.choiceWidget.index.x = 1;
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
	keystateUpdate(&c->up, ks[SDLK_UP]);
	keystateUpdate(&c->down, ks[SDLK_DOWN]);
	keystateUpdate(&c->left, ks[SDLK_LEFT]);
	keystateUpdate(&c->right, ks[SDLK_RIGHT]);
	keystateUpdate(&c->a, ks[SDLK_a]);
	keystateUpdate(&c->b, ks[SDLK_b]);
	keystateUpdate(&c->x, ks[SDLK_x]);
	keystateUpdate(&c->y, ks[SDLK_y]);
	return false;
}


void printCmd(struct cmd *c)
{
	NOT(c);

	switch (c->type) {
	case CMD_FOCUS_PREV: puts("[cmd:focus-prev]"); break;
	case CMD_FOCUS_NEXT: puts("[cmd:focus-next]"); break;
	case CMD_BOARD: printf("[cmd:board (%d,%d)]\n", c->data.board.x, c->data.board.y); break;
	case CMD_RACK: printf("[cmd:rack %d]\n", c->data.rack); break;
	case CMD_RECALL: puts("[cmd:recall]"); break;
	case CMD_MODE_UP: puts("[cmd:mode-up]"); break;
	case CMD_MODE_DOWN: puts("[cmd:mode-down]"); break;
	case CMD_PLAY: puts("[cmd:play]"); break;
	case CMD_QUIT: puts("[cmd:quit]"); break;
	/*case CMD_INVALID:	puts("[cmd:invalid]"); break; // very noisy */
	default: break;
	}
}


void printTransMove(struct transMove *tm)
{
	NOT(tm);

	switch (tm->type) {
	case TRANS_MOVE_NONE: puts("[trans-move:none]"); break;
	case TRANS_MOVE_PLACE_INIT: puts("[trans-move:place-init]"); break;
	case TRANS_MOVE_PLACE: puts("[trans-move:place]"); break;
	case TRANS_MOVE_PLACE_HOLD: puts("[trans-move:place-hold]"); break;
	case TRANS_MOVE_DISCARD_INIT: puts("[trans-move:discard-init]"); break;
	case TRANS_MOVE_DISCARD: puts("[trans-move:discard]"); break;
	case TRANS_MOVE_SKIP: puts("[trans-move:skip]"); break;
	case TRANS_MOVE_QUIT: puts("[trans-move:quit]"); break;
	case TRANS_MOVE_INVALID: puts("[trans-move:invalid]"); break;
	default: break;
	}
}


bool updateTransMovePlaceInit(struct transMove *tm, struct cmd *c)
{
	NOT(tm);
	NOT(c);
	assert(tm->type == TRANS_MOVE_PLACE_INIT);
	assert(c->type != CMD_BOARD);
	assert(c->type != CMD_RECALL);
	assert(c->type != CMD_PLAY);
	assert(c->type != CMD_QUIT);

	switch (c->type) {
	case CMD_RACK: {
		if (tm->adjust.data.tile[c->data.rack].type != TILE_NONE) {
			tm->data.place.idx = c->data.rack;
			tm->type = TRANS_MOVE_PLACE_HOLD;
			return true;
		}
		break;
	}
	case CMD_MODE_UP: {
		tm->type = TRANS_MOVE_SKIP;
		return true;
	}
	case CMD_MODE_DOWN: {
		tm->type = TRANS_MOVE_DISCARD_INIT;
		return true;
	}
	case CMD_INVALID: /* fall through */
	default: break;
	}
	return false;
}


bool updateTransMovePlace(struct transMove *tm, struct cmd *c)
{
	NOT(tm);
	NOT(c);
	assert(tm->type == TRANS_MOVE_PLACE);
	assert(c->type != CMD_MODE_UP);
	assert(c->type != CMD_MODE_DOWN);
	assert(c->type != CMD_QUIT);
	
	switch (c->type) {
	case CMD_BOARD:	tm->type = TRANS_MOVE_PLACE_HOLD; break;
	case CMD_RACK: {
		if (tm->adjust.data.tile[c->data.rack].type != TILE_NONE) {
			tm->data.place.idx = c->data.rack;
			tm->type = TRANS_MOVE_PLACE_HOLD;
			return true;
		}
		break;
	}
	case CMD_RECALL: {
		tm->type = TRANS_MOVE_PLACE_INIT;
		return true;
	}
	default: break;
	}
	return false;
}


bool updateTransMovePlaceHold(struct transMove *tm, struct cmd *c)
{
	TileType t;

	NOT(tm);
	NOT(c);
	assert(tm->type == TRANS_MOVE_PLACE_HOLD);
	assert(c->type != CMD_MODE_UP);
	assert(c->type != CMD_MODE_DOWN);
	assert(c->type != CMD_PLAY);
	assert(c->type != CMD_QUIT);
	
	switch (c->type) {
	case CMD_BOARD:	{
		tm->type = TRANS_MOVE_PLACE;
		break;
	}
	case CMD_RACK: {
		assert(c->data.rack >= 0);
		assert(c->data.rack < RACK_SIZE);
		t = tm->adjust.data.tile[c->data.rack].type;
		if (t == TILE_NONE) {
			assert(tm->data.place.num >= 0);
			assert(tm->data.place.num < RACK_SIZE);
			if (tm->data.place.num == 0) {
				tm->data.place.num = TRANS_MOVE_PLACE_INIT;
			} else {
				tm->data.place.num = TRANS_MOVE_PLACE;
			}
		} else {
			assert(t == TILE_LETTER || t == TILE_WILD);
			adjustSwap(&tm->adjust, tm->data.place.idx, c->data.rack);
			tm->data.place.idx = c->data.rack;
		}
		return true;
	}
	case CMD_RECALL: {
		tm->type = TRANS_MOVE_PLACE_INIT;
		return true;
	}
	default: break;
	}
	return false;
}


bool updateTransMoveDiscardInit(struct transMove *tm, struct cmd *c)
{
	NOT(tm);
	NOT(c);
	assert(tm->type == TRANS_MOVE_DISCARD_INIT);
	assert(c->type != CMD_BOARD);
	assert(c->type != CMD_RECALL);
	assert(c->type != CMD_PLAY);
	assert(c->type != CMD_QUIT);
	
	switch (c->type) {
	case CMD_RACK: tm->type = TRANS_MOVE_DISCARD; break;
	case CMD_MODE_UP: tm->type = TRANS_MOVE_PLACE_INIT; break;
	case CMD_MODE_DOWN: tm->type = TRANS_MOVE_SKIP; break;
	default: break;
	}
	return false;
}


bool updateTransMoveDiscard(struct transMove *tm, struct cmd *c)
{
	NOT(tm);
	NOT(c);
	assert(tm->type == TRANS_MOVE_DISCARD);
	assert(c->type != CMD_BOARD);
	assert(c->type != CMD_MODE_UP);
	assert(c->type != CMD_MODE_DOWN);
	assert(c->type != CMD_QUIT);
	
	switch (c->type) {
	case CMD_RACK: {
		tm->type = TRANS_MOVE_DISCARD;
		break;
	}
	case CMD_RECALL: {
		tm->type = TRANS_MOVE_DISCARD_INIT;
		return true;
	}
	default: break;
	}
	return false;
}


bool updateTransMoveSkip(struct transMove *tm, struct cmd *c)
{
	NOT(tm);
	NOT(c);
	assert(tm->type == TRANS_MOVE_SKIP);
	assert(c->type != CMD_BOARD);
	assert(c->type != CMD_RACK);
	assert(c->type != CMD_RECALL);
	
	switch (c->type) {
	case CMD_MODE_UP: {
		tm->type = TRANS_MOVE_DISCARD_INIT;
		return true;
	}
	case CMD_MODE_DOWN: {
		tm->type = TRANS_MOVE_PLACE_INIT;
		return true;
	}
	default: break;
	}
	return false;
}


bool updateTransMove(struct transMove *tm, struct cmd *c)
{
	NOT(tm);
	NOT(c);

	switch (tm->type) {
	case TRANS_MOVE_PLACE_INIT: return updateTransMovePlaceInit(tm, c);
	case TRANS_MOVE_PLACE: return updateTransMovePlace(tm, c);
	case TRANS_MOVE_PLACE_HOLD: return updateTransMovePlaceHold(tm, c);
	case TRANS_MOVE_DISCARD_INIT: return updateTransMoveDiscardInit(tm, c);
	case TRANS_MOVE_DISCARD: return updateTransMoveDiscard(tm, c);
	case TRANS_MOVE_SKIP: return updateTransMoveSkip(tm, c);
	case TRANS_MOVE_QUIT: /* fall through */
	case TRANS_MOVE_NONE:
	case TRANS_MOVE_INVALID:
	default: break;
	}
	return false;
}


void update(struct env *e)
{
	struct cmd c;

	NOT(e);

	if (e->transMove.type == TRANS_MOVE_INVALID) {
		e->transMove.playerIdx = 0;
		e->transMove.type = TRANS_MOVE_PLACE_INIT;
		mkAdjust(&e->transMove.adjust, &e->game.player[0]);
		c.type = CMD_INVALID;
		updateTransMove(&e->transMove, &c);
		updateBoardWidget(&e->gui.gameGui.boardWidget, &e->transMove); 
		updateChoiceWidget(&e->gui.gameGui.choiceWidget, &e->transMove);
		updateRackWidget(&e->gui.gameGui.rackWidget, &e->transMove);
	}

	switch (e->gui.gameGui.focus) {
	case GUI_FOCUS_BOARD: boardWidgetControls(&c, &e->gui.gameGui, &e->controls); break;
	case GUI_FOCUS_CHOICE: choiceWidgetControls(&c, &e->gui.gameGui, &e->controls); break;
	case GUI_FOCUS_RACK: rackWidgetControls(&c, &e->gui.gameGui, &e->controls); break;
	default: break;
	}

	switch (c.type) {
	case CMD_FOCUS_PREV: {
		e->gui.gameGui.focus += GUI_FOCUS_COUNT;
		e->gui.gameGui.focus--;
		e->gui.gameGui.focus %= GUI_FOCUS_COUNT;
		break;
	}
	case CMD_FOCUS_NEXT: {
		e->gui.gameGui.focus++;
		e->gui.gameGui.focus %= GUI_FOCUS_COUNT;
		break;
	}
	default: break;
	}

	printCmd(&c);

	if (updateTransMove(&e->transMove, &c)) {
		printTransMove(&e->transMove);
		updateBoardWidget(&e->gui.gameGui.boardWidget, &e->transMove); 
		updateChoiceWidget(&e->gui.gameGui.choiceWidget, &e->transMove);
		updateRackWidget(&e->gui.gameGui.rackWidget, &e->transMove);
	}
}


void guiDrawLockon(struct io *io, struct gameGui *gg)
{
	const int w = 14, h = 14;
	struct coor idx;

	NOT(io);
	NOT(gg);
	
	switch (gg->focus) {
	case GUI_FOCUS_BOARD: {
		idx = gg->boardWidget.index;
		surfaceDraw(io->screen, io->lockon, 104 + idx.x * w, 4 + idx.y * h);
		break;
	}
	case GUI_FOCUS_RACK: {
		idx = gg->rackWidget.index;
		surfaceDraw(io->screen, io->lockon, 162 + idx.x * w, 220);
		break;
	}
	case GUI_FOCUS_CHOICE: {
		idx = gg->choiceWidget.index;
		surfaceDraw(io->screen, io->lockon, 105 + idx.x * w, 220);
		break;
	}
	default: break;
	}
}


void guiDraw(struct io *io, struct gui *g, struct game *gm, struct transMove *tm)
{
	struct coor pos;
	struct coor dim;

	NOT(io);
	NOT(g);
	NOT(tm);

	dim.x = 14;
	dim.y = 14;

	pos.x = 106;
	pos.y = 5;
	gridWidgetDraw(io->screen, &g->gameGui.boardWidget, pos, dim);
	boardWidgetDraw(io, &g->gameGui.boardWidget, &gm->board, pos, dim);
	
	pos.x = 162;
	pos.y = 222;
	gridWidgetDraw(io->screen, &g->gameGui.rackWidget, pos, dim);
	rackWidgetDraw(io, tm, &g->gameGui.rackWidget, pos, dim, &gm->player[tm->playerIdx]);
	
	pos.x = 106;
	pos.y = 222;
	gridWidgetDraw(io->screen, &g->gameGui.choiceWidget, pos, dim);
	choiceWidgetDraw(io, tm, &g->gameGui.choiceWidget, pos, dim);
	
	guiDrawLockon(io, &g->gameGui);
}


void draw(struct env *e)
{
	NOT(e);

	SDL_FillRect(e->io.screen, NULL, 0);
	surfaceDraw(e->io.screen, e->io.back, 0, 0);
	guiDraw(&e->io, &e->gui, &e->game, &e->transMove); 
	SDL_Flip(e->io.screen);
}


void exec(struct env *e)
{
	int st;
	bool q;

	NOT(e);

	e->game.turn = 0; 
	q = false;

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
	struct adjust a;

	mkAdjust(&a, &g->player[0]);

	switch (fdAdjustErr(&a, &g->player[0])) {
	case ADJUST_ERR_RACK_OUT_OF_RANGE: puts("[adjust err: out of range]"); break;
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
		exec(&e);
		exit_status = EXIT_SUCCESS;
	}
	quit(&e);
	return exit_status;
}


