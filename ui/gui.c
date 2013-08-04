#include <math.h>

#include "gui.h"
#include "init.h"
#include "widget.h"
#include "print.h"


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

	if (f->map) {
		surfaceFree(f->map);
	}
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
	
	keystateInit(&c->start);
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

void initGame1vs1Human(struct Game *g)
{
	NOT(g);

	boardInit(&g->board);
	bagInit(&g->bag);
	g->playerNum = 2;
	playerInit(&g->player[0], &g->bag);
	playerInit(&g->player[1], &g->bag);
	g->player[0].active = true;
	g->player[1].active = true;
}

void initMenu(struct Menu *m)
{
	NOT(m);
}

void initGameMenu(struct GameMenu *gm)
{
	NOT(gm);

	gm->focus = GAME_MENU_FOCUS_RESUME;
}

void initGameGUI(struct GameGUI *gg)
{
	NOT(gg);
	
	mkRackWidget(&gg->rackWidget);
	mkChoiceWidget(&gg->choiceWidget);
	mkBoardWidget(&gg->boardWidget);
	gg->focus = GAME_GUI_FOCUS_CHOICE;
	gg->bottomLast = GAME_GUI_FOCUS_CHOICE;
	gg->choiceWidget.index.x = 1;
}

void initGUI(struct GUI *g)
{
	initMenu(&g->menu);
	initGameMenu(&g->gameMenu);
	initGameGUI(&g->gameGui);
	g->gameAreYouSureQuit = 0;
	g->focus = GUI_FOCUS_TITLE;
}

SDL_Surface *createText(struct Font *f, char *str)
{
	SDL_Surface *text;
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	NOT(f);
	NOT(str);
	
	text = SDL_CreateRGBSurface(SDL_SWSURFACE, f->width * strlen(str), f->height, SCREEN_BPP, rmask, gmask, bmask, amask);
	SDL_SetColorKey(text, SDL_SRCCOLORKEY, SDL_MapRGB(text->format, 0xff, 0x00, 0xff));
	strDraw(text, f, str, 0, 0);
	return text;
}

SDL_Surface *createOutlineText(struct Font *fIn, struct Font *fOut, char *str)
{
	int i, j;
	SDL_Surface *text;
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	NOT(fIn);
	NOT(fOut);
	NOT(str);
	assert(fIn->width == fOut->width);
	assert(fIn->height == fOut->height);
	
	text = SDL_CreateRGBSurface(SDL_SWSURFACE, fIn->width * strlen(str) + 2, fIn->height + 2, SCREEN_BPP, rmask, gmask, bmask, amask);
	SDL_SetColorKey(text, SDL_SRCCOLORKEY, SDL_MapRGB(text->format, 0xff, 0x00, 0xff));
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {	
			strDraw(text, fOut, str, i, j);
		}
	}
	strDraw(text, fIn, str, 1, 1); 
	return text;
}

void mkHighTexts(struct HighText *ht, struct Font *fIn, struct Font *fOut, struct Font *hfIn, struct Font *hfOut, char **text, int count)
{
	int max, i, len;

	NOT(ht);
	NOT(fIn);
	NOT(fOut);
	NOT(hfIn);
	NOT(hfOut);
	NOT(text);
	assert(count > 0);
	assert(fIn->width == fOut->width && fIn->height == fOut->height);
	assert(hfIn->width == hfOut->width && hfIn->height == hfOut->height);
	assert(fIn->width == hfOut->width && fIn->height == hfOut->height);

	max = 0;
	for (i = 0; i < count; i++) {
		ht[i].normal = createOutlineText(fIn, fOut, text[i]);
		ht[i].highlight = createOutlineText(hfIn, hfOut, text[i]);
		len = strlen(text[i]);
		max = MAX(len, max);
	}
	for (i = 0; i < count; i++) {
		len = strlen(text[i]);
		ht[i].offset = (max - len) * fIn->width / 2;
	}
}

bool areHighTextsLoaded(struct HighText *ht, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (!(ht[i].normal && ht[i].highlight)) {
			return false;
		}
	}
	return true;
}

