#include "gui.h"
#include "init.h"
#include "widget.h"
#include "print.h"
#include <math.h>

void initGUI(struct GUI *g)
{
	NOT(g);
	
	mkRackWidget(&g->gameGui.rackWidget);
	mkChoiceWidget(&g->gameGui.choiceWidget);
	mkBoardWidget(&g->gameGui.boardWidget);
	g->gameGui.focus = GUI_FOCUS_BOARD;
}

bool fontmapInit(struct Font *f, int w, int h, const char *filename)
{
	NOT(f);
	NOT(filename);

	f->width = w;
	f->height = h;
	f->map = surfaceLoad(filename);
	return f->map != NULL;
}

void fontmapQuit(struct Font *f)
{
	NOT(f);
	NOT(f->map);

	surfaceFree(f->map);
}

void strDraw(SDL_Surface *s, struct Font *f, const char *str, int x, int y)
{
	int i;
	char c;
	SDL_Rect offset, clip;

	NOT(s);
	NOT(f);
	NOT(f->map);
	NOT(str);

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

bool ioInit(struct IO *io)
{
	NOT(io);

	return false;
}

void keystateInit(struct KeyState *ks)
{
	NOT(ks);
	
	ks->type = KEY_STATE_UNTOUCHED;
	ks->time = 0.0f;
}

void controlsInit(struct Controls *c)
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
	keystateInit(&c->l);
	keystateInit(&c->r);
}

bool init(struct Env *e)
{
	int i, j;
	char str[32];
	SDL_Surface *tile[TILE_LOOK_COUNT];

	e->io.time = 0;

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

	e->io.sq[SQ_NORMAL] = surfaceLoad(RES_PATH "sq_normal.png");
	e->io.sq[SQ_DBL_LET] = surfaceLoad(RES_PATH "sq_dl.png");
	e->io.sq[SQ_DBL_WRD] = surfaceLoad(RES_PATH "sq_dw.png");
	e->io.sq[SQ_TRP_LET] = surfaceLoad(RES_PATH "sq_tl.png");
	e->io.sq[SQ_TRP_WRD] = surfaceLoad(RES_PATH "sq_tw.png");
	e->io.sq[SQ_FREE] = surfaceLoad(RES_PATH "sq_free.png");

	for (i = 0; i < SQ_COUNT; i++) {
		if (e->io.sq[i] == NULL) { 
			return false;
		}
	}

	tile[TILE_LOOK_DISABLE] = surfaceLoad(RES_PATH "tile_disable.png");
	tile[TILE_LOOK_NORMAL] = surfaceLoad(RES_PATH "tile_normal.png");
	tile[TILE_LOOK_HOLD] = surfaceLoad(RES_PATH "tile_hold.png");
	tile[TILE_LOOK_GHOST] = surfaceLoad(RES_PATH "tile_ghost.png");

	if (!(tile[TILE_LOOK_DISABLE] && tile[TILE_LOOK_NORMAL] && tile[TILE_LOOK_HOLD] && tile[TILE_LOOK_GHOST])) {
		surfaceFree(tile[TILE_LOOK_DISABLE]);
		surfaceFree(tile[TILE_LOOK_NORMAL]);
		surfaceFree(tile[TILE_LOOK_HOLD]);
		surfaceFree(tile[TILE_LOOK_GHOST]);
		return false;
	} 
	e->io.wild[TILE_LOOK_DISABLE] = surfaceCpy(tile[TILE_LOOK_DISABLE]);
	e->io.wild[TILE_LOOK_NORMAL] = surfaceCpy(tile[TILE_LOOK_NORMAL]);
	e->io.wild[TILE_LOOK_HOLD] = surfaceCpy(tile[TILE_LOOK_HOLD]);
	e->io.wild[TILE_LOOK_GHOST] = surfaceCpy(tile[TILE_LOOK_GHOST]);

	for (i = 0; i < LETTER_COUNT; i++) {
		for (j = 0; j < TILE_LOOK_COUNT; j++) {
			e->io.tile[TILE_WILD][i][j] = surfaceCpy(tile[j]);
			if (!e->io.tile[TILE_WILD][i][j]) {
				return false;
			}
			e->io.tile[TILE_LETTER][i][j] = surfaceCpy(tile[j]);
			if (!e->io.tile[TILE_LETTER][i][j]) {
				return false;
			}
			sprintf(str,"%c", i + 'a');
			strDraw(e->io.tile[TILE_WILD][i][j], &e->io.black_font, str, 3, 0);
			sprintf(str,"%c", i + 'A');
			strDraw(e->io.tile[TILE_LETTER][i][j], &e->io.black_font, str, 3, 0);
		}
	}
	for (i = 0; i < TILE_LOOK_COUNT; i++) {
		surfaceFree(tile[i]);
	}
	boardInit(&e->game.board);
	bagInit(&e->game.bag);
	initGUI(&e->gui);
	e->game.playerNum = 2;
	playerInit(&e->game.player[0], &e->game.bag);
	playerInit(&e->game.player[1], &e->game.bag);
	e->game.player[0].active = 1;
	e->game.player[1].active = 1;
	controlsInit(&e->controls);
	e->transMove.type = TRANS_MOVE_INVALID;
	e->gui.gameGui.focus = GUI_FOCUS_CHOICE;
	e->gui.gameGui.bottomLast = GUI_FOCUS_CHOICE;
	e->gui.gameGui.choiceWidget.index.x = 1;
	return true;
}

