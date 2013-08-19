#include <math.h>

#include "gui.h"
#include "update.h"
#include "draw.h"
#include "audi.h"
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

	char *menuText[MENU_FOCUS_COUNT] = {"Play", "Rules", "Settings", "Exit"};
	char *playMenuText[PLAY_MENU_FOCUS_COUNT] = {"1 Player", "2 Player", "Netplay (N/A)"};
	char *gameMenuText[GAME_MENU_FOCUS_COUNT] = {"Resume", "Settings", "Skip", "Quit"};
	char *settingsText[SETTINGS_FOCUS_COUNT] = {"Music:     ", "  SFX:     ", "Controls", "Back"};
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
	SDL_FillRect(e->io.fader, 0, SDL_MapRGBA(e->io.fader->format, 0, 0, 0, 0));

	e->io.pressStart = createText(&e->io.normalFont, "PRESS START");

	initGUI(&e->gui);
	if (!initMenuViews(&e->io, &e->gui)) {
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

