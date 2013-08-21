#include <math.h>
#include <limits.h>

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

void initKeyState(KeyState *ks)
{
	NOT(ks);
	
	ks->type = keyStateUntouched;
	ks->time = 0.0f;
}

void initGameControls(GameControls *gc)
{
	NOT(gc);
	
	gc->key[gameKeyPlay] = hardwareKeySelect;
	gc->key[gameKeyRecall] = hardwareKeyY;
	gc->key[gameKeyShuffle] = -1;
	gc->key[gameKeyCancel] = hardwareKeyX;
	gc->key[gameKeyMode] = hardwareKeyB;
	gc->key[gameKeySelect] = hardwareKeyA;
	gc->key[gameKeyUp] = hardwareKeyUp;
	gc->key[gameKeyDown] = hardwareKeyDown;
	gc->key[gameKeyLeft] = hardwareKeyLeft;
	gc->key[gameKeyRight] = hardwareKeyRight;
	gc->key[gameKeyPrevTile] = hardwareKeyL;
	gc->key[gameKeyNextTile] = hardwareKeyR;
}

void controlsInit(Controls *c)
{
	int i;
	HardwareControls *hc;

	NOT(c);

	hc = &c->hardware;

	for (i = 0; i < hardwareKeyCount; i++) {
		initKeyState(&hc->key[i]);
	}

	hc->axisX = 0.f;
	hc->axisY = 0.f;

	initGameControls(&c->game);
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
	s->previous = guiFocusMenu; 
	initMenuWidget(&s->menu, settingsFocusMusic, settingsFocusCount);
}

void initGameGUI(GameGUI *gg)
{
	NOT(gg);
	
	mkRackWidget(&gg->rackWidget);
	mkChoiceWidget(&gg->choiceWidget);
	mkBoardWidget(&gg->boardWidget);
	gg->focus = gameGUIFocusBoard;
	gg->bottomLast = gameGUIFocusChoice;
	gg->choiceWidget.index.x = 1;
}