void quit(struct Env *e)
{
	int i, j;

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
	for (i = 0; i < SQ_COUNT; i++) {
		surfaceFree(e->io.sq[i]);
	}
	for (i = 0; i < TILE_LOOK_COUNT; i++) {
		surfaceFree(e->io.wild[i]);
	}	
	for (i = 0; i < LETTER_COUNT; i++) {
		for (j = 0; j < TILE_LOOK_COUNT; j++) {
			surfaceFree(e->io.tile[TILE_WILD][i][j]);
			surfaceFree(e->io.tile[TILE_LETTER][i][j]);
		}
	}
	fontmapQuit(&e->io.white_font);
	fontmapQuit(&e->io.black_font);
	SDL_Quit();
}

void keyStateUpdate(struct KeyState *ks, bool touched)
{
	NOT(ks);

	if (touched) {
		switch(ks->type) {
		case KEY_STATE_UNTOUCHED: {
			ks->type = KEY_STATE_PRESSED;
			ks->time = 0.0f;
			break;
		}
		case KEY_STATE_PRESSED: {
			ks->type = KEY_STATE_HELD;
			ks->time = 0.0f;
			break;
		}
		case KEY_STATE_HELD: {
			ks->time += 1.0f / 60.0f;
			break;
		}
		case KEY_STATE_RELEASED: {
			ks->type = KEY_STATE_PRESSED;
			ks->time = 0.0f;
			break;
		}
		default: break;
		}
	} else {
		switch(ks->type) {
		case KEY_STATE_UNTOUCHED: {
			ks->time += 1.0f / 60.0f;
			break;
		}
		case KEY_STATE_PRESSED: {
			ks->type = KEY_STATE_RELEASED;
			ks->time = 0.0f;
			break;
		}
		case KEY_STATE_HELD: {
			ks->type = KEY_STATE_RELEASED;
			ks->time = 0.0f;
			break;
		}
		case KEY_STATE_RELEASED: {
			ks->type = KEY_STATE_UNTOUCHED;
			ks->time = 0.0f;
			break;
		}
		default: break;
		}
	}
}

bool handleEvent(struct Controls *c)
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
	keyStateUpdate(&c->up, ks[SDLK_UP]);
	keyStateUpdate(&c->down, ks[SDLK_DOWN]);
	keyStateUpdate(&c->left, ks[SDLK_LEFT]);
	keyStateUpdate(&c->right, ks[SDLK_RIGHT]);
	keyStateUpdate(&c->a, ks[SDLK_LCTRL]);
	keyStateUpdate(&c->b, ks[SDLK_LALT]);
	keyStateUpdate(&c->x, ks[SDLK_LSHIFT]);
	keyStateUpdate(&c->y, ks[SDLK_SPACE]);
	keyStateUpdate(&c->l, ks[SDLK_TAB]);
	keyStateUpdate(&c->r, ks[SDLK_BACKSPACE]);
	return false;
}