void freeHighTexts(struct HighText *ht, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		surfaceFree(ht[i].normal);
		surfaceFree(ht[i].highlight);
	}
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
	if ((e->io.titleScreen = surfaceLoad(RES_PATH "title_screen.png")) == NULL) {
		return false;
	}
	if ((e->io.titleBackground = surfaceLoad(RES_PATH "title_background.png")) == NULL) {
		return false;
	}
	if ((e->io.back = surfaceLoad(RES_PATH "back.png")) == NULL) {
		return false;
	}
	if ((e->io.wildUp = surfaceLoad(RES_PATH "wild_up.png")) == NULL) {
		return false;
	}
	if ((e->io.wildDown = surfaceLoad(RES_PATH "wild_down.png")) == NULL) {
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
	if ((e->io.shuffle = surfaceLoad(RES_PATH "shuffle.png")) == NULL) {
		return false;
	}
	if ((e->io.shuffleDisable = surfaceLoad(RES_PATH "shuffle_disable.png")) == NULL) {
		return false;
	}
	if (!fontmapInit(&e->io.whiteFont, 6, 12, RES_PATH "white_font.png")) {
		return false;
	}
	if (!fontmapInit(&e->io.blackFont, 6, 12, RES_PATH "black_font.png")) {
		return false;
	}
	if (!fontmapInit(&e->io.yellowFont, 6, 12, RES_PATH "yellow_font.png")) {
		return false;
	}
	if (!fontmapInit(&e->io.darkRedFont, 6, 12, RES_PATH "darkred_font.png")) {
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
			strDraw(e->io.tile[TILE_WILD][i][j], &e->io.blackFont, str, 3, 0);
			sprintf(str,"%c", i + 'A');
			strDraw(e->io.tile[TILE_LETTER][i][j], &e->io.blackFont, str, 3, 0);
		}
	}
	for (i = 0; i < TILE_LOOK_COUNT; i++) {
		surfaceFree(tile[i]);
	}

	e->io.fader = surfaceCpy(e->io.screen);

	e->io.pressStart = createOutlineText(&e->io.whiteFont, &e->io.blackFont, "PRESS START");

	{
		char *text[MENU_FOCUS_COUNT] = {"Start", "Exit"};
		mkHighTexts(e->io.menuFocus, &e->io.whiteFont, &e->io.blackFont, &e->io.yellowFont, &e->io.darkRedFont, text, MENU_FOCUS_COUNT);
	}
	if (!areHighTextsLoaded(e->io.menuFocus, MENU_FOCUS_COUNT)) {
		return false;
	}

	{
		char *text[GAME_MENU_FOCUS_COUNT] = {"Resume", "Quit"};
		mkHighTexts(e->io.gameMenuFocus, &e->io.whiteFont, &e->io.blackFont, &e->io.yellowFont, &e->io.darkRedFont, text, GAME_MENU_FOCUS_COUNT);
	}
	if (!areHighTextsLoaded(e->io.gameMenuFocus, GAME_MENU_FOCUS_COUNT)) {
		return false;
	}

	e->io.areYouSureQuit = createOutlineText(&e->io.whiteFont, &e->io.blackFont, "Are you sure you want to quit?");
	{
		char *text[GAME_MENU_FOCUS_COUNT] = {"Yes", "No"};
		mkHighTexts(e->io.yesNo, &e->io.whiteFont, &e->io.blackFont, &e->io.yellowFont, &e->io.darkRedFont, text, YES_NO_COUNT);
	}
	if (!areHighTextsLoaded(e->io.yesNo, YES_NO_COUNT)) {
		return false;
	}

	controlsInit(&e->controls);

	e->transMove.type = TRANS_MOVE_INVALID;
	initGame1vs1Human(&e->game);
	initGUI(&e->gui);

/*
	{
		struct Move move;
		struct Action action;

		aiFindMove(&move, e->game.player, &e->game.board);
		mkAction(&action, &e->game, &move, NULL);
		printAction(&action);
		
		e->game.turn = 1;
		applyAction(&e->game, &action);
		e->game.turn = 0;
	}
*/

	return true;
}