void initGUI(GUI *g)
{
	initMenuWidget(&g->menu, menuFocusPlay, menuFocusCount);
	initMenuWidget(&g->gameMenu, gameMenuFocusResume, gameMenuFocusCount);
	initMenuWidget(&g->playMenu, playMenuFocusHumanVsAI, playMenuFocusCount);
	initSettings(&g->settings);
	initGameGUI(&g->gameGui);
	initMenuWidget(&g->gameAreYouSureQuit, yes, yesNoCount);
	g->focus = guiFocusTitle;
	g->next = guiFocusTitle;
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

	char *menuText[menuFocusCount] = {"Play", "Rules", "Settings", "Exit"};
	char *playMenuText[playMenuFocusCount] = {"1 Player", "2 Player", "Netplay (N/A)"};
	char *gameMenuText[gameMenuFocusCount] = {"Resume", "Settings", "Skip", "Quit"};
	char *settingsText[settingsFocusCount] = {"Music:     ", "  SFX:     ", "Controls"};
	char *yesNoText[yesNoCount] = {"Yes", "No"};

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
	SDL_Surface *tile[tileLookCount];

	e->io.time = 0;
	e->quit = false;

	NOT(e);

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return false;
	}
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, audioChanCount, 4096 ) == -1) {
		return false;
	}
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("finite", NULL);



	e->io.joystick = NULL;
	if (SDL_NumJoysticks() < 1) {
		return false;
	}
	SDL_JoystickEventState(SDL_ENABLE);
	e->io.joystick = SDL_JoystickOpen(0);
	if (SDL_JoystickNumAxes(e->io.joystick) < 1) {
		return false;
	}


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
	if ((e->io.menuBg = surfaceAlphaLoad(RES_PATH "menu_bg.png")) == NULL) {
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

	e->io.sq[sqNormal] = surfaceAlphaLoad(RES_PATH "sq_normal.png");
	e->io.sq[sqDblLet] = surfaceAlphaLoad(RES_PATH "sq_dl.png");
	e->io.sq[sqDblWrd] = surfaceAlphaLoad(RES_PATH "sq_dw.png");
	e->io.sq[sqTrpLet] = surfaceAlphaLoad(RES_PATH "sq_tl.png");
	e->io.sq[sqTrpWrd] = surfaceAlphaLoad(RES_PATH "sq_tw.png");
	e->io.sq[sqFree] = surfaceAlphaLoad(RES_PATH "sq_free.png");
	e->io.sq[sqMystery] = surfaceAlphaLoad(RES_PATH "sq_mystery.png");

	for (i = 0; i < sqCount; i++) {
		if (e->io.sq[i] == NULL) { 
			return false;
		}
	}

	tile[tileLookDisable] = surfaceAlphaLoad(RES_PATH "tile_disable.png");
	tile[tileLookNormal] = surfaceAlphaLoad(RES_PATH "tile_normal.png");
	tile[tileLookHold] = surfaceAlphaLoad(RES_PATH "tile_hold.png");
	tile[tileLookGhost] = surfaceAlphaLoad(RES_PATH "tile_ghost.png");

	for (i = 0; i < tileLookCount; i++) {
		e->io.wild[i] = tile[i];
		if (!e->io.wild[i]) {
			return false;
		}
	}

	for (i = 0; i < letterCount; i++) {
		for (j = 0; j < tileLookCount; j++) {
			e->io.tile[tileWild][i][j] = surfaceCpy(tile[j]);
			if (!e->io.tile[tileWild][i][j]) {
				return false;
			}
			e->io.tile[tileLetter][i][j] = surfaceCpy(tile[j]);
			if (!e->io.tile[tileLetter][i][j]) {
				return false;
			}
			sprintf(str,"%c", i + 'a');
			strDraw(e->io.tile[tileWild][i][j], &e->io.blackFont, str, 3, 0);
			sprintf(str,"%c", i + 'A');
			strDraw(e->io.tile[tileLetter][i][j], &e->io.blackFont, str, 3, 0);
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
		char *text[gameMenuFocusCount] = {"Yes", "No"};
		mkHighTexts(e->io.yesNo, &e->io.normalFont, &e->io.highlightFont, text, yesNoCount);
	}
	if (!areHighTextsLoaded(e->io.yesNo, yesNoCount)) {
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

	if (e->io.joystick) {
		SDL_JoystickClose(e->io.joystick);
	}

	surfaceFree(e->io.screen);
	surfaceFree(e->io.titleScreen);
	surfaceFree(e->io.titleHover);
	surfaceFree(e->io.menuBg);
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
	for (i = 0; i < sqCount; i++) {
		surfaceFree(e->io.sq[i]);
	}
	for (i = 0; i < tileLookCount; i++) {
		surfaceFree(e->io.wild[i]);
	}	
	for (i = 0; i < letterCount; i++) {
		for (j = 0; j < tileLookCount; j++) {
			surfaceFree(e->io.tile[tileWild][i][j]);
			surfaceFree(e->io.tile[tileLetter][i][j]);
		}
	}
	freeMenuViews(&e->io);
	freeHighTexts(e->io.menuFocus, menuFocusCount);
	freeHighTexts(e->io.gameMenuFocus, gameMenuFocusCount);
	surfaceFree(e->io.areYouSureQuit);
	freeHighTexts(e->io.yesNo, yesNoCount);
	fontmapQuit(&e->io.whiteFont);
	fontmapQuit(&e->io.blackFont);
	fontmapQuit(&e->io.yellowFont);
	fontmapQuit(&e->io.darkRedFont);
	Mix_CloseAudio();
	SDL_Quit();
}


float normalizeAxis(int v)
{
	return (float)v / (float)(v > 0 ? INT_MAX : INT_MIN);
}


bool handleEvent(Controls *c)
{
	SDL_Event event;
	Uint8 *ks;
	HardwareControls *hc;

	NOT(c);

	hc = &c->hardware;
	ks = SDL_GetKeyState(NULL);
	NOT(ks);
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: return true;
		case SDL_KEYDOWN: ks[event.key.keysym.sym] = true; break;
		case SDL_KEYUP: ks[event.key.keysym.sym] = false; break;
		case SDL_JOYAXISMOTION: {
			if (event.jaxis.axis == 0) {
				hc->axisX = normalizeAxis(event.jaxis.value);
			}
			if (event.jaxis.axis == 1) {
				hc->axisY = -normalizeAxis(event.jaxis.value);
			}
		}
		default: break;
		}
	}

	keyStateUpdate(&hc->key[hardwareKeyStart], ks[SDLK_RETURN]);
	keyStateUpdate(&hc->key[hardwareKeySelect], ks[SDLK_ESCAPE]);
	keyStateUpdate(&hc->key[hardwareKeyUp], ks[SDLK_UP]);
	keyStateUpdate(&hc->key[hardwareKeyDown], ks[SDLK_DOWN]);
	keyStateUpdate(&hc->key[hardwareKeyLeft], ks[SDLK_LEFT]);
	keyStateUpdate(&hc->key[hardwareKeyRight], ks[SDLK_RIGHT]);
	keyStateUpdate(&hc->key[hardwareKeyA], ks[SDLK_LCTRL]);
	keyStateUpdate(&hc->key[hardwareKeyB], ks[SDLK_LALT]);
	keyStateUpdate(&hc->key[hardwareKeyX], ks[SDLK_LSHIFT]);
	keyStateUpdate(&hc->key[hardwareKeyY], ks[SDLK_SPACE]);
	keyStateUpdate(&hc->key[hardwareKeyL], ks[SDLK_TAB]);
	keyStateUpdate(&hc->key[hardwareKeyR], ks[SDLK_BACKSPACE]);
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

