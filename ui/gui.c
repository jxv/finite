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


void updateBoardWidget(struct cmd *cmd, struct gameGui *gg, struct controls *c)
{
	struct gridWidget *bw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == GUI_FOCUS_BOARD);

	bw = &gg->boardWidget;
	cmd->type = CMD_INVALID;

	if (c->x.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_PREV;
		return;
	}
	if (c->y.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_NEXT;
		return;
	}
	if (c->a.type == KEYSTATE_PRESSED && bw->button[bw->index.y][bw->index.x]) {
		cmd->type = CMD_BOARD;
		cmd->data.board = bw->index;
		return;
	}
	if (c->up.type == KEYSTATE_PRESSED) {
		bw->index.y += BOARD_Y;
		bw->index.y--;
		bw->index.y %= BOARD_Y;
		return;
	}
	if (c->down.type == KEYSTATE_PRESSED) {
		bw->index.y++;
		bw->index.y %= BOARD_Y;
		return;
	}
	if (c->left.type == KEYSTATE_PRESSED) {
		bw->index.x += BOARD_X;
		bw->index.x--;
		bw->index.x %= BOARD_X;
		return;
	}
	if (c->right.type == KEYSTATE_PRESSED) {
		bw->index.x++;
		bw->index.x %= BOARD_X;
	}
}


void updateChoiceWidget(struct cmd *cmd, struct gameGui *gg, struct controls *c)
{
	struct gridWidget *cw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == GUI_FOCUS_CHOICE);

	cw = &gg->boardWidget;
	cw->index.y = 0;
	cmd->type = CMD_INVALID;

	if (c->x.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_PREV;
		return;
	}
	if (c->y.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_NEXT;
		return;
	}
	if (cw->button[cw->index.y][cw->index.x]) {
		if (c->a.type == KEYSTATE_PRESSED) {
			switch (cw->index.x) {
			case CHOICE_RECALL: cmd->type = CMD_RECALL; break;
			case CHOICE_PLAY: cmd->type = CMD_PLAY; break;
			case CHOICE_MODE: /* fall through */
			default: break;
			}
			return;
		}
		if (c->up.type == KEYSTATE_PRESSED) {
			switch (cw->index.x) {
			case CHOICE_MODE: cmd->type = CMD_MODE_UP; break;
			case CHOICE_RECALL: /* fall through */
			case CHOICE_PLAY:
			default: break;
			}
			return;
		}
		if (c->down.type == KEYSTATE_PRESSED) {
			switch (cw->index.x) {
			case CHOICE_MODE: cmd->type = CMD_MODE_DOWN; break;
			case CHOICE_RECALL: /* fall through */
			case CHOICE_PLAY:
			default: break;
			}
			return;
		}
	}
	if (c->left.type == KEYSTATE_PRESSED) {
		cw->index.x += CHOICE_COUNT; 
		cw->index.x--;
		cw->index.x %= CHOICE_COUNT;
		return;
	}
	if (c->right.type == KEYSTATE_PRESSED) {
		cw->index.x++;
		cw->index.x %= CHOICE_COUNT;
	}
}


void updateRackWidget(struct cmd *cmd, struct gameGui *gg, struct controls *c)
{
	struct gridWidget *rw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == GUI_FOCUS_RACK);
	
	rw = &gg->boardWidget;
	rw->index.y = 0;
	cmd->type = CMD_INVALID;

	if (c->x.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_PREV;
		return;
	}
	if (c->y.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_NEXT;
		return;
	}
	if (c->a.type == KEYSTATE_PRESSED && rw->button[rw->index.y][rw->index.x]) {
		cmd->type = CMD_BOARD;
		cmd->data.rack = rw->index.x;
		return;
	}
	if (c->left.type == KEYSTATE_PRESSED) {
		rw->index.x += RACK_SIZE; 
		rw->index.x--;
		rw->index.x %= RACK_SIZE;
		return;
	}
	if (c->right.type == KEYSTATE_PRESSED) {
		rw->index.x++;
		rw->index.x %= RACK_SIZE;
	}
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
	case CMD_INVALID: /* fall through */
	default: break;
	}
}