void printCmd(struct Cmd *c)
{
	NOT(c);

	switch (c->type) {
	case CMD_FOCUS_TOP: puts("[cmd:focus-top]"); break;
	case CMD_FOCUS_BOTTOM: puts("[cmd:focus-bottom]"); break;
	case CMD_BOARD_SELECT: printf("[cmd:board-select (%d,%d)]\n", c->data.board.x, c->data.board.y); break;
	case CMD_RACK_SELECT: printf("[cmd:rack-select %d]\n", c->data.rack); break;
	case CMD_BOARD: printf("[cmd:board (%d, %d)]\n", c->data.board.x, c->data.board.y); break;
	case CMD_BOARD_LEFT: puts("[cmd:board-left"); break;
	case CMD_BOARD_RIGHT: puts("[cmd:board-right"); break;
	case CMD_BOARD_UP: puts("[cmd:board-up"); break;
	case CMD_BOARD_DOWN: puts("[cmd:board-down"); break;
	case CMD_RACK: printf("[cmd:rack %d]\n", c->data.rack); break;
	case CMD_RACK_LEFT: puts("[cmd:rack-left]"); break;
	case CMD_RACK_RIGHT: puts("[cmd:rack-right]"); break;
	case CMD_CHOICE: printf("[cmd:choice %d]\n", c->data.choice); break;
	case CMD_CHOICE_LEFT: puts("[cmd:choice-left]"); break;
	case CMD_CHOICE_RIGHT: puts("[cmd:choice-right]"); break;
	case CMD_RECALL: puts("[cmd:recall]"); break;
	case CMD_MODE_UP: puts("[cmd:mode-up]"); break;
	case CMD_MODE_DOWN: puts("[cmd:mode-down]"); break;
	case CMD_PLAY: puts("[cmd:play]"); break;
	case CMD_BOARD_CANCEL: printf("[cmd:board-cancel (%d,%d)\n]", c->data.board.x, c->data.board.y); break;
	case CMD_RACK_CANCEL: printf("[cmd:rack-cancel %d]", c->data.rack); break;
	case CMD_CHOICE_CANCEL: puts("[cmd:choice-cancel]"); break;
	case CMD_TILE_PREV: puts("[cmd:tile-prev]"); break;
	case CMD_TILE_NEXT: puts("[cmd:tile-next]"); break;
	case CMD_QUIT: puts("[cmd:quit]"); break;
	/*case CMD_INVALID: puts("[cmd:invalid]"); break; // very noisy */
	default: break;
	}
}

void printTransMove(struct TransMove *tm)
{
	NOT(tm);

	switch (tm->type) {
	case TRANS_MOVE_NONE: puts("[trans-move:none]"); break;
	case TRANS_MOVE_PLACE: puts("[trans-move:place]"); break;
	case TRANS_MOVE_PLACE_WILD: puts("[trans-move:place-wild]"); break;
	case TRANS_MOVE_PLACE_END: puts("[trans-move:place-end]"); break;
	case TRANS_MOVE_PLACE_PLAY: puts("[trans-move:place-play]"); break;
	case TRANS_MOVE_DISCARD: puts("[trans-move:discard]"); break;
	case TRANS_MOVE_DISCARD_PLAY: puts("[trans-move:discard-play]"); break;
	case TRANS_MOVE_SKIP: puts("[trans-move:skip]"); break;
	case TRANS_MOVE_SKIP_PLAY: puts("[trans-move:skip-play]"); break;
	case TRANS_MOVE_QUIT: puts("[trans-move:quit]"); break;
	case TRANS_MOVE_INVALID: puts("[trans-move:invalid]"); break;
	default: break;
	}
}

void clrMoveModePlace(struct MoveModePlace *mmp, struct Board *b) 
{
	int i;
	struct Coor idx;

	NOT(mmp);
	
	mmp->idx = 0;
	mmp->num = 0;

	for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
		for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
			mmp->taken[idx.y][idx.x] = false;
			mmp->rackIdx[idx.y][idx.x] = -1;
		}
	}

	idx.x = -1;
	idx.y = -1;
	for (i = 0; i < RACK_SIZE; i++) {
		mmp->boardIdx[i] = idx;
	}
}

void clrMoveModeDiscard(struct MoveModeDiscard *mmd)
{
	int i;

	NOT(mmd);
	
	mmd->num = 0;
	for (i = 0; i < RACK_SIZE; i++) {
		mmd->rack[i] = false;
	}
}

void findNextMoveModePlaceIdx(struct TransMove *tm)
{
	struct MoveModePlace *mmp;

	NOT(tm);
	assert(tm->type == TRANS_MOVE_PLACE);

	mmp = &tm->data.place;

	do {
		mmp->idx++;
		mmp->idx %= RACK_SIZE;
	} while (validBoardIdx(mmp->boardIdx[mmp->idx]) || tm->adjust.data.tile[mmp->idx].type == TILE_NONE);
}

