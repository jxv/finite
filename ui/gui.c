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
	f->width = w;
	f->height = h;
	f->spacing = 0;
	f->map = surface_alpha_load(filename);
	return f->map != NULL;
}

void fontmapQuit(Font *f)
{
	if (f->map)
		surface_free(f->map);
}

void initKeyState(key_state_t *ks)
{
	ks->type = KEY_STATE_UNTOUCHED;
	ks->time = 0.0f;
}

void initAxisState(axis_state_t *as)
{
	const float dead_zone = 0.33f;
	as->type = AXIS_STATE_IN_DEAD_ZONE;
	as->dead_zone = dead_zone;
	as->time = 0.f;
	as->value = 0.f;
}

void initGameControls(GameControls *gc)
{
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

void initDefaultRule(rule_t *r)
{
	r->place = NULL;
	r->discard = NULL;
	r->skip = NULL;
	r->quit = NULL;
}

void initGame1vs1Human(game_t *g)
{
	board_init(&g->board);
	bag_init(&g->bag);
	g->playerNum = 2;
	init_player_human(&g->player[0], &g->bag);
	init_player_human(&g->player[1], &g->bag);
	g->player[0].active = true;
	g->player[1].active = true;
	initDefaultRule(&g->rule);
}

void initGame1vs1HumanAI(game_t *g)
{
	board_init(&g->board);
	bag_init(&g->bag);
	g->playerNum = 2;
	init_player_human(&g->player[0], &g->bag);
	init_player_ai(&g->player[1], &g->bag);
	g->player[0].active = true;
	g->player[1].active = true;
	initDefaultRule(&g->rule);
}

void initScoreBoard(ScoreBoard *sb, game_t *g)
{
	int i;
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
	m->next = focus;
	m->focus = focus;
	m->max = max;
}

void initSettings(Settings *s)
{
	for (int i = 0; i < volCount; i++)
		s->vol[i] = MAX_GUI_VOLUME;
	s->previous = guiFocusMenu;
	initMenuWidget(&s->menu, settingsFocusSfx, settingsFocusCount);
}

void initOptions(Options *o)
{
	o->ai = 5;

	initMenuWidget(&o->menu, optionsFocusAI, optionsFocusCount);
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
	mkRackWidget(&gg->rackWidget);
	mkBoardWidget(&gg->boardWidget);
	gg->focus = gameGUIFocusBoard;
	gg->bottomLast = gameGUIFocusRack;
	initTextLog(&gg->textLog);
	gg->lastMove.type = lastMoveNone;
}

void actionToLastMove(LastMove *lm, action_t *a)
{
		coor_t idx;
		path_t *p;

		NOT(lm);
		NOT(a);
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++)
			for (idx.x = 0; idx.x < BOARD_X; idx.x++)
				lm->data.place[idx.y][idx.x] = false;
		if (a->type != ACTION_PLACE) {
			lm->type = lastMoveNone;
			return;
		}
		lm->type = lastMovePlace;

		p = &a->data.place.path;

		switch (p->type) {
		case PATH_DOT: {
			if (p->data.dot.right.type == DIR_RIGHT) {
				lm->data.place[p->data.dot.right.y]
					      [p->data.dot.right.x] = true;
			}
			if (p->data.dot.down.type == DIR_DOWN) {
				lm->data.place[p->data.dot.down.y]
					      [p->data.dot.down.x] = true;
			}
			break;
		}
		case PATH_HORZ: {
			if (p->data.horz.right.type != DIR_RIGHT) {
				lm->type = lastMoveNone;
				break;
			}
			idx.y = p->data.horz.right.y;
			for (int x = p->data.horz.right.x;
			     x < p->data.horz.right.x + p->data.horz.right.len;
			     x++)
				lm->data.place[idx.y][x] =
					p->data.horz.right.pos[x];
			break;
		}
		case PATH_VERT: {
			if (p->data.vert.down.type != DIR_DOWN) {
				lm->type = lastMoveNone;
				break;
			}
			idx.x = p->data.vert.down.x;
			for (idx.y = p->data.vert.down.y;
			     idx.y < p->data.vert.down.y +
				p->data.vert.down.len;
			     idx.y++)
				lm->data.place[idx.y][idx.x] =
					p->data.vert.down.pos[idx.y];
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
	initOptions(&g->options);
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

int mkHighTexts(HighText *ht, Font *normal, Font *highlight, char **text, int count, int *distance)
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
	if (distance) {
		*distance =  max * (normal->width + normal->spacing);
	}
	return max;
}

bool areHighTextsLoaded(HighText *ht, int count)
{
	for (int i = 0; i < count; i++)
		if (!(ht[i].normal && ht[i].highlight))
			return false;
	return true;
}

void freeHighTexts(HighText *ht, int count)
{
	for (int i = 0; i < count; i++) {
		surface_free(ht[i].normal);
		surface_free(ht[i].highlight);
	}
}

void recenterMenuView(MenuView *mv, Font *n)
{
	mv->pos.x = (SCREEN_WIDTH - mv->len * (n->width + n->spacing)) / 2;
	mv->pos.y = (SCREEN_HEIGHT - mv->menu->max * mv->spacing.y) / 2;
}

bool initMenuView(MenuView *mv, MenuWidget *mm, char *str[], Font *n, Font *h)
{
	mv->spacing.x = 0;
	mv->spacing.y = n->height * 2;
	mv->menu = mm;
	mv->text = memAlloc(sizeof(HighText) * mm->max);
	mv->len = mkHighTexts(mv->text, n, h, str, mm->max, &mv->distance);
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
	COUNT = 331 + 8 * 50.f;
	
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
	if ((e->io.menuBg = surface_alpha_load(RES_PATH "menu_bg.png")) == NULL)
		return false;
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.scoreBoard = surface_alpha_load(RES_PATH "scoreboard.png")) == NULL)
		return false;
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.textLog = surface_alpha_load(RES_PATH "text_log.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.gmBack = surface_alpha_load(RES_PATH "game_bg.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.back = surface_alpha_load(RES_PATH "back.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.wildUp = surface_alpha_load(RES_PATH "wild_up.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.wildDown = surface_alpha_load(RES_PATH "wild_down.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.lockon = surface_alpha_load(RES_PATH "lockon.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.lockon0 = surface_alpha_load(RES_PATH "lockon0.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.recall = surface_alpha_load(RES_PATH "recall.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.recallDisable = surface_alpha_load(RES_PATH "recall_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.mode = surface_alpha_load(RES_PATH "mode.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.modeDisable = surface_alpha_load(RES_PATH "mode_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.place = surface_alpha_load(RES_PATH "place.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.placeDisable = surface_alpha_load(RES_PATH "place_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.discard = surface_alpha_load(RES_PATH "discard.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.discardDisable = surface_alpha_load(RES_PATH "discard_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.skip = surface_alpha_load(RES_PATH "skip.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.skipDisable = surface_alpha_load(RES_PATH "skip_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.play = surface_alpha_load(RES_PATH "play.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.playDisable = surface_alpha_load(RES_PATH "play_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.shuffle = surface_alpha_load(RES_PATH "shuffle.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if ((e->io.shuffleDisable = surface_alpha_load(RES_PATH "shuffle_disable.png")) == NULL) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;

	if (!(e->io.boardCover = surface_alpha_load(RES_PATH "board_cover.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	
	if (!(e->io.pauseTitle = surface_alpha_load(RES_PATH "pause.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.settingsTitle = surface_alpha_load(RES_PATH "settings.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.finiteTitle = surface_alpha_load(RES_PATH "finite_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.controlsTitle = surface_alpha_load(RES_PATH "controls_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.areYouSureTitle = surface_alpha_load(RES_PATH "are_you_sure_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.chooseGameTitle = surface_alpha_load(RES_PATH "choose_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.rulesTitle = surface_alpha_load(RES_PATH "rules_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.optionsTitle = surface_alpha_load(RES_PATH "options_title.png"))) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!(e->io.gameOverTitle = surface_alpha_load(RES_PATH "gameover_title.png"))) {
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

		if ((e->io.btn[i] = surface_alpha_load(str)) == NULL) {
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

	e->io.sq[SQ_NORMAL] = surface_alpha_load(RES_PATH "sq_normal.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[SQ_DBL_LET] = surface_alpha_load(RES_PATH "sq_dl.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[SQ_DBL_WRD] = surface_alpha_load(RES_PATH "sq_dw.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[SQ_TRP_LET] = surface_alpha_load(RES_PATH "sq_tl.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[SQ_TRP_WRD] = surface_alpha_load(RES_PATH "sq_tw.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[SQ_FREE] = surface_alpha_load(RES_PATH "sq_free.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[SQ_NO_VOWEL] = surface_alpha_load(RES_PATH "sq_novowel.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[SQ_BLOCK] = surface_alpha_load(RES_PATH "sq_block.png");
	count++; e->io.loading += 1.f / COUNT;
	e->io.sq[SQ_MYSTERY] = surface_alpha_load(RES_PATH "sq_mystery.png");
	count++; e->io.loading += 1.f / COUNT;

	for (i = 0; i < SQ_COUNT; i++) {
		if (e->io.sq[i] == NULL) { 
			return false;
		}
	}
	

	tile[tileLookDisable] = surface_alpha_load(RES_PATH "tile_disable.png");
	count++; e->io.loading += 1.f / COUNT;
	tile[tileLookNormal] = surface_alpha_load(RES_PATH "tile_normal.png");
	count++; e->io.loading += 1.f / COUNT;
	tile[tileLookHold] = surface_alpha_load(RES_PATH "tile_hold.png");
	count++; e->io.loading += 1.f / COUNT;
	tile[tileLookGhost] = surface_alpha_load(RES_PATH "tile_ghost.png");
	count++; e->io.loading += 1.f / COUNT;
	tile[tileLookLast] = surface_alpha_load(RES_PATH "tile_last.png");
	count++; e->io.loading += 1.f / COUNT;

	for (i = 0; i < tileLookCount; i++) {
		e->io.wild[i] = tile[i];
		if (!e->io.wild[i]) {
			return false;
		}
	}

	for (i = 0; i < LETTER_COUNT; i++) {
		for (j = 0; j < tileLookCount; j++) {
			e->io.tile[TILE_WILD][i][j] = surface_cpy(tile[j]);
			if (!e->io.tile[TILE_WILD][i][j]) {
				return false;
			}
			e->io.tile[TILE_LETTER][i][j] = surface_cpy(tile[j]);
			if (!e->io.tile[TILE_LETTER][i][j]) {
				return false;
			}
			sprintf(str,"%c", i + 'a');
			strDraw(e->io.tile[TILE_WILD][i][j], &e->io.blackFont,
				str, 3, 0);
			sprintf(str,"%c", i + 'A');
			strDraw(e->io.tile[TILE_LETTER][i][j], &e->io.blackFont,
				str, 3, 0);
			count++; e->io.loading += 1.f / COUNT;
			count++; e->io.loading += 1.f / COUNT;
		}
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!dict_init_count_7(&e->game.dict, &e->io.loading, 50.f / COUNT,
			    RES_PATH "dict.txt"))
		return false;


	e->io.fader = surface_cpy(e->io.screen);
	SDL_FillRect(e->io.fader, 0, SDL_MapRGBA(e->io.fader->format, 0, 0, 0,
						 0));

	e->io.pressStart = createText(&e->io.normalFont, "PRESS START");
	count++; e->io.loading += 1.f / COUNT;

	e->gui.scoreBoard.playerNum = e->game.playerNum = 0;
	initGUI(&e->gui);
	count++; e->io.loading += 1.f / COUNT;
	if (!initMenuViews(&e->io, &e->gui))
		return false;
	count++; e->io.loading += 1.f / COUNT;

	e->io.areYouSureQuit = createText(&e->io.normalFont, "Are you sure?");
	{
		char *text[gameMenuFocusCount] = {"Yes", "No"};
		mkHighTexts(e->io.yesNo, &e->io.normalFont,
			    &e->io.highlightFont, text, yesNoCount, NULL);
	}
	count++; e->io.loading += 1.f / COUNT;
	if (!areHighTextsLoaded(e->io.yesNo, yesNoCount)) {
		return false;
	}
	count++; e->io.loading += 1.f / COUNT;

	if (!(e->io.openSnd = Mix_LoadWAV(RES_PATH "open_snd.wav"))) {
		return false;
	}
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
	if (!(e->io.enterSnd = Mix_LoadWAV(RES_PATH "enter_snd.wav"))) {
		return false;
	}
	if (!(e->io.backSnd = Mix_LoadWAV(RES_PATH "back_snd.wav"))) {
		return false;
	}
	if (!(e->io.scrollSnd = Mix_LoadWAV(RES_PATH "scroll_snd.wav"))) {
		return false;
	}
	if (!(e->io.startSnd = Mix_LoadWAV(RES_PATH "start_snd.wav"))) {
		return false;
	}
	if (!(e->io.pauseSnd = Mix_LoadWAV(RES_PATH "pause_snd.wav"))) {
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
	char *playMenuText[playMenuFocusCount] = {"1 Player", "2 Player", "Options"};
	char *optionsText[optionsFocusCount] = {"AI Difficulty:"};
	char *gameMenuText[gameMenuFocusCount] = {"Resume", "Settings", "Skip", "Quit"};
	char *settingsText[settingsFocusCount] = {"  SFX:     ", "Controls"};
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
	if (!initMenuView(&io->optionsMV, &g->options.menu, optionsText, n, h)) {
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

	io->optionsMV.pos.x -= 46;
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

		if (io->loading >= 1.0)
			pause -= SPF;
		SDL_FillRect(io->screen, NULL, 0);
		drawScrollingBackground(io);
		surface_draw(io->screen, io->titleScreen, 0, 0);
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

	e->io.openSnd = NULL;
	e->io.incorrectSnd = NULL;
	e->io.correctSnd = NULL;
	e->io.scoreSnd = NULL;
	e->io.enterSnd = NULL;
	e->io.backSnd = NULL;
	e->io.scrollSnd = NULL;
	e->io.pauseSnd = NULL;

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return false;
	}
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("finite", NULL);
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, audioChanCount, 4096 ) == -1)
		return false;
	if ((e->io.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
					     SCREEN_BPP,
					     SDL_SWSURFACE)) == NULL)
		return false;
	if ((e->io.titleScreen =
	     surface_alpha_load(RES_PATH "title_screen.png")) == NULL)
		return false;
	if (!(e->io.titleHover =
	      surface_alpha_load(RES_PATH "title_hover.png")))
		return false;
	if ((e->io.titleBackground =
	     surface_alpha_load(RES_PATH "title_background.png")) == NULL)
		return false;
	if ((e->io.menuBackground =
	     surface_alpha_load(RES_PATH "menu_background.png")) == NULL)
		return false;
	loadingScreen(e);
	e->game.player[1].aiShare.difficulty = e->gui.options.ai;
	return e->io.loaded;
}

void freeChunk(Mix_Chunk *c)
{
	if (c)
		Mix_FreeChunk(c);
}

void freeMusic(Mix_Music *m)
{
	if (m)
		Mix_FreeMusic(m);
}

void quit(Env *e)
{
	if (e->io.joystick)
		SDL_JoystickClose(e->io.joystick);
	if (e->io.accel)
		SDL_JoystickClose(e->io.accel);

	dict_quit(&e->game.dict);

	surface_free(e->io.screen);
	surface_free(e->io.titleScreen);
	surface_free(e->io.titleHover);
	surface_free(e->io.settingsTitle);
	surface_free(e->io.pauseTitle);
	surface_free(e->io.finiteTitle);
	surface_free(e->io.controlsTitle);
	surface_free(e->io.areYouSureTitle);
	surface_free(e->io.chooseGameTitle);
	surface_free(e->io.rulesTitle);
	surface_free(e->io.optionsTitle);
	surface_free(e->io.gameOverTitle);
	surface_free(e->io.menuBg);
	surface_free(e->io.gmBack);
	surface_free(e->io.titleBackground);
	surface_free(e->io.menuBackground);
	surface_free(e->io.scoreBoard);
	surface_free(e->io.textLog);
	surface_free(e->io.pressStart);
	surface_free(e->io.back);
	surface_free(e->io.lockon);
	surface_free(e->io.wildUp);
	surface_free(e->io.wildDown);
	surface_free(e->io.recall);
	surface_free(e->io.recallDisable);
	surface_free(e->io.mode);
	surface_free(e->io.modeDisable);
	surface_free(e->io.place);
	surface_free(e->io.discard);
	surface_free(e->io.discardDisable);
	surface_free(e->io.skip);
	surface_free(e->io.skipDisable);
	surface_free(e->io.play);
	surface_free(e->io.playDisable);
	surface_free(e->io.shuffle);
	surface_free(e->io.shuffleDisable);
	surface_free(e->io.boardCover);
	for (int i = 0; i < SQ_COUNT; i++)
		surface_free(e->io.sq[i]);
	for (int i = 0; i < tileLookCount; i++)
		surface_free(e->io.wild[i]);
	for (int i = 0; i < LETTER_COUNT; i++)
		for (int j = 0; j < tileLookCount; j++) {
			surface_free(e->io.tile[TILE_WILD][i][j]);
			surface_free(e->io.tile[TILE_LETTER][i][j]);
		}
	for (int i = 0; i < hardwareKeyCount; i++)
		surface_free(e->io.btn[i]);
	freeMenuViews(&e->io);
	freeHighTexts(e->io.menuFocus, menuFocusCount);
	freeHighTexts(e->io.gameMenuFocus, gameMenuFocusCount);
	surface_free(e->io.areYouSureQuit);
	freeHighTexts(e->io.yesNo, yesNoCount);
	fontmapQuit(&e->io.whiteFont);
	fontmapQuit(&e->io.blackFont);
	fontmapQuit(&e->io.yellowFont);
	fontmapQuit(&e->io.darkRedFont);

	freeChunk(e->io.openSnd);
	freeChunk(e->io.incorrectSnd);
	freeChunk(e->io.correctSnd);
	freeChunk(e->io.scoreSnd);
	freeChunk(e->io.enterSnd);
	freeChunk(e->io.backSnd);
	freeChunk(e->io.scrollSnd);
	freeChunk(e->io.startSnd);
	freeChunk(e->io.pauseSnd);

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
		axis_state_update(&hc->axisX);
		axis_state_update(&hc->axisY);
	}
	if (c->accelExists) {
		axis_state_update(&hc->accelX);
		axis_state_update(&hc->accelY);
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

