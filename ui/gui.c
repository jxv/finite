#include <math.h>
#include <limits.h>
#include <pthread.h>

#include "gui.h"
#include "update.h"
#include "draw.h"
#include "audi.h"
#include "init.h"
#include "widget.h"
#include "print.h"

bool initMenuViews(IO *io, GUI *g);

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

void initAxisState(AxisState *as)
{
	const float deadZone = 0.33f;

	NOT(as);
	
	as->type = axisStateInDeadZone;
	as->deadZone = deadZone;
	as->time = 0.f;
	as->value = 0.f;
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

	initAxisState(&hc->axisX);
	initAxisState(&hc->axisY);
	initAxisState(&hc->accelX);
	initAxisState(&hc->accelY);

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
	int i;

	NOT(s);

	for (i = 0; i < volCount; i++) {
		s->vol[i] = MAX_GUI_VOLUME;
		
	}

	s->previous = guiFocusMenu; 
	initMenuWidget(&s->menu, settingsFocusMusic, settingsFocusCount);
}

void initTextLog(TextLog *tl)
{
	NOT(tl);

	tl->head = 0;
	tl->maxLen = 15;
	tl->maxSize = 10;
	tl->size = 0;
	tl->head = 0;
}

void initGameGUI(GameGUI *gg)
{
	NOT(gg);
	
	mkRackWidget(&gg->rackWidget);
	mkBoardWidget(&gg->boardWidget);
	gg->focus = gameGUIFocusBoard;
	gg->bottomLast = gameGUIFocusRack;
	initTextLog(&gg->textLog);
	gg->lastMove.type = lastMoveNone;
}

void actionToLastMove(LastMove *lm, Action *a)
{
		Coor idx;
		Path *p;

		NOT(lm);
		NOT(a);
		
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				lm->data.place[idx.y][idx.x] = false;
			}
		}

		if (a->type != actionPlace) {
			return;
		}

		p = &a->data.place.path;

		switch (p->type) {
		case pathDot: {
			if (p->data.dot.right.type == dirRight) {
				lm->data.place[p->data.dot.right.y][p->data.dot.right.x] = true;
			}
			if (p->data.dot.down.type == dirDown) {
				lm->data.place[p->data.dot.down.y][p->data.dot.down.x] = true;
			}
			break;
		}
		case pathHorz: {
			if (p->data.horz.right.type != dirRight) {
				break;
			}
			idx.y = p->data.horz.right.y;
			for (idx.x = p->data.horz.right.x; idx.x < p->data.horz.right.x + p->data.horz.right.len; idx.x++) {
				lm->data.place[idx.y][idx.x] = p->data.horz.right.pos[idx.x];
			}
				
			break;
		}
		case pathVert: {
			if (p->data.vert.down.type == dirDown) {
				break;
			}
			idx.x = p->data.vert.down.x;
			for (idx.y = p->data.vert.down.y; idx.y < p->data.vert.down.y + p->data.vert.down.len; idx.y++) {
				lm->data.place[idx.y][idx.x] = p->data.vert.down.pos[idx.y];
			}
				
			break;
		}
		default: break;
		}
}