bool updateTransMovePlace(struct TransMove *tm, struct Cmd *c, struct Board *b, struct Player *p)
{
	struct MoveModePlace *mmp;
	
	NOT(tm);
	NOT(c);
	NOT(b);
	NOT(p);
	assert(tm->type == TRANS_MOVE_PLACE);
	assert(c->type != CMD_QUIT);
	
	mmp = &tm->data.place;
	
	switch (c->type) {
	case CMD_BOARD_SELECT: {
		int *idx;

		idx = &mmp->rackIdx[c->data.board.y][c->data.board.x];
		if (!validRackIdx(*idx)) {
			*idx = mmp->idx;
			mmp->boardIdx[mmp->idx] = c->data.board;
			if (tm->adjust.data.tile[*idx].type == TILE_LETTER) {
				mmp->num++;
				assert(mmp->num > 0 && mmp->num <= adjustTileCount(&tm->adjust));
				if (adjustTileCount(&tm->adjust) == mmp->num) {
					tm->type = TRANS_MOVE_PLACE_END;
				} else {
					findNextMoveModePlaceIdx(tm);
				}
			} else {
				assert(tm->adjust.data.tile[*idx].type == TILE_WILD);
				tm->type = TRANS_MOVE_PLACE_WILD;
			}
		} else {
			/*
			
			int a0, a1;
			struct TileAdjust b0, b1;
			a0 = mmp->rackIdx[c->data.board.y][c->data.board.x];
			a1 = mmp->idx;
			b0 = tm->adjust.data.tile[a0];
			b1 = tm->adjust.data.tile[a1];
			tm->adjust.data.tile[a0] = b1;
			tm->adjust.data.tile[a1] = b0;
			
			*/
		}
		return true;
	}
	case CMD_RACK_SELECT: {
		TileType t;

		assert(c->data.rack >= 0);
		assert(c->data.rack < RACK_SIZE);
		t = tm->adjust.data.tile[c->data.rack].type;
		assert(t == TILE_NONE || t == TILE_LETTER || t == TILE_WILD);
		if (t != TILE_NONE && mmp->idx != c->data.rack) {
			adjustSwap(&tm->adjust, mmp->idx, c->data.rack);
			if (validBoardIdx(mmp->boardIdx[c->data.rack])) {
				int *idx;
				idx = &mmp->rackIdx[mmp->boardIdx[mmp->idx].y][mmp->boardIdx[mmp->idx].x];
				*idx = -1;
				mmp->rackIdx[mmp->boardIdx[mmp->idx].y][mmp->boardIdx[mmp->idx].x] = -1;
				mmp->boardIdx[mmp->idx] = mmp->boardIdx[c->data.rack];
				mmp->rackIdx[mmp->boardIdx[mmp->idx].y][mmp->boardIdx[mmp->idx].x] = mmp->idx;
				mmp->boardIdx[c->data.rack].x = -1;
				mmp->boardIdx[c->data.rack].y = -1;
			}
			mmp->idx = c->data.rack;
		} 
		return true;
	}
	case CMD_RECALL: {
		tm->type = TRANS_MOVE_PLACE;
		clrMoveModePlace(mmp, b);
		return true;
	}
	case CMD_BOARD_CANCEL: {
		struct Coor bIdx;
		int rIdx;
		
		bIdx = c->data.board;
		rIdx = mmp->rackIdx[bIdx.y][bIdx.x];
	
		if (validRackIdx(rIdx)) {
			mmp->rackIdx[bIdx.y][bIdx.x] = -1;
			mmp->boardIdx[rIdx].x = -1;
			mmp->boardIdx[rIdx].y = -1;
			mmp->num--;
			return true;
		}
		break;
	}
	case CMD_TILE_PREV: {
		do {
			mmp->idx += RACK_SIZE;
			mmp->idx--;
			mmp->idx %= RACK_SIZE;
		} while(validBoardIdx(mmp->boardIdx[mmp->idx]));
		return true;
	}
	case CMD_TILE_NEXT: {
		do {
			mmp->idx++;
			mmp->idx %= RACK_SIZE;
		} while(validBoardIdx(mmp->boardIdx[mmp->idx]));
		return true;
	}
	case CMD_MODE_UP: {
		tm->type = TRANS_MOVE_SKIP;
		return true;
	}
	case CMD_MODE_DOWN: {
		tm->type = TRANS_MOVE_DISCARD;
		clrMoveModeDiscard(&tm->data.discard);
		return true;
	}
	case CMD_PLAY: {
		tm->type = TRANS_MOVE_PLACE_PLAY;
		return true;
	}
	default: break;
	}
	return false;
}

