#include <math.h>

#include "gui.h"
#include "init.h"
#include "widget.h"
#include "print.h"

bool fontmapInit(Font *f, int w, int h, const char *filename)
{
	NOT(f);
	NOT(filename);

	f->width = w;
	f->height = h;
	f->spacing = 0;
	f->map = surfaceAlphaLoad(filename);
	return f->map != NULL;
}

void fontmapQuit(Font *f)
{
	NOT(f);

	if (f->map) {
		surfaceFree(f->map);
	}
}

void strDraw(SDL_Surface *s, Font *f, const char *str, int x, int y)
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
		offset.x += f->width + f->spacing;
	}
}

void keystateInit(KeyState *ks)
{
	NOT(ks);
	
	ks->type = KEY_STATE_UNTOUCHED;
	ks->time = 0.0f;
}

void controlsInit(Controls *c)
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

void initDefaultRule(Rule *r)
{
	NOT(r);

	r->place = NULL;
	r->discard = NULL;
	r->skip = NULL;
	r->quit = NULL;
}

void initGame1vs1Human(Game *g)
{
	NOT(g);

	boardInit(&g->board);
	bagInit(&g->bag);
	g->playerNum = 2;
	initPlayerHuman(&g->player[0], &g->bag);
	initPlayerHuman(&g->player[1], &g->bag);
	g->player[0].active = true;
	g->player[1].active = true;
	initDefaultRule(&g->rule);
}

void initGame1vs1HumanAI(Game *g)
{
	NOT(g);

	boardInit(&g->board);
	bagInit(&g->bag);
	g->playerNum = 2;
	initPlayerHuman(&g->player[0], &g->bag);
	initPlayerAI(&g->player[1], &g->bag);
	g->player[0].active = true;
	g->player[1].active = true;
	initDefaultRule(&g->rule);
}

void initScoreBoard(ScoreBoard *sb, Game *g)
{
	int i;

	NOT(sb);
	NOT(g);

	sb->turn = g->turn;	
	sb->playerNum = g->playerNum;
	sb->speed = 1.f/20.f;
	sb->stable = true;

	for (i = 0; i < sb->playerNum; i++) {
		sb->ctr[i].curTime = 0;
		sb->ctr[i].endTime = 0;
		sb->ctr[i].start = 0;
		sb->ctr[i].cur = 0;
		sb->ctr[i].end = 0;
		sb->ctr[i].stable = true;
	}
	
}

void initMenuWidget(MenuWidget *m, int focus, int max)
{
	m->init = focus;
	m->focus = focus;
	m->max = max;
}

void initSettings(Settings *s)
{
	NOT(s);

	s->sfxVolume = MAX_GUI_VOLUME;
	s->musVolume = MAX_GUI_VOLUME;
	s->previous = GUI_FOCUS_MENU; 
	initMenuWidget(&s->menu, SETTINGS_FOCUS_MUSIC, SETTINGS_FOCUS_COUNT);
}

void initGameGUI(GameGUI *gg)
{
	NOT(gg);
	
	mkRackWidget(&gg->rackWidget);
	mkChoiceWidget(&gg->choiceWidget);
	mkBoardWidget(&gg->boardWidget);
	gg->focus = GAME_GUI_FOCUS_CHOICE;
	gg->bottomLast = GAME_GUI_FOCUS_CHOICE;
	gg->choiceWidget.index.x = 1;
}

void initGUI(GUI *g)
{
	initMenuWidget(&g->menu, MENU_FOCUS_PLAY, MENU_FOCUS_COUNT);
	initMenuWidget(&g->gameMenu, GAME_MENU_FOCUS_RESUME, GAME_MENU_FOCUS_COUNT);
	initMenuWidget(&g->playMenu, PLAY_MENU_FOCUS_HUMAN_VS_AI, PLAY_MENU_FOCUS_COUNT);
	initSettings(&g->settings);
	initGameGUI(&g->gameGui);
	initMenuWidget(&g->gameAreYouSureQuit, YES, YES_NO_COUNT);
	g->focus = GUI_FOCUS_TITLE;
	g->next = GUI_FOCUS_TITLE;
}

SDL_Surface *createText(Font *f, char *str)
{
	SDL_Surface *text;
	Uint32 rm, gm, bm, am, a;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rm = 0xff000000;
    gm = 0x00ff0000;
    bm = 0x0000ff00;
    am = 0x000000ff;
#else
    rm = 0x000000ff;
    gm = 0x0000ff00;
    bm = 0x00ff0000;
    am = 0xff000000;
#endif
	text = SDL_CreateRGBSurface(SDL_SWSURFACE, (f->width + f->spacing) * strlen(str) + 1, f->height, SCREEN_BPP, rm, gm, bm, am);
	if (!text) {
		return NULL;
	}
	SDL_SetAlpha(text, 0, SDL_ALPHA_OPAQUE);
	a = SDL_MapRGB(text->format, ALPHA_R, ALPHA_G, ALPHA_B);
	SDL_FillRect(text, NULL, a);
	SDL_SetColorKey(text, SDL_SRCCOLORKEY, a);
	strDraw(text, f, str, 0, 0);
	return text;
}

int mkHighTexts(HighText *ht, Font *normal, Font *highlight, char **text, int count)
{
	int max, i, len;

	NOT(ht);
	NOT(normal);
	NOT(highlight);
	NOT(text);
	assert(count > 0);
	assert(normal->width == highlight->width);
	assert(normal->height == highlight->height);

	max = 0;
	for (i = 0; i < count; i++) {
		NOT(text[i]);
		ht[i].normal = createText(normal, text[i]);
		NOT(ht[i].normal);
		ht[i].highlight = createText(highlight, text[i]);
		NOT(ht[i].highlight);
		len = strlen(text[i]);
		max = MAX(len, max);
	}
	for (i = 0; i < count; i++) {
		len = strlen(text[i]);
		ht[i].offset = (max - len) * (normal->width + normal->spacing) / 2;
	}
	return max;
}