void quit(struct Env *e)
{
	int i, j;

	NOT(e);

	dictQuit(&e->game.dict);
	surfaceFree(e->io.screen);
	surfaceFree(e->io.titleScreen);
	surfaceFree(e->io.titleBackground);
	surfaceFree(e->io.pressStart);
	surfaceFree(e->io.back);
	surfaceFree(e->io.lockon);
	surfaceFree(e->io.wildUp);
	surfaceFree(e->io.wildDown);
	surfaceFree(e->io.recall);
	surfaceFree(e->io.recallDisable);
	surfaceFree(e->io.mode);
	surfaceFree(e->io.modeDisable);
	surfaceFree(e->io.place);
	surfaceFree(e->io.discard);
	surfaceFree(e->io.discardDisable);
	surfaceFree(e->io.skip);
	surfaceFree(e->io.skipDisable);
	surfaceFree(e->io.play);
	surfaceFree(e->io.playDisable);
	surfaceFree(e->io.shuffle);
	surfaceFree(e->io.shuffleDisable);
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
	freeHighTexts(e->io.menuFocus, MENU_FOCUS_COUNT);
	freeHighTexts(e->io.gameMenuFocus, GAME_MENU_FOCUS_COUNT);
	surfaceFree(e->io.areYouSureQuit);
	freeHighTexts(e->io.yesNo, YES_NO_COUNT);
	fontmapQuit(&e->io.whiteFont);
	fontmapQuit(&e->io.blackFont);
	fontmapQuit(&e->io.yellowFont);
	fontmapQuit(&e->io.darkRedFont);
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
	keyStateUpdate(&c->start, ks[SDLK_RETURN]);
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
	case CMD_BOARD_LEFT: puts("[cmd:board-left]"); break;
	case CMD_BOARD_RIGHT: puts("[cmd:board-right]"); break;
	case CMD_BOARD_UP: puts("[cmd:board-up]"); break;
	case CMD_BOARD_DOWN: puts("[cmd:board-down]"); break;
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
	case CMD_SHUFFLE: puts("[cmd:shuffle]"); break;
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

	mmp = &tm->place;

	do {
		mmp->idx++;
		mmp->idx %= RACK_SIZE;
	} while (validBoardIdx(mmp->boardIdx[mmp->idx]) || tm->adjust.data.tile[mmp->idx].type == TILE_NONE);
}

void shuffleRackTransMove(struct TransMove *tm)
{

	int val[RACK_SIZE], i, j, k;
	bool d;
	struct TileAdjust tmp;
	struct Coor coor;

	NOT(tm);

	for (i = 0; i < RACK_SIZE; i++) {
		val[i] = rand();
	}

	for (i = 0; i < RACK_SIZE; i++) {
		for (j = 0; j < RACK_SIZE; j++) {
			if (val[i] > val[j]) {
				if (validBoardIdx(tm->place.boardIdx[i])) {
						tm->place.rackIdx
						[tm->place.boardIdx[i].y]
						[tm->place.boardIdx[i].x] = j;
				}
				if (validBoardIdx(tm->place.boardIdx[j])) {
					tm->place.rackIdx
						[tm->place.boardIdx[j].y]
						[tm->place.boardIdx[j].x] = i;
				}

				coor = tm->place.boardIdx[i];
				tm->place.boardIdx[i] = tm->place.boardIdx[j];
				tm->place.boardIdx[j] = coor;

				k = val[i];
				val[i] = val[j];
				val[j] = k;

				tmp = tm->adjust.data.tile[i];
				tm->adjust.data.tile[i] = tm->adjust.data.tile[j];
				tm->adjust.data.tile[j] = tmp;
				
				if (tm->place.idx == i) {
					tm->place.idx = j;
				} else if (tm->place.idx == j) {
					tm->place.idx = i;
				}
				d = tm->discard.rack[i];
				tm->discard.rack[i] = tm->discard.rack[j];
				tm->discard.rack[j] = d;
			}
		}
	}
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
	
	mmp = &tm->place;
	
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
			int a0, a1;
			struct TileAdjust b0, b1;
			a0 = mmp->rackIdx[c->data.board.y][c->data.board.x];
			a1 = mmp->idx;
			b0 = tm->adjust.data.tile[a0];
			b1 = tm->adjust.data.tile[a1];
			tm->adjust.data.tile[a0] = b1;
			tm->adjust.data.tile[a1] = b0;
			if (tm->adjust.data.tile[a0].type == TILE_WILD) {
				tm->type = TRANS_MOVE_PLACE_WILD;
				mmp->idx = a0;
			}
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
		return true;
	}
	case CMD_PLAY: {
		tm->type = TRANS_MOVE_PLACE_PLAY;
		return true;
	}
	case CMD_SHUFFLE: {
		shuffleRackTransMove(tm);
		return true;
	}
	default: break;
	}
	return false;
}