bool updateTransMovePlaceWild(struct TransMove *tm, struct Cmd *c, struct Board *b, struct Player *p)
{
	struct MoveModePlace *mmp;
	NOT(tm);
	NOT(c);
	NOT(b);
	NOT(p);
	assert(tm->type == TRANS_MOVE_PLACE_WILD);
	
	mmp = &tm->data.place;
	
	switch (c->type) {
	case CMD_BOARD_SELECT: {
		mmp->num++;
		assert(mmp->num > 0 && mmp->num <= adjustTileCount(&tm->adjust));
		if (adjustTileCount(&tm->adjust) == mmp->num) {
			tm->type = TRANS_MOVE_PLACE_END;
		} else {
			tm->type = TRANS_MOVE_PLACE;
			findNextMoveModePlaceIdx(tm);
		}
		return true;
	}
	case CMD_BOARD_CANCEL: {
		tm->type = TRANS_MOVE_PLACE;
		mmp->rackIdx[mmp->boardIdx[mmp->idx].y][mmp->boardIdx[mmp->idx].x] = -1;
		mmp->boardIdx[mmp->idx].x = -1;
		mmp->boardIdx[mmp->idx].y = -1;
		return true;
	}
	case CMD_BOARD_UP: {
		int idx = tm->adjust.data.tile[mmp->idx].idx;
		p->tile[idx].letter += LETTER_COUNT;
		p->tile[idx].letter--;
		p->tile[idx].letter %= LETTER_COUNT;
		return true;
	}
	case CMD_BOARD_DOWN: {
		int idx = tm->adjust.data.tile[mmp->idx].idx;
		p->tile[idx].letter++;
		p->tile[idx].letter %= LETTER_COUNT;
		return true;
	}
	default: break;
	}

	return false;
}

bool updateTransMovePlaceEnd(struct TransMove *tm, struct Cmd *c, struct Board *b, struct Player *p)
{
	struct MoveModePlace *mmp;

	NOT(tm);
	NOT(c);
	NOT(b);
	assert(tm->type == TRANS_MOVE_PLACE_END);
	assert(c->type != CMD_QUIT);
	
	mmp = &tm->data.place;
	
	switch (c->type) {
	case CMD_BOARD_SELECT:	{
		if (validRackIdx(mmp->rackIdx[c->data.board.y][c->data.board.x])) {
			tm->type = TRANS_MOVE_PLACE;
			mmp->idx = mmp->rackIdx[c->data.board.y][c->data.board.x];
			mmp->rackIdx[c->data.board.y][c->data.board.x] = -1;
			mmp->boardIdx[mmp->idx].x = -1;
			mmp->boardIdx[mmp->idx].y = -1;
			mmp->num--;
			return true;
		}
		break;
	}
	case CMD_BOARD_CANCEL: {
		struct Coor bIdx;
		int rIdx;
		
		bIdx = c->data.board;
		rIdx = mmp->rackIdx[bIdx.y][bIdx.x];
	
		if (validRackIdx(rIdx)) {
			mmp->rackIdx[bIdx.y][bIdx.x] = -1;
			mmp->boardIdx[rIdx].x = -1;
			mmp->boardIdx[rIdx].y = -1;
			mmp->num--;
			tm->type = TRANS_MOVE_PLACE;
			return true;
		}
		break;
	}
	case CMD_MODE_UP: {
		tm->type = TRANS_MOVE_SKIP;
		return true;
	}
	case CMD_MODE_DOWN: {
		tm->type = TRANS_MOVE_DISCARD;
		clrMoveModeDiscard(&tm->data.discard);
		return true;
	}
	case CMD_RECALL: {
		tm->type = TRANS_MOVE_PLACE;
		clrMoveModePlace(mmp, b);
		return true;
	}
	default: break;
	}
	return false;
}