bool areHighTextsLoaded(HighText *ht, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (!(ht[i].normal && ht[i].highlight)) {
			return false;
		}
	}
	return true;
}

void freeHighTexts(HighText *ht, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		surfaceFree(ht[i].normal);
		surfaceFree(ht[i].highlight);
	}
}

bool initMenuView(MenuView *mv, MenuWidget *mm, char *str[], Font *n, Font *h)
{
	NOT(mv);
	NOT(mm);
	NOT(str);

	mv->spacing.x = 0;
	mv->spacing.y = n->height * 2;
	mv->menu = mm;
	mv->text = memAlloc(sizeof(HighText) * mm->max);
	mv->len = mkHighTexts(mv->text, n, h, str, mm->max);
	mv->pos.x = (SCREEN_WIDTH - mv->len * (n->width + n->spacing)) / 2;
	mv->pos.y = (SCREEN_HEIGHT - mm->max * n->height * 2) / 2; 
	return areHighTextsLoaded(mv->text, mm->max);
}

void freeMenuView(MenuView *mv)
{
	NOT(mv);
	NOT(mv->menu);

	freeHighTexts(mv->text, mv->menu->max);
	memFree(mv->text);
}

void freeMenuViews(IO *io)
{
	NOT(io);

	freeMenuView(&io->menuMV);
	freeMenuView(&io->playMenuMV);
	freeMenuView(&io->gameMenuMV);
	freeMenuView(&io->settingsMV);
	freeMenuView(&io->yesNoMV);
}

bool initIO(IO *io)
{
	NOT(io);

	return false;
}

bool initMenuViews(IO *io, GUI *g)
{
	Font *n, *h;

	char *menuText[MENU_FOCUS_COUNT] = {"Play", "Settings", "Exit"};
	char *playMenuText[PLAY_MENU_FOCUS_COUNT] = {"1 Player", "2 Player"};
	char *gameMenuText[GAME_MENU_FOCUS_COUNT] = {"Resume", "Settings", "Quit"};
	char *settingsText[SETTINGS_FOCUS_COUNT] = {"Music:", "  SFX:"};
	char *yesNoText[YES_NO_COUNT] = {"Yes", "No"};

	NOT(io);
	NOT(g);

	n = &io->normalFont;
	h = &io->highlightFont;

	if (!initMenuView(&io->menuMV, &g->menu, menuText, n, h)) {
		return false;
	}
	if (!initMenuView(&io->playMenuMV, &g->playMenu, playMenuText, n, h)) {
		return false;
	}
	if (!initMenuView(&io->gameMenuMV, &g->gameMenu, gameMenuText, n, h)) {
		return false;
	}
	if (!initMenuView(&io->settingsMV, &g->settings.menu, settingsText, n, h)) {
		return false;
	}
	if (!initMenuView(&io->yesNoMV, &g->gameAreYouSureQuit, yesNoText, n, h)) {
		return false;
	}
	return true;
}