bool updateTransMovePlacePlay(struct TransMove *tm, struct Cmd *c, struct Board *b, struct Player *p)
{
	struct MoveModePlace *mmp;
	
	NOT(tm);
	NOT(c);
	NOT(b);
	NOT(p);
	assert(tm->type == TRANS_MOVE_PLACE_PLAY);
	assert(c->type != CMD_QUIT);

	mmp = &tm->place;
	tm->type = (adjustTileCount(&tm->adjust) == mmp->num) ? TRANS_MOVE_PLACE_END : TRANS_MOVE_PLACE;

	return true;
}

bool updateTransMovePlaceWild(struct TransMove *tm, struct Cmd *c, struct Board *b, struct Player *p)
{
	struct MoveModePlace *mmp;

	NOT(tm);
	NOT(c);
	NOT(b);
	NOT(p);
	assert(tm->type == TRANS_MOVE_PLACE_WILD);
	
	mmp = &tm->place;
	
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
	
	mmp = &tm->place;
	
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
	
	mmd = &tm->discard;
	
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
	case CMD_SHUFFLE: {
		shuffleRackTransMove(tm);
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
		return true;
	}
	case CMD_MODE_DOWN: {
		tm->type = TRANS_MOVE_PLACE;
		return true;
	}
	case CMD_PLAY: {
		tm->type = TRANS_MOVE_SKIP_PLAY;
		return true;
	}
	case CMD_SHUFFLE: {
		shuffleRackTransMove(tm);
		return true;
	}
	default: break;
	}
	return false;
}

bool updateTransMoveQuit(struct TransMove *tm, struct Cmd *c, struct Board *b, struct Player *p)
{
	NOT(tm);
	NOT(c);
	NOT(b);
	NOT(p);

	p->active = false;
	return true;
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
	case TRANS_MOVE_PLACE_PLAY: return updateTransMovePlacePlay(tm, c, b, p);
	case TRANS_MOVE_DISCARD: return updateTransMoveDiscard(tm, c, b, p);
	case TRANS_MOVE_SKIP: return updateTransMoveSkip(tm, c, b, p);
	case TRANS_MOVE_QUIT: return updateTransMoveQuit(tm, c, b, p);
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
	clrMoveModePlace(&tm->place, b);
	clrMoveModeDiscard(&tm->discard);
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
		moveModePlaceToMovePlace(&m->data.place, &tm->place, &tm->adjust);
		return true;
	}
	case TRANS_MOVE_DISCARD_PLAY: {
		m->type = MOVE_DISCARD;
		moveModeDiscardToMoveDiscard(&m->data.discard, &tm->discard, &tm->adjust);
		return true;
	}
	case TRANS_MOVE_SKIP_PLAY: m->type = MOVE_SKIP; return true;
	case TRANS_MOVE_QUIT: m->type = MOVE_QUIT; return true;
	default: m->type = MOVE_INVALID; break;
	}
	return false;
}

void quitGame(struct Env *e)
{
	NOT(e);
	
	e->transMove.type = TRANS_MOVE_QUIT;
	e->gui.focus = GUI_FOCUS_GAME_GUI;
}

void updateTitle(struct Env *e)
{
	NOT(e);
	if (e->controls.start.type == KEY_STATE_PRESSED) {
		e->gui.focus = GUI_FOCUS_MENU;
	}
}