bool updateTransMoveDiscard(struct TransMove *tm, struct Cmd *c, struct Board *b, struct Player *p)
{
	struct MoveModeDiscard *mmd;

	NOT(tm);
	NOT(c);
	NOT(b);
	NOT(p);
	assert(tm->type == TRANS_MOVE_DISCARD);
	assert(c->type != CMD_BOARD_SELECT);
	assert(c->type != CMD_QUIT);
	
	mmd = &tm->data.discard;
	
	switch (c->type) {
	case CMD_RACK_SELECT: {
		mmd->rack[c->data.rack] = !mmd->rack[c->data.rack];
		if (mmd->rack[c->data.rack]) {
			mmd->num++;
		} else {
			mmd->num--;
		}
		if (mmd->num == 0) {
			clrMoveModeDiscard(mmd);
		}
		return true;
	}
	case CMD_RECALL: {
		clrMoveModeDiscard(mmd);
		return true;
	}
	case CMD_PLAY: {
		tm->type = TRANS_MOVE_DISCARD_PLAY;
		return true;
	}
	case CMD_MODE_UP: {
		tm->type = TRANS_MOVE_PLACE;
		clrMoveModePlace(&tm->data.place, b);
		return true;
	}
	case CMD_MODE_DOWN: {
		tm->type = TRANS_MOVE_SKIP;
		return true;
	}
	case CMD_RACK_CANCEL: {
		if (mmd->rack[c->data.rack]) {
			assert(mmd->num > 0);
			mmd->num--;
			mmd->rack[c->data.rack] = false;
		}
		return true;
	}
	default: break;
	}
	return false;
}

bool updateTransMoveSkip(struct TransMove *tm, struct Cmd *c, struct Board *b, struct Player *p)
{
	NOT(tm);
	NOT(c);
	NOT(b);
	NOT(p);
	assert(tm->type == TRANS_MOVE_SKIP);
	assert(c->type != CMD_BOARD_SELECT);
	assert(c->type != CMD_RACK_SELECT);
	assert(c->type != CMD_RECALL);
	
	switch (c->type) {
	case CMD_MODE_UP: {
		tm->type = TRANS_MOVE_DISCARD;
		clrMoveModeDiscard(&tm->data.discard);
		return true;
	}
	case CMD_MODE_DOWN: {
		tm->type = TRANS_MOVE_PLACE;
		clrMoveModePlace(&tm->data.place, b);
		return true;
	}
	case CMD_PLAY: {
		tm->type = TRANS_MOVE_SKIP_PLAY;
		return true;
	}
	default: break;
	}
	return false;
}

bool updateTransMove(struct TransMove *tm, struct Cmd *c, struct Board *b, struct Player *p)
{
	NOT(tm);
	NOT(c);
	NOT(b);
	NOT(p);
	
	switch (tm->type) {
	case TRANS_MOVE_PLACE: return updateTransMovePlace(tm, c, b, p);
	case TRANS_MOVE_PLACE_WILD: return updateTransMovePlaceWild(tm, c, b, p);
	case TRANS_MOVE_PLACE_END: return updateTransMovePlaceEnd(tm, c, b, p);
	case TRANS_MOVE_DISCARD: return updateTransMoveDiscard(tm, c, b, p);
	case TRANS_MOVE_SKIP: return updateTransMoveSkip(tm, c, b, p);
	case TRANS_MOVE_QUIT: /* fall through */
	case TRANS_MOVE_NONE:
	case TRANS_MOVE_INVALID: 
	default: tm->type = TRANS_MOVE_INVALID; break;
	}
	return false;
}

void clrTransMove(struct TransMove *tm, int pidx, struct Player *p, struct Board *b)
{
	NOT(tm);

	tm->type = TRANS_MOVE_PLACE;
	tm->playerIdx = pidx;
	mkAdjust(&tm->adjust, p);
	clrMoveModePlace(&tm->data.place, b);
}

void moveModePlaceToMovePlace(struct MovePlace *mp, struct MoveModePlace *mmp, struct Adjust *a)
{
	int i, ridx, j, k;
	struct Coor idx;
	
	NOT(mp);
	NOT(mmp);
	NOT(a);
	
	mp->num = mmp->num;
	for (i = 0, k = 0; i < RACK_SIZE; i++) {
		j = a->data.tile[i].idx;
		idx = mmp->boardIdx[j];
		if (!validBoardIdx(idx)) {
			continue;
		}
		ridx = a->data.tile[mmp->rackIdx[idx.y][idx.x]].idx;
		VALID_RACK_SIZE(ridx);
		mp->rackIdx[k] = ridx;
		mp->coor[k] = mmp->boardIdx[j];
		k++;
	}
	assert(mp->num == k);
}