bool init(Env *e)
{
	int i, j;
	char str[32];
	SDL_Surface *tile[TILE_LOOK_COUNT];

	e->io.time = 0;
	e->quit = false;

	NOT(e);

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return false;
	}
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, AUDIO_CHAN_COUNT, 4096 ) == -1) {
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
	if ((e->io.titleScreen = surfaceAlphaLoad(RES_PATH "title_screen.png")) == NULL) {
		return false;
	}
	if ((e->io.titleBackground = surfaceAlphaLoad(RES_PATH "title_background.png")) == NULL) {
		return false;
	}
	if ((e->io.titleHover = surfaceAlphaLoad(RES_PATH "title_hover.png")) == NULL) {
		return false;
	}
	if ((e->io.back = surfaceAlphaLoad(RES_PATH "back.png")) == NULL) {
		return false;
	}
	if ((e->io.wildUp = surfaceAlphaLoad(RES_PATH "wild_up.png")) == NULL) {
		return false;
	}
	if ((e->io.wildDown = surfaceAlphaLoad(RES_PATH "wild_down.png")) == NULL) {
		return false;
	}
	if ((e->io.lockon = surfaceAlphaLoad(RES_PATH "lockon.png")) == NULL) {
		return false;
	}
	if ((e->io.recall = surfaceAlphaLoad(RES_PATH "recall.png")) == NULL) {
		return false;
	}
	if ((e->io.recallDisable = surfaceAlphaLoad(RES_PATH "recall_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.mode = surfaceAlphaLoad(RES_PATH "mode.png")) == NULL) {
		return false;
	}
	if ((e->io.modeDisable = surfaceAlphaLoad(RES_PATH "mode_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.place = surfaceAlphaLoad(RES_PATH "place.png")) == NULL) {
		return false;
	}
	if ((e->io.placeDisable = surfaceAlphaLoad(RES_PATH "place_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.discard = surfaceAlphaLoad(RES_PATH "discard.png")) == NULL) {
		return false;
	}
	if ((e->io.discardDisable = surfaceAlphaLoad(RES_PATH "discard_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.skip = surfaceAlphaLoad(RES_PATH "skip.png")) == NULL) {
		return false;
	}
	if ((e->io.skipDisable = surfaceAlphaLoad(RES_PATH "skip_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.play = surfaceAlphaLoad(RES_PATH "play.png")) == NULL) {
		return false;
	}
	if ((e->io.playDisable = surfaceAlphaLoad(RES_PATH "play_disable.png")) == NULL) {
		return false;
	}
	if ((e->io.shuffle = surfaceAlphaLoad(RES_PATH "shuffle.png")) == NULL) {
		return false;
	}
	if ((e->io.shuffleDisable = surfaceAlphaLoad(RES_PATH "shuffle_disable.png")) == NULL) {
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
	if (!fontmapInit(&e->io.darkRedFont, 6, 12, RES_PATH "darkred_font.png")) {
		return false;
	}
	
	if (!fontmapInit(&e->io.normalFont, 8, 14, RES_PATH "normal_font.png")) {
		return false;
	}
	e->io.normalFont.spacing = -2;
	if (!fontmapInit(&e->io.highlightFont, 8, 14, RES_PATH "highlight_font.png")) {
		return false;
	}
	e->io.highlightFont.spacing = -2;

	e->io.sq[SQ_NORMAL] = surfaceAlphaLoad(RES_PATH "sq_normal.png");
	e->io.sq[SQ_DBL_LET] = surfaceAlphaLoad(RES_PATH "sq_dl.png");
	e->io.sq[SQ_DBL_WRD] = surfaceAlphaLoad(RES_PATH "sq_dw.png");
	e->io.sq[SQ_TRP_LET] = surfaceAlphaLoad(RES_PATH "sq_tl.png");
	e->io.sq[SQ_TRP_WRD] = surfaceAlphaLoad(RES_PATH "sq_tw.png");
	e->io.sq[SQ_FREE] = surfaceAlphaLoad(RES_PATH "sq_free.png");

	for (i = 0; i < SQ_COUNT; i++) {
		if (e->io.sq[i] == NULL) { 
			return false;
		}
	}

	tile[TILE_LOOK_DISABLE] = surfaceAlphaLoad(RES_PATH "tile_disable.png");
	tile[TILE_LOOK_NORMAL] = surfaceAlphaLoad(RES_PATH "tile_normal.png");
	tile[TILE_LOOK_HOLD] = surfaceAlphaLoad(RES_PATH "tile_hold.png");
	tile[TILE_LOOK_GHOST] = surfaceAlphaLoad(RES_PATH "tile_ghost.png");

	for (i = 0; i < TILE_LOOK_COUNT; i++) {
		e->io.wild[i] = tile[i];
		if (!e->io.wild[i]) {
			return false;
		}
	}

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

	e->io.fader = surfaceCpy(e->io.screen);

	e->io.pressStart = createText(&e->io.normalFont, "PRESS START");

	initGUI(&e->gui);
	if (!initMenuViews(&e->io, &e->gui)) {
		return false;
	}

	{
		char *text[MENU_FOCUS_COUNT] = {"Play", "Settings", "Exit"};
		mkHighTexts(e->io.menuFocus, &e->io.normalFont, &e->io.highlightFont, text, MENU_FOCUS_COUNT);
	}
	if (!areHighTextsLoaded(e->io.menuFocus, MENU_FOCUS_COUNT)) {
		return false;
	}

	{
		char *text[PLAY_MENU_FOCUS_COUNT] = {"Human vs. Human", "Human vs. CPU"};
		mkHighTexts(e->io.playMenuFocus, &e->io.normalFont, &e->io.highlightFont, text, PLAY_MENU_FOCUS_COUNT);
	}
	if (!areHighTextsLoaded(e->io.playMenuFocus, PLAY_MENU_FOCUS_COUNT)) {
		return false;
	}

	{
		char *text[GAME_MENU_FOCUS_COUNT] = {"Resume", "Settings", "Quit"};
		mkHighTexts(e->io.gameMenuFocus, &e->io.normalFont, &e->io.highlightFont, text, GAME_MENU_FOCUS_COUNT);
	}
	if (!areHighTextsLoaded(e->io.gameMenuFocus, GAME_MENU_FOCUS_COUNT)) {
		return false;
	}

	{
		char *text[SETTINGS_FOCUS_COUNT] = {"Music:", "SFX:"};
		mkHighTexts(e->io.settingsFocus, &e->io.normalFont, &e->io.highlightFont, text, SETTINGS_FOCUS_COUNT);
	}
	if (!areHighTextsLoaded(e->io.settingsFocus, SETTINGS_FOCUS_COUNT)) {
		return false;
	}

	e->io.areYouSureQuit = createText(&e->io.normalFont, "Are you sure?");
	{
		char *text[GAME_MENU_FOCUS_COUNT] = {"Yes", "No"};
		mkHighTexts(e->io.yesNo, &e->io.normalFont, &e->io.highlightFont, text, YES_NO_COUNT);
	}
	if (!areHighTextsLoaded(e->io.yesNo, YES_NO_COUNT)) {
		return false;
	}

	if (!(e->io.incorrectSnd = Mix_LoadWAV(RES_PATH "incorrect_snd.wav"))) {
		return false;
	}
	if (!(e->io.correctSnd = Mix_LoadWAV(RES_PATH "correct_snd.wav"))) {
		return false;
	}

	controlsInit(&e->controls);

	return true;
}

void quit(Env *e)
{
	int i, j;

	NOT(e);

	dictQuit(&e->game.dict);
	surfaceFree(e->io.screen);
	surfaceFree(e->io.titleScreen);
	surfaceFree(e->io.titleHover);
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
	freeMenuViews(&e->io);
	freeHighTexts(e->io.menuFocus, MENU_FOCUS_COUNT);
	freeHighTexts(e->io.gameMenuFocus, GAME_MENU_FOCUS_COUNT);
	surfaceFree(e->io.areYouSureQuit);
	freeHighTexts(e->io.yesNo, YES_NO_COUNT);
	fontmapQuit(&e->io.whiteFont);
	fontmapQuit(&e->io.blackFont);
	fontmapQuit(&e->io.yellowFont);
	fontmapQuit(&e->io.darkRedFont);
	Mix_CloseAudio();
	SDL_Quit();
}

void keyStateUpdate(KeyState *ks, bool touched)
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

bool handleEvent(Controls *c)
{
	SDL_Event event;
	Uint8 *ks;

	NOT(c);

	ks = SDL_GetKeyState(NULL);
	NOT(ks);
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: return true;
		case SDL_KEYDOWN: ks[event.key.keysym.sym] = true; break;
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

void printCmd(Cmd *c)
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

void printTransMove(TransMove *tm)
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

void clrMoveModePlace(MoveModePlace *mmp, Board *b) 
{
	int i;
	Coor idx;

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

void clrMoveModeDiscard(MoveModeDiscard *mmd)
{
	int i;

	NOT(mmd);
	
	mmd->num = 0;
	for (i = 0; i < RACK_SIZE; i++) {
		mmd->rack[i] = false;
	}
}

void findNextMoveModePlaceIdx(TransMove *tm)
{
	MoveModePlace *mmp;

	NOT(tm);
	assert(tm->type == TRANS_MOVE_PLACE);

	mmp = &tm->place;

	do {
		mmp->idx++;
		mmp->idx %= RACK_SIZE;
	} while (validBoardIdx(mmp->boardIdx[mmp->idx]) || tm->adjust.data.tile[mmp->idx].type == TILE_NONE);
}

void shuffleRackTransMove(TransMove *tm)
{

	int val[RACK_SIZE], i, j, k;
	bool d;
	TileAdjust tmp;
	Coor coor;

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

bool updateTransMovePlace(TransMove *tm, Cmd *c, Board *b, Player *p)
{
	MoveModePlace *mmp;
	
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
		if (validRackIdx(*idx)) {
			int a0, a1;
			TileAdjust b0, b1;
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
		} else {
			*idx = mmp->idx;
			
			assert(mmp->boardIdx[mmp->idx].x == -1 && mmp->boardIdx[mmp->idx].y == -1);;
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
		Coor bIdx;
		int rIdx;
		
		bIdx = c->data.board;
		rIdx = mmp->rackIdx[bIdx.y][bIdx.x];
	
		if (validRackIdx(rIdx)) {
			assert(mmp->rackIdx[mmp->boardIdx[rIdx].y][mmp->boardIdx[rIdx].x] == rIdx);
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
#ifdef DEBUG
		int original;
		original = mmp->idx;
#endif
		do {
			mmp->idx++;
			mmp->idx %= RACK_SIZE;
#ifdef DEBUG
			assert(mmp->idx != original);
#endif
	
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

bool updateTransMovePlacePlay(TransMove *tm, Cmd *c, Board *b, Player *p)
{
	MoveModePlace *mmp;
	
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

bool updateTransMovePlaceWild(TransMove *tm, Cmd *c, Board *b, Player *p)
{
	MoveModePlace *mmp;

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

bool updateTransMovePlaceEnd(TransMove *tm, Cmd *c, Board *b, Player *p)
{
	MoveModePlace *mmp;

	NOT(tm);
	NOT(c);
	NOT(b);
	assert(tm->type == TRANS_MOVE_PLACE_END);
	assert(c->type != CMD_QUIT);
	
	mmp = &tm->place;
	
	switch (c->type) {
	case CMD_BOARD_SELECT:
	case CMD_BOARD_CANCEL: {
		Coor bIdx;
		int rIdx;
		
		bIdx = c->data.board;
		rIdx = mmp->rackIdx[bIdx.y][bIdx.x];
	
		if (validRackIdx(rIdx)) {
			assert(mmp->boardIdx[rIdx].x == bIdx.x && mmp->boardIdx[rIdx].y == bIdx.y);
			mmp->rackIdx[bIdx.y][bIdx.x] = -1;
			mmp->boardIdx[rIdx].x = -1;
			mmp->boardIdx[rIdx].y = -1;
			mmp->num--;
			tm->type = TRANS_MOVE_PLACE;
			mmp->idx = rIdx;
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

bool updateTransMoveDiscard(TransMove *tm, Cmd *c, Board *b, Player *p)
{
	MoveModeDiscard *mmd;

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

bool updateTransMoveSkip(TransMove *tm, Cmd *c, Board *b, Player *p)
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

bool updateTransMoveQuit(TransMove *tm, Cmd *c, Board *b, Player *p)
{
	NOT(tm);
	NOT(c);
	NOT(b);
	NOT(p);

	p->active = false;
	return true;
}

bool updateTransMove(TransMove *tm, Cmd *c, Board *b, Player *p)
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

void clrTransMove(TransMove *tm, int pidx, Player *p, Board *b)
{
	NOT(tm);

	tm->type = TRANS_MOVE_PLACE;
	tm->playerIdx = pidx;
	mkAdjust(&tm->adjust, p);
	clrMoveModePlace(&tm->place, b);
	clrMoveModeDiscard(&tm->discard);
}

void moveModePlaceToMovePlace(MovePlace *mp, MoveModePlace *mmp, Adjust *a)
{
	int i, ridx, j, k;
	Coor idx;
	
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

void moveModeDiscardToMoveDiscard(MoveDiscard *md, MoveModeDiscard *mmd, Adjust *a)
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

bool transMoveToMove(Move *m, TransMove *tm)
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

void quitGame(GUI *g)
{
	NOT(g);

	g->next = GUI_FOCUS_GAME_OVER;
	g->transMove.type = TRANS_MOVE_QUIT;
}

void updateTitle(GUI *g, Controls *c)
{
	NOT(g);
	NOT(c);

	if (c->start.type == KEY_STATE_PRESSED) {
		g->next = GUI_FOCUS_MENU;
	}
}

bool submitted(Controls *c)
{
	return c->a.type == KEY_STATE_PRESSED || c->start.type == KEY_STATE_PRESSED;
}

bool goBack(Controls *c)
{
	return c->b.type == KEY_STATE_PRESSED;
}

void updateMenuWidget(MenuWidget *m, Controls *c)
{
	NOT(m);
	NOT(c);

	if (m->focus < 0 && m->focus >= m->max) {
		m->focus = m->init;
	}

	if (c->up.type == KEY_STATE_PRESSED) {
		m->focus += m->max;
		m->focus--;
		m->focus %= m->max;
		return;
	}
	if (c->down.type == KEY_STATE_PRESSED) {
		m->focus++;
		m->focus %= m->max;
	}
}

bool updateMenu(GUI *g, Controls *c)
{

	MenuWidget *m;

	NOT(g);
	NOT(c);

	m = &g->menu;

	updateMenuWidget(m, c);



	if (submitted(c)) {
		switch (m->focus) {
		case MENU_FOCUS_PLAY: g->next = GUI_FOCUS_PLAY_MENU; break;
		case MENU_FOCUS_SETTINGS: g->next = GUI_FOCUS_SETTINGS; break;
		case MENU_FOCUS_EXIT: return true;
		default: break;
		}

	}
	if (goBack(c)) {
		g->next = GUI_FOCUS_TITLE;
	}
	return false;
}

int updateVolumes(int curVol, Controls *c)
{
	int newVol;

	NOT(c);

	newVol = curVol;
	
	if (c->left.type == KEY_STATE_PRESSED) {
		newVol --;
	} 
	if (c->right.type == KEY_STATE_PRESSED) {
		newVol ++;
	}

	newVol = newVol < 0 ? 0 : newVol;
	newVol = newVol > MAX_GUI_VOLUME ? MAX_GUI_VOLUME : newVol;

	return newVol;
}

void updateSettings(GUI *g, Controls *c)
{
	Settings *s;

	NOT(g);
	NOT(c);

	s = &g->settings;
	
	updateMenuWidget(&s->menu, c);
	
	if (submitted(c) || goBack(c)) {
		g->next = s->previous;
		return;
	}
	
	switch (s->menu.focus) {
	case SETTINGS_FOCUS_MUSIC: s->musVolume = updateVolumes(s->musVolume, c); break;
	case SETTINGS_FOCUS_SFX: s->sfxVolume = updateVolumes(s->sfxVolume, c); break;
	default: break;
	}


}

void updateGameGUIWidgets(GameGUI *gg, TransMove *tm, Board *b)
{
	updateBoardWidget(&gg->boardWidget, tm, b); 
	updateChoiceWidget(&gg->choiceWidget, tm);
	updateRackWidget(&gg->rackWidget, tm);
}

void resetNewGameGui(GUI *g, Game *gm)
{
	Cmd c;

	NOT(g);
	NOT(gm);

	g->next = GUI_FOCUS_GAME_GUI;
	clrTransMove(&g->transMove, gm->turn, &gm->player[gm->turn], &gm->board);
	c.type = CMD_INVALID;
	updateTransMove(&g->transMove, &c, &gm->board, &gm->player[gm->turn]);
	updateGameGUIWidgets(&g->gameGui, &g->transMove, &gm->board);
}

void updatePlayMenu(GUI *g, Controls *c, Game *gm)
{
	MenuWidget *m;

	NOT(g);
	NOT(c);
	NOT(gm);

	m = &g->playMenu;
	
	updateMenuWidget(m, c);

	if (goBack(c)) {
		g->next = GUI_FOCUS_MENU;
		return;
	}

	if (submitted(c)) {
		switch (m->focus) {
		case PLAY_MENU_FOCUS_HUMAN_VS_HUMAN: {
			initGame1vs1Human(gm);
			resetNewGameGui(g, gm);
			initScoreBoard(&g->scoreBoard, gm);
			break;
		}
		case PLAY_MENU_FOCUS_HUMAN_VS_AI: {
			initGame1vs1HumanAI(gm);
			resetNewGameGui(g, gm);
			initScoreBoard(&g->scoreBoard, gm);
			break;
		}
		default: break;
		}
	}
}

GUIFocusType nextGUIFocusByPlayerType(PlayerType pt)
{
	assert(pt >= 0 && pt < PLAYER_COUNT);

	switch (pt) {
	case PLAYER_HUMAN: return GUI_FOCUS_GAME_HOTSEAT_PAUSE;
	case PLAYER_AI: return GUI_FOCUS_GAME_AI_PAUSE;
	default: break;
	}
	assert(false);
	return GUI_FOCUS_GAME_GUI;
}

void updateScoreBoard(ScoreBoard *sb, Game *gm, float timeStep)
{
	int i;

	NOT(sb);
	NOT(gm);
	assert(sb->playerNum == gm->playerNum);

	sb->turn = gm->turn;
	sb->stable = true;

	for (i = 0; i < sb->playerNum; i++) {
		sb->ctr[i].end = gm->player[i].score;
		if (sb->ctr[i].end == sb->ctr[i].start) {
			sb->ctr[i].stable = true; 
			sb->ctr[i].cur = sb->ctr[i].end;
			continue;
		}
		if (sb->ctr[i].stable) {
			sb->ctr[i].curTime = 0;
			sb->ctr[i].endTime = sb->speed * (sb->ctr[i].end - sb->ctr[i].start);
			sb->ctr[i].stable = false;
		} else {
			sb->ctr[i].curTime += timeStep;
			if (sb->ctr[i].curTime >= sb->ctr[i].endTime) {
				sb->ctr[i].cur = sb->ctr[i].end;
				sb->ctr[i].start = sb->ctr[i].end;
				continue;
			}
		}
		sb->stable = false;
		sb->ctr[i].cur = sb->ctr[i].start;
		sb->ctr[i].cur += (sb->ctr[i].curTime / sb->ctr[i].endTime) * (sb->ctr[i].end - sb->ctr[i].start);
	}
}

void updateGameGUI(GUI *g, Controls *c, Game *gm)
{
	Cmd cmd;
	Move m;
	Action a;
	Log l;
	GameGUI *gg;
	TransMove *tm;
	
	NOT(g);
	NOT(c);
	NOT(gm);

	gg = &g->gameGui;
	tm = &g->transMove;

	if (c->start.type == KEY_STATE_PRESSED) {
		g->next = GUI_FOCUS_GAME_MENU;
		return;
	}
	
	if (tm->type == TRANS_MOVE_INVALID) {
		resetNewGameGui(g, gm);
	}
	
	switch (gg->focus) {
	case GAME_GUI_FOCUS_BOARD: boardWidgetControls(&cmd, gg, c); break;
	case GAME_GUI_FOCUS_CHOICE: choiceWidgetControls(&cmd, gg, c); break;
	case GAME_GUI_FOCUS_RACK: rackWidgetControls(&cmd, gg, c); break;
	default: break;
	}
	updateGameGUIViaCmd(gg, &cmd, tm->type);
	
	switch (cmd.type) {
	case CMD_FOCUS_TOP: {
		if (tm->type == TRANS_MOVE_PLACE ||
		    tm->type == TRANS_MOVE_PLACE_WILD ||
		    tm->type == TRANS_MOVE_PLACE_END ||
		    tm->type == TRANS_MOVE_PLACE_PLAY) {
			gg->focus = GAME_GUI_FOCUS_BOARD;
		}
		break;
	}
	case CMD_FOCUS_BOTTOM: {
		gg->focus = gg->bottomLast != GAME_GUI_FOCUS_CHOICE 
			? GAME_GUI_FOCUS_RACK
			: GAME_GUI_FOCUS_CHOICE;
		break;
	}
	case CMD_BOARD: {
		gg->focus = GAME_GUI_FOCUS_BOARD;
		gg->boardWidget.index = cmd.data.board;
		break;
	}
	case CMD_RACK: {
		if (tm->type == TRANS_MOVE_SKIP) {
			break;
		}
		gg->focus = GAME_GUI_FOCUS_RACK;
		gg->rackWidget.index.x = cmd.data.rack;
		gg->rackWidget.index.y = 0;
		break;
	}
	case CMD_CHOICE: {
		gg->focus = GAME_GUI_FOCUS_CHOICE;
		gg->choiceWidget.index.x = cmd.data.choice;
		gg->choiceWidget.index.y = 0;
		break;
	}
	default: break;
	}

	if (gg->focus != GAME_GUI_FOCUS_BOARD) {
		gg->bottomLast = gg->focus;
	}

	/* printCmd(&c); */

	if (updateTransMove(tm, &cmd, &gm->board, &gm->player[gm->turn])) {
		/* printTransMove(&e->transMove); */
		updateBoardWidget(&gg->boardWidget, tm, &gm->board); 
		updateChoiceWidget(&gg->choiceWidget, tm);
		updateRackWidget(&gg->rackWidget, tm);
	}

	transMoveToMove(&m, tm);

	mkAction(&a, gm, &m);
	applyAction(gm, &a);

	if (cmd.type == CMD_PLAY || cmd.type == CMD_QUIT) {
		mkLog(&a, &l);
		/* printLog(&l); */
	}

	if (a.type != ACTION_INVALID) {
		applyAdjust(&gm->player[a.playerIdx], &tm->adjust);
		if (endGame(gm)) {
			g->next = GUI_FOCUS_GAME_OVER;
		} else {
			nextTurn(gm);
			clrTransMove(tm, gm->turn, &gm->player[gm->turn], &gm->board);
			g->next = nextGUIFocusByPlayerType(gm->player[gm->turn].type);
		}
		tm->type = TRANS_MOVE_INVALID;
	} else {
		if (m.type != MOVE_INVALID) {
			/* printActionErr(a.type); */
		}
	} 

	if (cmd.type == CMD_PLAY) {
		gg->validPlay = a.type != ACTION_INVALID ? YES : NO;
	} else {
		gg->validPlay = YES_NO_INVALID;
	}

	updateScoreBoard(&g->scoreBoard, gm, SPF);
}

void updateGameMenu(GUI *g, Controls *c)
{
	MenuWidget *m;

	NOT(g);
	NOT(c);

	m = &g->gameMenu;

	updateMenuWidget(m, c);

	if (submitted(c)) {
		switch (m->focus) {
		case GAME_MENU_FOCUS_RESUME: {
			g->next = GUI_FOCUS_GAME_GUI;
			break;
		}
		case GAME_MENU_FOCUS_SETTINGS: {
			g->settings.previous = g->focus;
			g->next = GUI_FOCUS_SETTINGS;
			break;
		}
		case GAME_MENU_FOCUS_QUIT: {
			g->next = GUI_FOCUS_GAME_ARE_YOU_SURE_QUIT;
			break;
		}
		default: break;
		}
		return;
	}
}

void updateGameHotseatPause(GUI *g, Controls *c, Game *gm)
{
	NOT(g);
	NOT(c);

	if (submitted(c)) {
		g->next = GUI_FOCUS_GAME_GUI;
	}
	updateScoreBoard(&g->scoreBoard, gm, SPF);
}

void updateGameAIPause(GUI *g, Controls *c, Game *gm)
{
	Move m;
	Action a;

	NOT(g);
	NOT(c);
	NOT(gm);
	
	aiFindMove(&m, gm->turn, gm, NULL);
	mkAction(&a, gm, &m);
	applyAction(gm, &a);
	nextTurn(gm);
	g->next = nextGUIFocusByPlayerType(gm->player[gm->turn].type);

	updateScoreBoard(&g->scoreBoard, gm, SPF);
}

void updateGameAreYouSureQuit(GUI *g, Controls *c)
{
	NOT(g);
	NOT(c);

	updateMenuWidget(&g->gameAreYouSureQuit, c);
	
	if (submitted(c)) {
		g->next = g->gameAreYouSureQuit.focus == YES
				? GUI_FOCUS_GAME_OVER
				: GUI_FOCUS_GAME_MENU;
		return;
	}
	if (goBack(c)) {
		g->next = GUI_FOCUS_GAME_MENU;
	}
}

void updateGameOver(GUI *g, Controls *c, Game *gm)
{
	NOT(g);
	NOT(c);
		
	if (c->start.type == KEY_STATE_PRESSED) {
		g->next = GUI_FOCUS_TITLE;
	}
	updateScoreBoard(&g->scoreBoard, gm, SPF);
}

void update(Env *e)
{
	NOT(e);

	e->gui.focus = e->gui.next;
	if (!e->gui.scoreBoard.stable) {
		updateScoreBoard(&e->gui.scoreBoard, &e->game, SPF);
		return;
	}
	switch (e->gui.focus) {
	case GUI_FOCUS_TITLE: updateTitle(&e->gui, &e->controls); break;
	case GUI_FOCUS_MENU: e->quit = updateMenu(&e->gui, &e->controls); break;
	case GUI_FOCUS_SETTINGS: updateSettings(&e->gui, &e->controls); break;
	case GUI_FOCUS_PLAY_MENU: updatePlayMenu(&e->gui, &e->controls, &e->game); break;
	case GUI_FOCUS_GAME_GUI: updateGameGUI(&e->gui, &e->controls, &e->game); break;
	case GUI_FOCUS_GAME_MENU: updateGameMenu(&e->gui, &e->controls); break;
	case GUI_FOCUS_GAME_HOTSEAT_PAUSE: updateGameHotseatPause(&e->gui, &e->controls, &e->game); break;
	case GUI_FOCUS_GAME_AI_PAUSE: updateGameAIPause(&e->gui, &e->controls, &e->game); break;
	case GUI_FOCUS_GAME_OVER: updateGameOver(&e->gui, &e->controls, &e->game); break;
	case GUI_FOCUS_GAME_ARE_YOU_SURE_QUIT: updateGameAreYouSureQuit(&e->gui, &e->controls); break;
	default: break;
	}
	e->io.time += 1.0f / ((float)(FPS));
}

void guiDrawLockon(IO *io, GameGUI *gg)
{
	const int w = 14, h = 14;
	Coor idx;

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

void guiDrawGhostTile(IO *io, GameGUIFocusType gf, TransMove *tm, Player *p, GridWidget *bw)
{
	int i;
	Coor idx;
	SDL_Surface *s;
	Tile *t;

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

void guiDrawBoard(IO *io, GridWidget *bw, Game *g, TransMove *tm)
{
	Coor pos, dim;
	
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

void guiDrawRack(IO *io, GridWidget *rw, Game *g, TransMove *tm)
{
	Coor pos, dim;
	
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

void guiDraw(IO *io, GUI *g, Game *gm, TransMove *tm)
{
	Coor pos, dim;
	
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

int scrollOffset(int dis, int pps, float time)
{
	int secsToLoop;
	secsToLoop = dis / pps;
	return (int)(dis * time / secsToLoop) % dis;
}

void drawScrollingBackground(Env *e)
{
	int off0, off1;
	
	NOT(e);

	off0 = scrollOffset(e->io.titleBackground->h, 15, e->io.time);
	off1 = scrollOffset(e->io.titleHover->h, -30, e->io.time);

	surfaceDraw(e->io.screen, e->io.titleBackground, 0, off0);
	surfaceDraw(e->io.screen, e->io.titleBackground, 0, off0 - e->io.titleBackground->h);
	surfaceDraw(e->io.screen, e->io.titleHover, 0, off1);
	surfaceDraw(e->io.screen, e->io.titleHover, 0, off1 + e->io.titleHover->h);
}


void drawGameAreYouSureQuit(Env *e)
{
	int i;
	const int orgX = 150 - 8;
	const int orgY = 80;

	NOT(e);

	drawScrollingBackground(e);
	
	surfaceDraw(e->io.screen, e->io.back, 0, 0);
	guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove); 
	SDL_FillRect(e->io.fader, 0, SDL_MapRGB(e->io.fader->format, 0, 0, 0));
	SDL_SetAlpha(e->io.fader, SDL_SRCALPHA, 218);
	surfaceDraw(e->io.screen, e->io.fader, 0, 0);
	surfaceDraw(e->io.screen, e->io.areYouSureQuit, orgX - e->io.whiteFont.width * 11, orgY - e->io.whiteFont.height);
	for (i = 0; i < YES_NO_COUNT; i++) {
		surfaceDraw(e->io.screen,
				e->gui.gameAreYouSureQuit.focus == i ? e->io.yesNo[i].highlight : e->io.yesNo[i].normal,
				orgX, orgY + i * e->io.whiteFont.height);
	}
}

void drawNum(SDL_Surface *s, int x, int y, int num, Font *f)
{
	SDL_Rect clip, offset;
	int c, n;

	NOT(s);
	NOT(f);
	NOT(f->map);

	n = num > -num ? num : -num;
	offset.x = x;
	offset.y = y;
	clip.y = 0;
	clip.h = f->height;
	clip.w = f->width;

	if (num == 0) {
		c = '0' - 32;
		clip.x = c * f->width;
		SDL_BlitSurface(f->map, &clip, s, &offset);
		return;
	}

	do {	
		c = '0' + (n % 10) - 32;
		clip.x = c * f->width;
		SDL_BlitSurface(f->map, &clip, s, &offset);
		offset.x -= f->width;
		n /= 10;
	} while (n != 0);

	if (num < 0) {
		c = '-' - 32;
		clip.x = c * f->width;
		SDL_BlitSurface(f->map, &clip, s, &offset);
	} 
}

void drawScoreBoard(ScoreBoard *sb, IO *io)
{
	int i;
	Font *f;

	NOT(sb);
	NOT(io);
	
	for (i = 0; i < sb->playerNum; i++) {
		f = i == sb->turn || sb->ctr[i].cur < sb->ctr[i].end 
				? &io->highlightFont 
				: &io->normalFont;
		drawNum(io->screen, 40, (io->highlightFont.height + 2) * i + 30, sb->ctr[i].cur, f);
	}
}

void drawMenuView(SDL_Surface *s, MenuView *mv)
{
	SDL_Surface *t;
	int i, x, y;

	NOT(s);
	NOT(mv);

	for (i = 0; i < mv->menu->max; i++) {
		t = mv->menu->focus == i ? mv->text[i].highlight : mv->text[i].normal;
		NOT(t);
		x = mv->pos.x + mv->text[i].offset;
		y = mv->pos.y + mv->spacing.y * i;
		surfaceDraw(s, t, x, y);
	}
}

void audi(Env *e)
{
	NOT(e);
	
	switch (e->gui.focus) {
	case GUI_FOCUS_GAME_GUI: {
		switch (e->gui.gameGui.validPlay) {
		case YES: Mix_PlayChannel(AUDIO_CHAN_SFX, e->io.correctSnd, 0); break;
		case NO: Mix_PlayChannel(AUDIO_CHAN_SFX, e->io.incorrectSnd, 0); break;
		default: break;
		}
		break;
	}
	case GUI_FOCUS_SETTINGS: {
		Mix_Volume(AUDIO_CHAN_MUSIC, e->gui.settings.musVolume * MIX_MAX_VOLUME / MAX_GUI_VOLUME);
		Mix_Volume(AUDIO_CHAN_SFX, e->gui.settings.sfxVolume * MIX_MAX_VOLUME / MAX_GUI_VOLUME);
		break;
	}
	default: break;
	}
}

void draw(Env *e)
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
		drawMenuView(e->io.screen, &e->io.menuMV);
		break;
	}
	case GUI_FOCUS_SETTINGS: {
		Font *f;
		int i;

		drawScrollingBackground(e);

		drawMenuView(e->io.screen, &e->io.settingsMV);
		for (i = 0; i < SETTINGS_FOCUS_COUNT; i++) {
			f = i == e->gui.settings.menu.focus ? &e->io.highlightFont : &e->io.normalFont;
			if (i == SETTINGS_FOCUS_MUSIC || i == SETTINGS_FOCUS_SFX) {
				int v;
				v = i == SETTINGS_FOCUS_MUSIC ? e->gui.settings.musVolume : e->gui.settings.sfxVolume;
				drawNum(e->io.screen, 190, i * e->io.normalFont.height * 2 + 100, v, f);
			}
		}
		surfaceDraw(e->io.screen, e->io.pressStart, (320 - e->io.pressStart->w) / 2, 200);
		break;
	}
	case GUI_FOCUS_GAME_GUI: {

		drawScrollingBackground(e);
		surfaceDraw(e->io.screen, e->io.back, 0, 0);
		guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove); 
		drawScoreBoard(&e->gui.scoreBoard, &e->io);
		break;
	}
	case GUI_FOCUS_GAME_MENU: {
		drawScrollingBackground(e);
		surfaceDraw(e->io.screen, e->io.back, 0, 0);
		guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove); 
		SDL_FillRect(e->io.fader, 0, SDL_MapRGB(e->io.fader->format, 0, 0, 0));
		SDL_SetAlpha(e->io.fader, SDL_SRCALPHA, 196);
		surfaceDraw(e->io.screen, e->io.fader, 0, 0);
		
		drawMenuView(e->io.screen, &e->io.gameMenuMV);
		break;
	}
	case GUI_FOCUS_PLAY_MENU: {
		drawScrollingBackground(e);
		drawMenuView(e->io.screen, &e->io.playMenuMV);
		break;
	}
	case GUI_FOCUS_GAME_AI_PAUSE: {
		drawScrollingBackground(e);
		surfaceDraw(e->io.screen, e->io.back, 0, 0);
		guiDrawBoard(&e->io, &e->gui.gameGui.boardWidget, &e->game, &e->gui.transMove);
		drawScoreBoard(&e->gui.scoreBoard, &e->io);
		SDL_FillRect(e->io.fader, 0, SDL_MapRGB(e->io.fader->format, 0, 0, 0));
		SDL_SetAlpha(e->io.fader, SDL_SRCALPHA, 196);
		surfaceDraw(e->io.screen, e->io.fader, 0, 0);
		break;
	}
	case GUI_FOCUS_GAME_HOTSEAT_PAUSE: {
		drawScrollingBackground(e);
		surfaceDraw(e->io.screen, e->io.back, 0, 0);
		guiDrawBoard(&e->io, &e->gui.gameGui.boardWidget, &e->game, &e->gui.transMove);
		drawScoreBoard(&e->gui.scoreBoard, &e->io);
		SDL_FillRect(e->io.fader, 0, SDL_MapRGB(e->io.fader->format, 0, 0, 0));
		SDL_SetAlpha(e->io.fader, SDL_SRCALPHA, 196);
		surfaceDraw(e->io.screen, e->io.fader, 0, 0);
		surfaceDraw(e->io.screen, e->io.pressStart, (320 - e->io.pressStart->w) / 2, 200);
		break;
	}
	case GUI_FOCUS_GAME_OVER: {
		drawScrollingBackground(e);
		surfaceDraw(e->io.screen, e->io.back, 0, 0);
		guiDrawBoard(&e->io, &e->gui.gameGui.boardWidget, &e->game, &e->gui.transMove);
		drawScoreBoard(&e->gui.scoreBoard, &e->io);
		break;
	}
	case GUI_FOCUS_GAME_ARE_YOU_SURE_QUIT: drawGameAreYouSureQuit(e); break;
	default: break;
	}
	SDL_Flip(e->io.screen);
}

void exec(Env *e)
{
	int st;
	bool q;

	NOT(e);

	q = false;
	e->game.turn = 0; 

	do {
		st = SDL_GetTicks();
		q = handleEvent(&e->controls);
		update(e);
		draw(e);
		audi(e);
		delay(st, SDL_GetTicks(), FPS);
	} while (!e->quit && !q);
}

int gui()
{
	int exitStatus = EXIT_FAILURE;
	Env e;
	if (init(&e)) {
		exec(&e);
		exitStatus = EXIT_SUCCESS;
	}
	quit(&e);
	return exitStatus;
}