void initGUI(GUI *g)
{
	initMenuWidget(&g->menu, menuFocusPlay, menuFocusCount);
	initMenuWidget(&g->gameMenu, gameMenuFocusResume, gameMenuFocusCount);
	initMenuWidget(&g->playMenu, playMenuFocusHumanVsAI, playMenuFocusCount);
	initSettings(&g->settings);
	initGameGUI(&g->gameGui);
	initMenuWidget(&g->gameAreYouSureQuit, yes, yesNoCount);
	initMenuWidget(&g->controlsMenu.menu, gameKeyPlay,gameKeyCount);


	g->focus = guiFocusTitle;
	g->next = guiFocusTitle;

	clrDups(&g->controlsMenu);
	g->controlsMenu.time = 0.f;

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

void recenterMenuView(MenuView *mv, Font *n)
{
	NOT(mv);

	mv->pos.x = (SCREEN_WIDTH - mv->len * (n->width + n->spacing)) / 2;
	mv->pos.y = (SCREEN_HEIGHT - mv->menu->max * mv->spacing.y) / 2;
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
	recenterMenuView(mv, n);
	return areHighTextsLoaded(mv->text, mm->max);
}

void freeMenuView(MenuView *mv)
{
	NOT(mv);

	if (mv->menu) {
		freeHighTexts(mv->text, mv->menu->max);
	}
	if (mv->text) {
		memFree(mv->text);
	}
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

bool initIO(Env *e)
{
	int count;
	float COUNT;
	int i, j;
	char str[32];
	SDL_Surface *tile[tileLookCount];

	NOT(e);
	
	count = 0;
	COUNT = 328 + 8 * 50.f;
	
	SDL_JoystickEventState(SDL_ENABLE);
	e->io.joystick = SDL_JoystickOpen(0);
	count++; e->io.loading += 1.f / COUNT;

	if (e->io.joystick) {
		e->io.joyExists = true;
		if (SDL_JoystickNumAxes(e->io.joystick) < 1) {
			return false;
		}
	} else {
		e->io.joyExists = false;
	}
	count++; e->io.loading += 1.f / COUNT;


	
	SDL_JoystickEventState(SDL_ENABLE);
	e->io.accel = SDL_JoystickOpen(1);
	count++; e->io.loading += 1.f / COUNT;

	if (e->io.accel) {
		e->io.accelExists = true;
		if (SDL_JoystickNumAxes(e->io.accel) < 1) {
			return false;
		}
	} else {
		e->io.accelExists = false;
	}

	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.menuBg = surfaceAlphaLoad(RES_PATH "menu_bg.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.scoreBoard = surfaceAlphaLoad(RES_PATH "scoreboard.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.textLog = surfaceAlphaLoad(RES_PATH "text_log.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.gmBack = surfaceAlphaLoad(RES_PATH "game_bg.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.back = surfaceAlphaLoad(RES_PATH "back.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.wildUp = surfaceAlphaLoad(RES_PATH "wild_up.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.wildDown = surfaceAlphaLoad(RES_PATH "wild_down.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.lockon = surfaceAlphaLoad(RES_PATH "lockon.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.recall = surfaceAlphaLoad(RES_PATH "recall.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.recallDisable = surfaceAlphaLoad(RES_PATH "recall_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.mode = surfaceAlphaLoad(RES_PATH "mode.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.modeDisable = surfaceAlphaLoad(RES_PATH "mode_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.place = surfaceAlphaLoad(RES_PATH "place.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.placeDisable = surfaceAlphaLoad(RES_PATH "place_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.discard = surfaceAlphaLoad(RES_PATH "discard.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.discardDisable = surfaceAlphaLoad(RES_PATH "discard_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.skip = surfaceAlphaLoad(RES_PATH "skip.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.skipDisable = surfaceAlphaLoad(RES_PATH "skip_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.play = surfaceAlphaLoad(RES_PATH "play.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.playDisable = surfaceAlphaLoad(RES_PATH "play_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.shuffle = surfaceAlphaLoad(RES_PATH "shuffle.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.shuffleDisable = surfaceAlphaLoad(RES_PATH "shuffle_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;

	if (!(e->io.boardCover = surfaceAlphaLoad(RES_PATH "board_cover.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	
	if (!(e->io.pauseTitle = surfaceAlphaLoad(RES_PATH "pause.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.settingsTitle = surfaceAlphaLoad(RES_PATH "settings.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.finiteTitle = surfaceAlphaLoad(RES_PATH "finite_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.controlsTitle = surfaceAlphaLoad(RES_PATH "controls_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.areYouSureTitle = surfaceAlphaLoad(RES_PATH "are_you_sure_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.chooseGameTitle = surfaceAlphaLoad(RES_PATH "choose_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.rulesTitle = surfaceAlphaLoad(RES_PATH "rules_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.optionsTitle = surfaceAlphaLoad(RES_PATH "options_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.gameOverTitle = surfaceAlphaLoad(RES_PATH "gameover_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;



	for (i = 0; i < hardwareKeyCount; i++) {
		char *str = "\0";

		switch (i) {
		default: 
		case hardwareKeyX: str = RES_PATH "x.png"; break;
		case hardwareKeyY: str = RES_PATH "y.png"; break;
		case hardwareKeyA: str = RES_PATH "a.png"; break;
		case hardwareKeyB: str = RES_PATH "b.png"; break;
		case hardwareKeyL: str = RES_PATH "l.png"; break;
		case hardwareKeyR: str = RES_PATH "r.png"; break;
		case hardwareKeySelect: str = RES_PATH "select.png"; break;
		}

		if ((e->io.btn[i] = surfaceAlphaLoad(str)) == NULL) {
			return false;
		}
		count++; e->io.loading += 1.f / COUNT;
	}

	if (!fontmapInit(&e->io.whiteFont, 6, 12, RES_PATH "white_font.png")) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!fontmapInit(&e->io.blackFont, 6, 12, RES_PATH "black_font.png")) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!fontmapInit(&e->io.yellowFont, 6, 12, RES_PATH "yellow_font.png")) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!fontmapInit(&e->io.darkRedFont, 6, 12, RES_PATH "darkred_font.png")) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!fontmapInit(&e->io.darkRedFont, 6, 12, RES_PATH "darkred_font.png")) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!fontmapInit(&e->io.normalFont, 8, 14, RES_PATH "normal_font.png")) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;

	e->io.normalFont.spacing = -2;
	if (!fontmapInit(&e->io.highlightFont, 8, 14, RES_PATH "highlight_font.png")) {
		return false;
	}
	e->io.highlightFont.spacing = -2;
	count++; e->io.loading += 1.f / COUNT;

	e->io.sq[sqNormal] = surfaceAlphaLoad(RES_PATH "sq_normal.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[sqDblLet] = surfaceAlphaLoad(RES_PATH "sq_dl.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[sqDblWrd] = surfaceAlphaLoad(RES_PATH "sq_dw.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[sqTrpLet] = surfaceAlphaLoad(RES_PATH "sq_tl.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[sqTrpWrd] = surfaceAlphaLoad(RES_PATH "sq_tw.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[sqFree] = surfaceAlphaLoad(RES_PATH "sq_free.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[sqMystery] = surfaceAlphaLoad(RES_PATH "sq_mystery.png");
	count++; e->io.loading += 1.f / COUNT;

	for (i = 0; i < sqCount; i++) {
		if (e->io.sq[i] == NULL) { 
			return false;
		}
	}
	

	tile[tileLookDisable] = surfaceAlphaLoad(RES_PATH "tile_disable.png");
	count++; e->io.loading += 1.f / COUNT;
	tile[tileLookNormal] = surfaceAlphaLoad(RES_PATH "tile_normal.png");
	count++; e->io.loading += 1.f / COUNT;
	tile[tileLookHold] = surfaceAlphaLoad(RES_PATH "tile_hold.png");
	count++; e->io.loading += 1.f / COUNT;
	tile[tileLookGhost] = surfaceAlphaLoad(RES_PATH "tile_ghost.png");
	count++; e->io.loading += 1.f / COUNT;
	tile[tileLookLast] = surfaceAlphaLoad(RES_PATH "tile_last.png");
	count++; e->io.loading += 1.f / COUNT;

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
			count++; e->io.loading += 1.f / COUNT;
			count++; e->io.loading += 1.f / COUNT;
		}
	}
	
	count++; e->io.loading += 1.f / COUNT;
	if (!dictInitCount7(&e->game.dict, &e->io.loading, 50.f / COUNT, RES_PATH "dict.txt")) {
		return false;
	}


	e->io.fader = surfaceCpy(e->io.screen);
	SDL_FillRect(e->io.fader, 0, SDL_MapRGBA(e->io.fader->format, 0, 0, 0, 0));

	e->io.pressStart = createText(&e->io.normalFont, "PRESS START");
	count++; e->io.loading += 1.f / COUNT;

	e->gui.scoreBoard.playerNum = e->game.playerNum = 0;
	initGUI(&e->gui);
	count++; e->io.loading += 1.f / COUNT;
	if (!initMenuViews(&e->io, &e->gui)) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;

	e->io.areYouSureQuit = createText(&e->io.normalFont, "Are you sure?");
	{
		char *text[gameMenuFocusCount] = {"Yes", "No"};
		mkHighTexts(e->io.yesNo, &e->io.normalFont, &e->io.highlightFont, text, yesNoCount);
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!areHighTextsLoaded(e->io.yesNo, yesNoCount)) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;

	if (!(e->io.incorrectSnd = Mix_LoadWAV(RES_PATH "incorrect_snd.wav"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.correctSnd = Mix_LoadWAV(RES_PATH "correct_snd.wav"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.scoreSnd = Mix_LoadWAV(RES_PATH "score_snd.wav"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;

	controlsInit(&e->io.controls);
	count++; e->io.loading += 1.f / COUNT;
	e->io.controls.accelExists = e->io.accelExists;
	e->io.controls.joyExists = e->io.joyExists;

	e->io.loading = 1.f;

	return true;
}

void* cbInitIO(void *data)
{
	Env *e;

	NOT(data);

	e = data;

	e->io.loadAttempted = false;
	e->io.loaded = initIO(e);
	e->io.loadAttempted = true;

	return NULL;
}

bool initMenuViews(IO *io, GUI *g)
{
	Font *n, *h;

	char *menuText[menuFocusCount] = {"Play", "Rules", "Settings", "Exit"};
	char *playMenuText[playMenuFocusCount] = {"1 Player", "2 Player", "Netplay (N/A)"};
	char *gameMenuText[gameMenuFocusCount] = {"Resume", "Settings", "Skip", "Quit"};
	char *settingsText[settingsFocusCount] = {"Music:     ", "  SFX:     ", "Controls"};
	char *yesNoText[yesNoCount] = {"Yes", "No"};
	char *controlsText[gameKeyCount] = {"Play:", "Recall:", "Shuffle:", "Mode:", "Select:", "Cancel:", "Prev Tile:","Next Tile:", "Up:", "Down:", "Left:", "Right:"};

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
	if (!initMenuView(&io->controlsMV, &g->controlsMenu.menu, controlsText, n, h)) {
		return false;
	}

	io->controlsMV.spacing.y = n->height + 1;
	recenterMenuView(&io->controlsMV, n);
	io->controlsMV.pos.x -= 36;
	io->controlsMV.pos.y += n->height  + n->height / 2;

	io->loading = 1.f;

	return true;
}

void drawScrollingBackground(IO *io);

void loadingScreen(Env *e)
{
	IO *io;
	pthread_t thread;
	float pause = 0.2f;

	io = &e->io;

	io->loadAttempted = false;

	if (pthread_create(&thread, NULL, cbInitIO, e) != 0) {
		io->loaded = false;
		return;	
	}
	
	while (!io->loadAttempted || (io->loadAttempted && pause > 0 && io->loaded)) {

		int st = SDL_GetTicks();

		if (io->loading >= 1.0) {
			pause -= SPF;
		}

		SDL_FillRect(io->screen, NULL, 0);
		drawScrollingBackground(io);
		surfaceDraw(io->screen, io->titleScreen, 0, 0);
		drawProgressBar(e->io.screen, io->loading, 60, 180, 200, 10, 1);
		SDL_Flip(io->screen);

		delay(st, SDL_GetTicks(), FPS);

		io->time += SPF;
	}
}

bool init(Env *e)
{
	NOT(e);

	e->io.time = 0;
	e->quit = false;
	e->io.loading = 0;

	e->game.dict.num = 0;
	e->game.dict.words = NULL;
	e->io.joystick = NULL;
	e->io.accel = NULL;
	e->io.joyExists = false;
	e->io.accelExists = false;
	e->io.song = NULL;

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return false;
	}
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("finite", NULL);
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, audioChanCount, 4096 ) == -1) {
		return false;
	}
	if ((e->io.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE)) == NULL) {
		return false;
	}
	if ((e->io.titleScreen = surfaceAlphaLoad(RES_PATH "title_screen.png")) == NULL) {
		return false;
	}
	if (!(e->io.titleHover = surfaceAlphaLoad(RES_PATH "title_hover.png"))) {
		return false;
	}
	if ((e->io.titleBackground = surfaceAlphaLoad(RES_PATH "title_background.png")) == NULL) {
		return false;
	}


	e->io.song = Mix_LoadMUS(RES_PATH "George Street Shuffle (filtered).ogg");
	if (!e->io.song) {
		return false;
	}
	if (Mix_PlayMusic(e->io.song, -1) == -1) {
		return false;
	}

	loadingScreen(e);

	return e->io.loaded; 
}

void quit(Env *e)
{
	int i, j;

	NOT(e);

	if (e->io.song) {
		Mix_FreeMusic(e->io.song);
	}
	
	if (e->io.joystick) {
		SDL_JoystickClose(e->io.joystick);
	}
	if (e->io.accel) {
		SDL_JoystickClose(e->io.accel);
	}

	dictQuit(&e->game.dict);

	surfaceFree(e->io.screen);
	surfaceFree(e->io.titleScreen);
	surfaceFree(e->io.titleHover);
	surfaceFree(e->io.settingsTitle);
	surfaceFree(e->io.pauseTitle);
	surfaceFree(e->io.finiteTitle);
	surfaceFree(e->io.controlsTitle);
	surfaceFree(e->io.areYouSureTitle);
	surfaceFree(e->io.chooseGameTitle);
	surfaceFree(e->io.rulesTitle);
	surfaceFree(e->io.optionsTitle);
	surfaceFree(e->io.gameOverTitle);
	surfaceFree(e->io.menuBg);
	surfaceFree(e->io.gmBack);
	surfaceFree(e->io.titleBackground);
	surfaceFree(e->io.scoreBoard);
	surfaceFree(e->io.textLog);
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
	surfaceFree(e->io.boardCover);
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

	for (i = 0; i < hardwareKeyCount; i++) {
		surfaceFree(e->io.btn[i]);
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

	Mix_FreeChunk(e->io.incorrectSnd);
	Mix_FreeChunk(e->io.correctSnd);
	Mix_FreeChunk(e->io.scoreSnd);
	Mix_CloseAudio();
	SDL_Quit();
}


float normalizeAxis(int v)
{
	const int  max = 32768;
	return (float)v / (float)max;
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
			if (event.jaxis.which == 0) {
				if (event.jaxis.axis == 0) {
					hc->axisX.value = normalizeAxis(event.jaxis.value);
				}
				if (event.jaxis.axis == 1) {
					hc->axisY.value = -normalizeAxis(event.jaxis.value);
				}
			}
			if (event.jaxis.which == 1) {
				if (event.jaxis.axis == 0) {
					hc->accelX.value = normalizeAxis(event.jaxis.value);
				}
				if (event.jaxis.axis == 1) {
					hc->accelY.value = -normalizeAxis(event.jaxis.value);
				}
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

	if (c->joyExists) {
		axisStateUpdate(&hc->axisX);
		axisStateUpdate(&hc->axisY);
	}
	
	if (c->accelExists) {
		axisStateUpdate(&hc->accelX);
		axisStateUpdate(&hc->accelY);
	}


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
		q = handleEvent(&e->io.controls);
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