void moveModeDiscardToMoveDiscard(struct MoveDiscard *md, struct MoveModeDiscard *mmd, struct Adjust *a)
{
	int i, j, k;
	
	NOT(md);
	NOT(mmd);
	NOT(a);
	
	md->num = mmd->num;
	for (i = 0, j = 0; i < RACK_SIZE; i++) {
		k = a->data.tile[i].idx;
		md->rackIdx[k] = -1;
		if (mmd->rack[k]) {
			md->rackIdx[j] = k;
			j++;
		}
	}
	assert(md->num == j);
}

bool transMoveToMove(struct Move *m, struct TransMove *tm)
{
	NOT(m);
	NOT(tm);
	
	m->playerIdx = tm->playerIdx;
	switch (tm->type) {
	case TRANS_MOVE_PLACE_PLAY: {
		m->type = MOVE_PLACE;
		moveModePlaceToMovePlace(&m->data.place, &tm->data.place, &tm->adjust);
		return true;
	}
	case TRANS_MOVE_DISCARD_PLAY: {
		m->type = MOVE_DISCARD;
		moveModeDiscardToMoveDiscard(&m->data.discard, &tm->data.discard, &tm->adjust);
		return true;
	}
	case TRANS_MOVE_SKIP_PLAY: m->type = MOVE_SKIP; return true;
	case TRANS_MOVE_QUIT: m->type = MOVE_QUIT; return true;
	default: m->type = MOVE_INVALID; break;
	}
	return false;
}

void update(struct Env *e)
{
	struct Cmd c;
	struct Move m;
	struct Action a;
	
	NOT(e);
	
	if (e->transMove.type == TRANS_MOVE_INVALID) {
		clrTransMove(&e->transMove, e->game.turn, &e->game.player[e->game.turn], &e->game.board);
		c.type = CMD_INVALID;
		updateTransMove(&e->transMove, &c, &e->game.board, &e->game.player[e->game.turn]);
		updateBoardWidget(&e->gui.gameGui.boardWidget, &e->transMove, &e->game.board); 
		updateChoiceWidget(&e->gui.gameGui.choiceWidget, &e->transMove);
		updateRackWidget(&e->gui.gameGui.rackWidget, &e->transMove);
	}
	
	switch (e->gui.gameGui.focus) {
	case GUI_FOCUS_BOARD: boardWidgetControls(&c, &e->gui.gameGui, &e->controls); break;
	case GUI_FOCUS_CHOICE: choiceWidgetControls(&c, &e->gui.gameGui, &e->controls); break;
	case GUI_FOCUS_RACK: rackWidgetControls(&c, &e->gui.gameGui, &e->controls); break;
	default: break;
	}
	updateGameGUI(&e->gui.gameGui, &c, e->transMove.type);
	
	switch (c.type) {
	case CMD_FOCUS_TOP: {
		e->gui.gameGui.focus = GUI_FOCUS_BOARD;
		break;
	}
	case CMD_FOCUS_BOTTOM: {
		if (e->gui.gameGui.bottomLast != GUI_FOCUS_CHOICE) {
			e->gui.gameGui.focus = GUI_FOCUS_RACK;
		} else {
			e->gui.gameGui.focus = GUI_FOCUS_CHOICE;
		}
		break;
	}
	case CMD_BOARD: {
		e->gui.gameGui.focus = GUI_FOCUS_BOARD;
		e->gui.gameGui.boardWidget.index = c.data.board;
		break;
	}
	case CMD_RACK: {
		e->gui.gameGui.focus = GUI_FOCUS_RACK;
		e->gui.gameGui.rackWidget.index.x = c.data.rack;
		e->gui.gameGui.rackWidget.index.y = 0;
		break;
	}
	case CMD_CHOICE: {
		e->gui.gameGui.focus = GUI_FOCUS_CHOICE;
		e->gui.gameGui.choiceWidget.index.x = c.data.choice;
		e->gui.gameGui.choiceWidget.index.y = 0;
		break;
	}
	default: break;
	}
	if (e->gui.gameGui.focus != GUI_FOCUS_BOARD) {
		
		e->gui.gameGui.bottomLast = e->gui.gameGui.focus;
	}

	printCmd(&c);

	if (updateTransMove(&e->transMove, &c, &e->game.board, &e->game.player[e->game.turn])) {
		printTransMove(&e->transMove);
		updateBoardWidget(&e->gui.gameGui.boardWidget, &e->transMove, &e->game.board); 
		updateChoiceWidget(&e->gui.gameGui.choiceWidget, &e->transMove);
		updateRackWidget(&e->gui.gameGui.rackWidget, &e->transMove);
	}

	transMoveToMove(&m, &e->transMove);

	mkAction(&a, &e->game, &m);
	applyAction(&e->game, &a);
	if (a.type != ACTION_INVALID) {
		printf("[PLAYER_%d: %d]\n", a.playerIdx, e->game.player[a.playerIdx].score);
		nextTurn(&e->game);
		e->transMove.type = TRANS_MOVE_INVALID;
	} else {
		if (m.type != MOVE_INVALID) {
			printActionErr(a.type);
		}
	} 
	e->io.time += 1.0f / 60.0f;
}