void updateMenu(struct Env *e)
{
	struct Cmd c;
	
	NOT(e);
	
	if (e->controls.start.type == KEY_STATE_PRESSED) {
		e->gui.focus = GUI_FOCUS_GAME_GUI;

		clrTransMove(&e->transMove, e->game.turn, &e->game.player[e->game.turn], &e->game.board);
		c.type = CMD_INVALID;
		updateTransMove(&e->transMove, &c, &e->game.board, &e->game.player[e->game.turn]);
		updateBoardWidget(&e->gui.gameGui.boardWidget, &e->transMove, &e->game.board); 
		updateChoiceWidget(&e->gui.gameGui.choiceWidget, &e->transMove);
		updateRackWidget(&e->gui.gameGui.rackWidget, &e->transMove);
		return;
	}
	if (e->controls.b.type == KEY_STATE_PRESSED) {
		e->gui.focus = GUI_FOCUS_TITLE;
	}
}

void updateGameGui(struct Env *e)
{
	struct Cmd c;
	struct Move m;
	struct Action a;
	struct Log l;
	
	NOT(e);

	if (e->controls.start.type == KEY_STATE_PRESSED) {
		e->gui.focus = GUI_FOCUS_GAME_MENU;
		return;
	}
	
	if (e->transMove.type == TRANS_MOVE_INVALID) {
		puts("yes??");
		clrTransMove(&e->transMove, e->game.turn, &e->game.player[e->game.turn], &e->game.board);
		c.type = CMD_INVALID;
		updateTransMove(&e->transMove, &c, &e->game.board, &e->game.player[e->game.turn]);
		updateBoardWidget(&e->gui.gameGui.boardWidget, &e->transMove, &e->game.board); 
		updateChoiceWidget(&e->gui.gameGui.choiceWidget, &e->transMove);
		updateRackWidget(&e->gui.gameGui.rackWidget, &e->transMove);
	}
	
	switch (e->gui.gameGui.focus) {
	case GAME_GUI_FOCUS_BOARD: boardWidgetControls(&c, &e->gui.gameGui, &e->controls); break;
	case GAME_GUI_FOCUS_CHOICE: choiceWidgetControls(&c, &e->gui.gameGui, &e->controls); break;
	case GAME_GUI_FOCUS_RACK: rackWidgetControls(&c, &e->gui.gameGui, &e->controls); break;
	default: break;
	}
	updateGameGUI(&e->gui.gameGui, &c, e->transMove.type);
	
	switch (c.type) {
	case CMD_FOCUS_TOP: {
		struct TransMove *tm;
		tm = &e->transMove;
		if (		tm->type == TRANS_MOVE_PLACE ||
				tm->type == TRANS_MOVE_PLACE_WILD ||
				tm->type == TRANS_MOVE_PLACE_END ||
				tm->type == TRANS_MOVE_PLACE_PLAY) {
			e->gui.gameGui.focus = GAME_GUI_FOCUS_BOARD;
		}
		break;
	}
	case CMD_FOCUS_BOTTOM: {
		if (e->gui.gameGui.bottomLast != GAME_GUI_FOCUS_CHOICE) {
			e->gui.gameGui.focus = GAME_GUI_FOCUS_RACK;
		} else {
			e->gui.gameGui.focus = GAME_GUI_FOCUS_CHOICE;
		}
		break;
	}
	case CMD_BOARD: {
		e->gui.gameGui.focus = GAME_GUI_FOCUS_BOARD;
		e->gui.gameGui.boardWidget.index = c.data.board;
		break;
	}
	case CMD_RACK: {
		if (e->transMove.type == TRANS_MOVE_SKIP) {
			break;
		}
		e->gui.gameGui.focus = GAME_GUI_FOCUS_RACK;
		e->gui.gameGui.rackWidget.index.x = c.data.rack;
		e->gui.gameGui.rackWidget.index.y = 0;
		break;
	}
	case CMD_CHOICE: {
		e->gui.gameGui.focus = GAME_GUI_FOCUS_CHOICE;
		e->gui.gameGui.choiceWidget.index.x = c.data.choice;
		e->gui.gameGui.choiceWidget.index.y = 0;
		break;
	}
	default: break;
	}
	if (e->gui.gameGui.focus != GAME_GUI_FOCUS_BOARD) {
		
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

	mkAction(&a, &e->game, &m, NULL);
	applyAction(&e->game, &a);

	if (c.type == CMD_PLAY || c.type == CMD_QUIT) {
		mkLog(&a, &l);
		printLog(&l);
	}

	if (a.type != ACTION_INVALID) {
		printf("[PLAYER_%d: %d]\n", a.playerIdx, e->game.player[a.playerIdx].score);
		applyAdjust(&e->game.player[a.playerIdx], &e->transMove.adjust);
		if (endGame(&e->game)) {
			e->gui.focus = GUI_FOCUS_GAME_OVER;
		} else {
			nextTurn(&e->game);
			clrTransMove(&e->transMove, e->game.turn, &e->game.player[e->game.turn], &e->game.board);
		}
		e->transMove.type = TRANS_MOVE_INVALID;
	} else {
		if (m.type != MOVE_INVALID) {
			printActionErr(a.type);
		}
	} 
}

void updateGameMenu(struct Env *e)
{
	NOT(e);
	
	if (e->controls.start.type == KEY_STATE_PRESSED) {
		switch (e->gui.gameMenu.focus) {
		case GAME_MENU_FOCUS_RESUME: {
			e->gui.focus = GUI_FOCUS_GAME_GUI;
			break;
		}
		case GAME_MENU_FOCUS_QUIT: {
			e->gui.focus = GUI_FOCUS_GAME_ARE_YOU_SURE_QUIT;
			break;
		}
		default: break;
		}
		return;
	}
	if (e->controls.up.type == KEY_STATE_PRESSED) {
		e->gui.gameMenu.focus += GAME_MENU_FOCUS_COUNT;
		e->gui.gameMenu.focus--;
		e->gui.gameMenu.focus %= GAME_MENU_FOCUS_COUNT;
		return;
	}
	if (e->controls.down.type == KEY_STATE_PRESSED) {
		e->gui.gameMenu.focus++;
		e->gui.gameMenu.focus %= GAME_MENU_FOCUS_COUNT;
	}
}

void updateGameAreYouSureQuit(struct Env *e)
{
	NOT(e);
	if (e->controls.down.type == KEY_STATE_PRESSED) {
		e->gui.gameAreYouSureQuit++;
		e->gui.gameAreYouSureQuit %= YES_NO_COUNT;
		return;
	}
	if (e->controls.up.type == KEY_STATE_PRESSED) {
		e->gui.gameAreYouSureQuit += YES_NO_COUNT;
		e->gui.gameAreYouSureQuit--;
		e->gui.gameAreYouSureQuit %= YES_NO_COUNT;
		return;
	}
	if (e->controls.a.type == KEY_STATE_PRESSED || e->controls.start.type == KEY_STATE_PRESSED) {
		if (e->gui.gameAreYouSureQuit == YES) {
			quitGame(e);
		} else {
			e->gui.focus = GUI_FOCUS_GAME_MENU;
		}
		return;
	}
	if (e->controls.b.type == KEY_STATE_PRESSED) {
		e->gui.focus = GUI_FOCUS_GAME_MENU;
		return;
	}
}

void updateGameOver(struct Env *e)
{
	NOT(e);
	
	if (e->controls.start.type == KEY_STATE_PRESSED) {
		e->gui.focus = GUI_FOCUS_TITLE;
		return;
	}
}

void update(struct Env *e)
{
	NOT(e);

	switch (e->gui.focus) {
	case GUI_FOCUS_TITLE: updateTitle(e); break;
	case GUI_FOCUS_MENU: updateMenu(e); break;
	case GUI_FOCUS_GAME_GUI: updateGameGui(e); break;
	case GUI_FOCUS_GAME_MENU: updateGameMenu(e); break;
	case GUI_FOCUS_GAME_OVER: updateGameOver(e); break;
	case GUI_FOCUS_GAME_ARE_YOU_SURE_QUIT: updateGameAreYouSureQuit(e); break;
	default: break;
	}
	e->io.time += 1.0f / ((float)(FPS));
}

void guiDrawLockon(struct IO *io, struct GameGUI *gg)
{
	const int w = 14, h = 14;
	struct Coor idx;

	NOT(io);
	NOT(gg);
	
	switch (gg->focus) {
	case GAME_GUI_FOCUS_BOARD: {
		idx = gg->boardWidget.index;
		surfaceDraw(io->screen, io->lockon, 104 + idx.x * w, 4 + idx.y * h);
		break;
	}
	case GAME_GUI_FOCUS_RACK: {
		idx = gg->rackWidget.index;
		surfaceDraw(io->screen, io->lockon, 174 + idx.x * w, 219);
		break;
	}
	case GAME_GUI_FOCUS_CHOICE: {
		idx = gg->choiceWidget.index;
		surfaceDraw(io->screen, io->lockon, 104 + idx.x * w, 219);
		break;
	}
	default: break;
	}
}

#define POS_X 106
#define POS_Y 5

void guiDrawGhostTile(struct IO *io, GameGUIFocusType gf, struct TransMove *tm, struct Player *p, struct GridWidget *bw)
{
	int i;
	struct Coor idx;
	SDL_Surface *s;
	struct Tile *t;

	NOT(io);
	NOT(tm);
	NOT(p);
	NOT(bw);
	
	if (gf != GAME_GUI_FOCUS_BOARD) {
		return;
	}

	switch (tm->type) {
	case TRANS_MOVE_PLACE: {
		i = tm->adjust.data.tile[tm->place.idx].idx;
		idx = bw->index;
		t = &p->tile[i];
		s = t->type == TILE_WILD ? io->wild[TILE_LOOK_GHOST] : io->tile[t->type][t->letter][TILE_LOOK_GHOST];
		surfaceDraw(io->screen, s, idx.x * TILE_WIDTH + POS_X, idx.y * TILE_HEIGHT + POS_Y);
		break;
	}
	case TRANS_MOVE_PLACE_WILD: {
		i = tm->adjust.data.tile[tm->place.idx].idx;
		idx = bw->index;
		t = &p->tile[i];
		surfaceDraw(io->screen, io->wildUp, idx.x * TILE_WIDTH + POS_X,
			(idx.y-1) * TILE_HEIGHT + POS_Y + (TILE_HEIGHT/2));
		surfaceDraw(io->screen, io->wildDown, idx.x * TILE_WIDTH + POS_X,
			(idx.y+1) * TILE_HEIGHT + POS_Y);
		break;
	}
	default: break;
	}
}

void guiDrawBoard(struct IO *io, struct GridWidget *bw, struct Game *g, struct TransMove *tm)
{
	struct Coor pos, dim;
	
	NOT(io);
	NOT(bw);
	NOT(g);
	NOT(tm);

	dim.x = TILE_WIDTH;
	dim.y = TILE_HEIGHT;
	pos.x = 106;
	pos.y = 5;

	boardWidgetDraw(io, bw, &g->player[tm->playerIdx], &g->board, tm, pos, dim);
}

void guiDrawRack(struct IO *io, struct GridWidget *rw, struct Game *g, struct TransMove *tm)
{
	struct Coor pos, dim;
	
	NOT(io);
	NOT(rw);
	NOT(g);
	NOT(tm);

	dim.x = TILE_WIDTH;
	dim.y = TILE_HEIGHT;
	pos.x = 162;
	pos.y = 222;

	rackWidgetDraw(io, tm, rw, pos, dim, &g->player[tm->playerIdx]);
}

void guiDraw(struct IO *io, struct GUI *g, struct Game *gm, struct TransMove *tm)
{
	struct Coor pos, dim;
	
	NOT(io);
	NOT(g);
	NOT(tm);
	
	dim.x = 14;
	dim.y = 14;

	guiDrawBoard(io, &g->gameGui.boardWidget, gm, tm);
	guiDrawRack(io, &g->gameGui.rackWidget, gm, tm);
	
	pos.x = 106;
	pos.y = 222;
	choiceWidgetDraw(io, tm, &g->gameGui.choiceWidget, pos, dim);
	
	if (gm->turn == tm->playerIdx && ((io->time * 2.0 - floorf(io->time * 2.0)) ) < (1.0/2.0)) {
		guiDrawGhostTile(io, g->gameGui.focus, tm, &gm->player[tm->playerIdx], &g->gameGui.boardWidget);
	}
	guiDrawLockon(io, &g->gameGui);
}


void drawGameAreYouSureQuit(struct Env *e)
{
	int i;
	const int orgX = 150 - 8;
	const int orgY = 80;

	NOT(e);

	surfaceDraw(e->io.screen, e->io.back, 0, 0);
	guiDraw(&e->io, &e->gui, &e->game, &e->transMove); 
	SDL_FillRect(e->io.fader, 0, SDL_MapRGB(e->io.fader->format, 0, 0, 0));
	SDL_SetAlpha(e->io.fader, SDL_SRCALPHA, 218);
	surfaceDraw(e->io.screen, e->io.fader, 0, 0);
	surfaceDraw(e->io.screen, e->io.areYouSureQuit, orgX - e->io.whiteFont.width * 11, orgY - e->io.whiteFont.height);
	for (i = 0; i < YES_NO_COUNT; i++) {
		surfaceDraw(e->io.screen,
				e->gui.gameAreYouSureQuit == i ? e->io.yesNo[i].highlight : e->io.yesNo[i].normal,
				orgX, orgY + i * e->io.whiteFont.height);
	}
}

void drawScrollingBackground(struct Env *e)
{
	int bgOffset;
	int secsToLoop;
	int loopSpeed;
	
	NOT(e);

	loopSpeed = 15;
	secsToLoop = e->io.titleBackground->h / loopSpeed;
	bgOffset = (int)(e->io.titleBackground->h * e->io.time / secsToLoop) % e->io.titleBackground->h;
	surfaceDraw(e->io.screen, e->io.titleBackground, 0, bgOffset);
	surfaceDraw(e->io.screen, e->io.titleBackground, 0, bgOffset - e->io.titleBackground->h);
}

void draw(struct Env *e)
{
	NOT(e);

	SDL_FillRect(e->io.screen, NULL, 0);
	switch (e->gui.focus) {
	case GUI_FOCUS_TITLE: {
		drawScrollingBackground(e);
		surfaceDraw(e->io.screen, e->io.titleScreen, 0, 0);
		if ((e->io.time - floorf(e->io.time)) > 0.5) {
			surfaceDraw(e->io.screen, e->io.pressStart, 128, 200);
		}
		break;
	}
	case GUI_FOCUS_MENU: {
		drawScrollingBackground(e);
		surfaceDraw(e->io.screen, e->io.menuFocus[MENU_FOCUS_START].highlight, 120, 100);
		surfaceDraw(e->io.screen, e->io.menuFocus[MENU_FOCUS_EXIT].normal, 124, 120);
		break;
	}
	case GUI_FOCUS_GAME_GUI: {
		surfaceDraw(e->io.screen, e->io.back, 0, 0);
		guiDraw(&e->io, &e->gui, &e->game, &e->transMove); 
		break;
	}
	case GUI_FOCUS_GAME_MENU: {
		int i;
		surfaceDraw(e->io.screen, e->io.back, 0, 0);
		guiDraw(&e->io, &e->gui, &e->game, &e->transMove); 
		SDL_FillRect(e->io.fader, 0, SDL_MapRGB(e->io.fader->format, 0, 0, 0));
		SDL_SetAlpha(e->io.fader, SDL_SRCALPHA, 196);
		surfaceDraw(e->io.screen, e->io.fader, 0, 0);
		for (i = 0; i < GAME_MENU_FOCUS_COUNT; i++) {
			SDL_Surface *s;
			s = i == e->gui.gameMenu.focus ? e->io.gameMenuFocus[i].highlight : e->io.gameMenuFocus[i].normal;
			surfaceDraw(e->io.screen, s, 150 + e->io.gameMenuFocus[i].offset, i * e->io.whiteFont.height + 80);
		}
		break;
	}
	case GUI_FOCUS_GAME_OVER: {
		surfaceDraw(e->io.screen, e->io.back, 0, 0);
		guiDrawBoard(&e->io, &e->gui.gameGui.boardWidget, &e->game, &e->transMove);
		break;
	}
	case GUI_FOCUS_GAME_ARE_YOU_SURE_QUIT: drawGameAreYouSureQuit(e); break;
	default: break;
	}
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
		delay(st, SDL_GetTicks(), FPS);
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