void updateTransMovePlaceInit(struct transMove *tm, struct cmd *c)
{
	NOT(tm);
	NOT(c);
	assert(tm->type == TRANS_MOVE_PLACE_INIT);
	assert(c->type != CMD_BOARD);
	assert(c->type != CMD_RECALL);
	assert(c->type != CMD_PLAY);

	switch (c->type) {
	case CMD_RACK: {
	       tm->type = TRANS_MOVE_PLACE_HOLD;
	       tm->data.rack = c->data.rack;
	       break;
	}
	case CMD_MODE_UP: tm->type = TRANS_MOVE_SKIP_INIT; break;
	case CMD_MODE_DOWN: tm->type = TRANS_MOVE_DISCARD_INIT; break;
	case CMD_QUIT: tm->type = TRANS_MOVE_QUIT; break;
	case CMD_INVALID: /* fall through */
	case CMD_FOCUS_PREV:
	case CMD_FOCUS_NEXT:
	default: break;
	}
}


void updateTransMove(struct transMove *tm, struct cmd *c)
{
	NOT(tm);
	NOT(c);

	switch (tm->type) {
	case TRANS_MOVE_PLACE_INIT:  break;
	case TRANS_MOVE_PLACE:  break;
	case TRANS_MOVE_PLACE_HOLD:  break;
	case TRANS_MOVE_DISCARD_INIT: break;
	case TRANS_MOVE_DISCARD: break;
	case TRANS_MOVE_DISCARD_HOLD: break;
	case TRANS_MOVE_SKIP: break;
	case TRANS_MOVE_QUIT: break;
	case TRANS_MOVE_NONE: /* fall through */
	case TRANS_MOVE_INVALID:
	default: break;
	}
}


void update(struct env *e)
{
	/*
	Follows a MVC pattern:
	Game           -> Model
	GUI + IO       -> View
	GUI + Controls -> Controller
	*/
	struct cmd c;

	NOT(e);

	switch (e->gui.gameGui.focus) {
	case GUI_FOCUS_BOARD: updateBoardWidget(&c, &e->gui.gameGui, &e->controls); break;
	case GUI_FOCUS_CHOICE: updateChoiceWidget(&c, &e->gui.gameGui, &e->controls); break;
	case GUI_FOCUS_RACK: updateRackWidget(&c, &e->gui.gameGui, &e->controls); break;
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

	updateTransMove(&e->transMove, &c);
}


void guiDraw(struct io *io, struct gui *g, struct game *gm)
{
	struct coor pos;
	struct coor dim;

	NOT(io), NOT(g);

	dim.x = 14;
	dim.y = 14;

	pos.x = 106;
	pos.y = 5;
	gridWidgetDraw(io->screen, &g->gameGui.boardWidget, pos, dim);
	boardWidgetDraw(io, &g->gameGui.boardWidget, &gm->board, pos, dim);
	
	pos.x = 162;
	pos.y = 222;
	gridWidgetDraw(io->screen, &g->gameGui.rackWidget, pos, dim);
	
	pos.x = 106;
	pos.y = 222;
	gridWidgetDraw(io->screen, &g->gameGui.choiceWidget, pos, dim);
}


void draw(struct env *e)
{
	NOT(e);

	SDL_FillRect(e->io.screen, NULL, 0);
	surfaceDraw(e->io.screen, e->io.back, 0, 0); surfaceDraw(e->io.screen, e->io.recall, 105, 220);
	surfaceDraw(e->io.screen, e->io.mode,   119, 217);
	surfaceDraw(e->io.screen, e->io.place,   119, 220);
	surfaceDraw(e->io.screen, e->io.discard, 119, 220);
	surfaceDraw(e->io.screen, e->io.discard, 119, 220);
	surfaceDraw(e->io.screen, e->io.play, 133, 220);
	guiDraw(&e->io, &e->gui, &e->game); 
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
		exec(&e);
		exit_status = EXIT_SUCCESS;
	}
	quit(&e);
	return exit_status;
}