void guiDrawLockon(struct IO *io, struct GameGUI *gg)
{
	const int w = 14, h = 14;
	struct Coor idx;

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

#define POS_X 106
#define POS_Y 5

void guiDrawGhostTile(struct IO *io, GUIFocusType gf, struct TransMove *tm, struct Player *p, struct GridWidget *bw)
{
	int i;
	struct Coor idx;
	SDL_Surface *s;
	struct Tile *t;

	NOT(io);
	NOT(tm);
	NOT(p);
	NOT(bw);
	
	if (gf != GUI_FOCUS_BOARD) {
		return;
	}

	switch (tm->type) {
	case TRANS_MOVE_PLACE:
	case TRANS_MOVE_PLACE_END: {
		i = tm->adjust.data.tile[tm->data.place.idx].idx;
		idx = bw->index;
		t = &p->tile[i];
		s = t->type == TILE_WILD ? io->wild[TILE_LOOK_GHOST] : io->tile[t->type][t->letter][TILE_LOOK_GHOST];
		surfaceDraw(io->screen, s, idx.x * TILE_WIDTH + POS_X, idx.y * TILE_HEIGHT + POS_Y);
		break;
	}
	default: break;
	}
}

void guiDraw(struct IO *io, struct GUI *g, struct Game *gm, struct TransMove *tm)
{
	struct Coor pos, dim;
	
	NOT(io);
	NOT(g);
	NOT(tm);
	
	dim.x = 14;
	dim.y = 14;
	
	pos.x = 106;
	pos.y = 5;
	/* gridWidgetDraw(io->screen, &g->gameGui.boardWidget, pos, dim); */
	boardWidgetDraw(io, &g->gameGui.boardWidget, &gm->player[tm->playerIdx], &gm->board, tm, pos, dim);
	
	pos.x = 162;
	pos.y = 222;
	/* gridWidgetDraw(io->screen, &g->gameGui.rackWidget, pos, dim); */
	rackWidgetDraw(io, tm, &g->gameGui.rackWidget, pos, dim, &gm->player[tm->playerIdx]);
	
	pos.x = 106;
	pos.y = 222;
	/* gridWidgetDraw(io->screen, &g->gameGui.choiceWidget, pos, dim); */
	choiceWidgetDraw(io, tm, &g->gameGui.choiceWidget, pos, dim);
	
	if (gm->turn == tm->playerIdx && ((io->time * 2.0 - floor(io->time * 2.0)) ) < 0.5) {
		guiDrawGhostTile(io, g->gameGui.focus, tm, &gm->player[tm->playerIdx], &g->gameGui.boardWidget);
	}
	guiDrawLockon(io, &g->gameGui);
}

void draw(struct Env *e)
{
	NOT(e);

	SDL_FillRect(e->io.screen, NULL, 0);
	surfaceDraw(e->io.screen, e->io.back, 0, 0);
	guiDraw(&e->io, &e->gui, &e->game, &e->transMove); 
	SDL_Flip(e->io.screen);
}

void exec(struct Env *e)
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

int gui()
{
	int exitStatus = EXIT_FAILURE;
	struct Env e;
	if (init(&e)) {
		exec(&e);
		exitStatus = EXIT_SUCCESS;
	}
	quit(&e);
	return exitStatus;
}

