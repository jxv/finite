#include <string.h>
#include <math.h>

#include "gui.h"
#include "draw.h"
#include "widget.h"

void drawScoreBoard(ScoreBoard *sb, IO *io);

void strDraw(SDL_Surface *s, Font *f, const char *str, int x, int y)
{
	int i;
	char c;
	SDL_Rect offset, clip, prev;

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
		if (c == '\n') {
			offset.x = x;
			offset.y += f->height;
			continue;
		}
		/* [32..126] are drawable ASCII chars */
		if (c >= 32 && c <= 126) {
			clip.x = f->width * (c - 32);
			prev = offset;
			SDL_BlitSurface(f->map, &clip, s, &offset);
			offset = prev;
		}
		offset.x += f->width + f->spacing;
	}
}

void drawProgressBar(SDL_Surface *s, float progress, int x, int y, int w, int h, int sp)
{
	SDL_Rect rect;

	NOT(s);

	rect.x = x - sp;
	rect.y = y - sp;
	rect.w = w + sp * 2;
	rect.h = h + sp * 2;
	SDL_FillRect(s, &rect, SDL_MapRGB(s->format, 0x00, 0x00, 0x00));

	rect.x = x;
	rect.y = y;
	rect.w = w * progress;
	rect.h = h;
	SDL_FillRect(s, &rect, SDL_MapRGB(s->format, 0xff, 0xff, 0xff));
}

void printCmd(Cmd *c)
{
	NOT(c);

	switch (c->type) {
	case cmdFocusTop: puts("[cmd:focus-top]"); break;
	case cmdFocusBottom: puts("[cmd:focus-bottom]"); break;
	case cmdBoardSelect: printf("[cmd:board-select (%d,%d)]\n", c->data.board.x, c->data.board.y); break;
	case cmdRackSelect: printf("[cmd:rack-select %d]\n", c->data.rack); break;
	case cmdBoard: printf("[cmd:board (%d, %d)]\n", c->data.board.x, c->data.board.y); break;
	case cmdBoardLeft: puts("[cmd:board-left]"); break;
	case cmdBoardRight: puts("[cmd:board-right]"); break;
	case cmdBoardUp: puts("[cmd:board-up]"); break;
	case cmdBoardDown: puts("[cmd:board-down]"); break;
	case cmdRack: printf("[cmd:rack %d]\n", c->data.rack); break;
	case cmdRackLeft: puts("[cmd:rack-left]"); break;
	case cmdRackRight: puts("[cmd:rack-right]"); break;
	case cmdRecall: puts("[cmd:recall]"); break;
	case cmdMode: puts("[cmd:mode]"); break;
	case cmdPlay: puts("[cmd:play]"); break;
	case cmdShuffle: puts("[cmd:shuffle]"); break;
	case cmdBoardCancel: printf("[cmd:board-cancel (%d,%d)\n]", c->data.board.x, c->data.board.y); break;
	case cmdRackCancel: printf("[cmd:rack-cancel %d]", c->data.rack); break;
	case cmdTilePrev: puts("[cmd:tile-prev]"); break;
	case cmdTileNext: puts("[cmd:tile-next]"); break;
	case cmdQuit: puts("[cmd:quit]"); break;
	/*case cmdInvalid: puts("[cmd:invalid]"); break; // very noisy */
	default: break;
	}
}

void printTransMove(TransMove *tm)
{
	NOT(tm);

	switch (tm->type) {
	case transMoveNone: puts("[trans-move:none]"); break;
	case transMovePlace: puts("[trans-move:place]"); break;
	case transMovePlaceWild: puts("[trans-move:place-wild]"); break;
	case transMovePlaceEnd: puts("[trans-move:place-end]"); break;
	case transMovePlacePlay: puts("[trans-move:place-play]"); break;
	case transMoveDiscard: puts("[trans-move:discard]"); break;
	case transMoveDiscardPlay: puts("[trans-move:discard-play]"); break;
	case transMoveSkip: puts("[trans-move:skip]"); break;
	case transMoveSkipPlay: puts("[trans-move:skip-play]"); break;
	case transMoveQuit: puts("[trans-move:quit]"); break;
	case transMoveInvalid: puts("[trans-move:invalid]"); break;
	default: break;
	}
}

void guiDrawLockon(IO *io, GameGUI *gg)
{
	const int w = TILE_WIDTH, h = TILE_HEIGHT; 
	Coor idx;

	NOT(io);
	NOT(gg);
	
	switch (gg->focus) {
	case gameGUIFocusBoard: {
		idx = gg->boardWidget.index;
		surfaceDraw(io->screen, io->lockon, gg->boardWidget.pos.x - 2 + idx.x * w, gg->boardWidget.pos.y - 2 + idx.y * h);
		break;
	}
	case gameGUIFocusRack: {
		idx = gg->rackWidget.index;
		if (interval(io->time, 0.2f)) {
			surfaceDraw(io->screen, io->lockon, gg->rackWidget.pos.x - 2 + idx.x * w, gg->rackWidget.pos.y - 2);
		}
		break;
	}
	default: break;
	}
}

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
	
	if (gf != gameGUIFocusBoard) {
		return;
	}

	switch (tm->type) {
	case transMovePlace: {
		i = tm->adjust.data.tile[tm->place.idx].idx;
		idx = bw->index;
		t = &p->tile[i];
		if (t->type == tileNone) {
			break;
		}
		s = t->type == tileWild ? io->wild[tileLookGhost] : io->tile[t->type][t->letter][tileLookGhost];
		surfaceDraw(io->screen, s, idx.x * TILE_WIDTH + bw->pos.x, idx.y * TILE_HEIGHT + bw->pos.y);
		break;
	}
	case transMovePlaceWild: {
		i = tm->adjust.data.tile[tm->place.idx].idx;
		idx = bw->index;
		t = &p->tile[i];
		surfaceDraw(io->screen, io->wildUp, idx.x * TILE_WIDTH + bw->pos.x,
			(idx.y-1) * TILE_HEIGHT + bw->pos.y + (TILE_HEIGHT/2));
		surfaceDraw(io->screen, io->wildDown, idx.x * TILE_WIDTH + bw->pos.x,
			(idx.y+1) * TILE_HEIGHT + bw->pos.y);
		break;
	}
	default: break;
	}
}

void guiDrawBoard(IO *io, GridWidget *bw, Game *g, TransMove *tm, LastMove *lm)
{
	Coor dim;
	
	NOT(io);
	NOT(bw);
	NOT(g);
	NOT(tm);

	dim.x = TILE_WIDTH;
	dim.y = TILE_HEIGHT;

	boardWidgetDraw(io, bw, &g->player[tm->playerIdx], &g->board, tm, lm, dim);
}

void guiDrawRack(IO *io, GridWidget *rw, Game *g, TransMove *tm)
{
	Coor dim;
	
	NOT(io);
	NOT(rw);
	NOT(g);
	NOT(tm);

	dim.x = TILE_WIDTH;
	dim.y = TILE_HEIGHT;

	rackWidgetDraw(io, tm, rw, dim, &g->player[tm->playerIdx]);
}

bool interval(float lapsed, float interval)
{
	return ((int)floorf(lapsed / interval)) % 2 == 0;
}

void guiDraw(IO *io, GUI *g, Game *gm, TransMove *tm, GameControls *gc)
{
	int i, j;

	NOT(io);
	NOT(g);
	NOT(tm);
	NOT(gc);
	
	surfaceDraw(io->screen, io->gmBack, 0, 0);
	guiDrawBoard(io, &g->gameGui.boardWidget, gm, tm, &g->gameGui.lastMove);
	guiDrawRack(io, &g->gameGui.rackWidget, gm, tm);
	drawScoreBoard(&g->scoreBoard, io);

	
	if (gm->turn == tm->playerIdx && interval(io->time, 0.2f)) {
		guiDrawGhostTile(io, g->gameGui.focus, tm, &gm->player[tm->playerIdx], &g->gameGui.boardWidget);
	}
	guiDrawLockon(io, &g->gameGui);
	if (tm->type == transMoveDiscard || tm->type == transMoveDiscardPlay) {
		Tile *t;

		strDraw(io->screen, &io->normalFont, "DISCARD", 126 + 12, 8);
		surfaceDraw(io->screen, io->boardCover, g->gameGui.boardWidget.pos.x, g->gameGui.boardWidget.pos.y);
		
		i = tm->adjust.data.tile[tm->place.idx].idx;
		t = &gm->player[tm->playerIdx].tile[i];
		if (t->type != tileNone) {
			char str0[32];
			SDL_Surface *s;
			s = t->type == tileWild ? io->wild[tileLookNormal] : io->tile[t->type][t->letter][tileLookNormal];
			surfaceDraw(io->screen, s, 269, 9);
			sprintf(str0,": %d", tileScore(t));
			strDraw(io->screen, &io->normalFont, str0, 281, 8);
		}
	} else {
		Coor *idx;
		char *str = "\0";
		Tile *t;

		idx = &g->gameGui.boardWidget.index;

		strDraw(io->screen, &io->normalFont, "PLACE", 132 + 12,  8);
		if (g->gameGui.focus == gameGUIFocusBoard) {
			SqType sq;
			sq = gm->board.sq[idx->y][idx->x];

			surfaceDraw(io->screen, io->sq[sq], 190, 9);

			str = "\0";
			switch (sq) {
			case sqNormal: str = "NO BONUS"; break;
			case sqDblLet: str = "x2 LETTER"; break;
			case sqTrpLet: str = "x3 LETTER"; break;
			case sqDblWrd: str = "x2 WORD"; break;
			case sqTrpWrd: str = "x3 WORD"; break;
			case sqFree:   str = "FREE SQ."; break;
			case sqMystery: str = "MYSTERY"; break;
			default: break;
			}
			strDraw(io->screen, &io->normalFont, str, 204, 8);

		}

		i = tm->adjust.data.tile[tm->place.idx].idx;
		t = &gm->player[tm->playerIdx].tile[i];
		if (t->type != tileNone) {
			char str0[32];
			SDL_Surface *s;
			s = t->type == tileWild ? io->wild[tileLookNormal] : io->tile[t->type][t->letter][tileLookNormal];
			surfaceDraw(io->screen, s, 269, 9);
			sprintf(str0,": %d", tileScore(t));
			strDraw(io->screen, &io->normalFont, str0, 281, 8);
		}
	}

/* hack for printing bag count */
	{
		int len;
		char tilesLeft[32] = "\0";
		if (!bagEmpty(&gm->bag)) {
			sprintf(tilesLeft, "BAG: %d", bagCount(&gm->bag));
		} else {
			int idx;
			assert(gm->turn == 0 || gm->turn == 1);
			idx = gm->turn == 0 ? 1 : 0;
			sprintf(tilesLeft, "PLAYER %d: %d", idx + 1, rackCount(&gm->player[idx]));
		}
		strDraw(io->screen, &io->normalFont, tilesLeft, 13, 82);
		len = strlen(tilesLeft) * (io->normalFont.width + io->normalFont.spacing) + 13;
		len += 6;
		surfaceDraw(io->screen, io->wild[tileLookNormal], len, 83);
		len += 12;
		strDraw(io->screen, &io->normalFont, "'s", len, 82);
	}
/* hack above */

	for (i = 0, j = g->gameGui.textLog.head; i < g->gameGui.textLog.size; i++, j++, j %= g->gameGui.textLog.size) {
		strDraw(io->screen, &io->blackFont, g->gameGui.textLog.line[j], 16, 84 + 24 + 12 * i);
	}

	if (tm->type == transMovePlace || tm->type == transMovePlacePlay || tm->type == transMovePlaceEnd) {
		surfaceDraw(io->screen, io->btn[gc->key[gameKeyPrevTile]], 157, 218);
		surfaceDraw(io->screen, io->btn[gc->key[gameKeyNextTile]], 274, 218);
	}


}

int scrollOffset(int dis, int pps, float time)
{
	int secsToLoop;
	secsToLoop = dis / pps;
	return (int)(dis * time / secsToLoop) % dis;
}

void drawScrollingBackground(IO *io)
{
	int off0, off1;
	
	NOT(io);

	off0 = scrollOffset(io->titleBackground->h, 20, io->time);
	off1 = scrollOffset(io->titleHover->h, -30, io->time);

	surfaceDraw(io->screen, io->titleBackground, 0, off0);
	surfaceDraw(io->screen, io->titleBackground, 0, off0 - io->titleBackground->h);
	
	surfaceDraw(io->screen, io->titleHover, 0, off1);
	surfaceDraw(io->screen, io->titleHover, 0, off1 + io->titleHover->h);
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

void drawFader(IO *io, int val)
{
	NOT(io);
	assert(val >= 0);
	assert(val < 256);

	SDL_FillRect(io->fader, 0, SDL_MapRGBA(io->fader->format, 0, 0, 0, val));
	SDL_SetAlpha(io->fader, SDL_SRCALPHA, val);
	surfaceDraw(io->screen, io->fader, 0, 0);
}

void drawScoreBoard(ScoreBoard *sb, IO *io)
{
	char text[32];
	int i;
	Font *f;

	NOT(sb);
	NOT(io);
	
	surfaceDraw(io->screen, io->scoreBoard, 8, 6);
	surfaceDraw(io->screen, io->textLog, 8, 104);
	for (i = 0; i < sb->playerNum; i++) {
		f = i == sb->turn || sb->ctr[i].cur < sb->ctr[i].end 
				? &io->highlightFont 
				: &io->normalFont;
		drawNum(io->screen, 100, (f->height + 1) * i + 11, sb->ctr[i].cur, f);
		sprintf(text, "PLAYER %d", i+1);
		strDraw(io->screen, f, text, 15, (f->height + 1) * i + 11);
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

void drawMenuViewRight(SDL_Surface *s, MenuView *mv)
{
	SDL_Surface *t;
	int i, x, y;

	NOT(s);
	NOT(mv);

	for (i = 0; i < mv->menu->max; i++) {
		t = mv->menu->focus == i ? mv->text[i].highlight : mv->text[i].normal;
		NOT(t);
		x = mv->pos.x + mv->text[i].offset * 2;
		y = mv->pos.y + mv->spacing.y * i;
		surfaceDraw(s, t, x, y);
	}
}

void drawMenuViewLeft(SDL_Surface *s, MenuView *mv)
{
	SDL_Surface *t;
	int i, x, y;

	NOT(s);
	NOT(mv);

	for (i = 0; i < mv->menu->max; i++) {
		t = mv->menu->focus == i ? mv->text[i].highlight : mv->text[i].normal;
		NOT(t);
		x = mv->pos.x;
		y = mv->pos.y + mv->spacing.y * i;
		surfaceDraw(s, t, x, y);
	}
}

void drawDemoInfo(Env *e)
{
	NOT(e);
	
	strDraw(e->io.screen, &e->io.normalFont, "Demo August 2013", 0, SCREEN_HEIGHT - e->io.normalFont.height);
}

void draw_guiFocusTitle(Env *e)
{
	NOT(e);

	drawScrollingBackground(&e->io);
	surfaceDraw(e->io.screen, e->io.titleScreen, 0, 0);
	if ((e->io.time - floorf(e->io.time)) > 0.5) {
		surfaceDraw(e->io.screen, e->io.pressStart, 128, 200);
	}
	/*drawProgressBar(e->io.screen, 0.5f, 60, 180, 200, 10, 1);*/
	drawDemoInfo(e);
}

void draw_guiFocusMenu(Env *e)
{
	NOT(e);

	drawScrollingBackground(&e->io);
	surfaceDraw(e->io.screen, e->io.finiteTitle, 0, 0);
	drawMenuView(e->io.screen, &e->io.menuMV);
}

void draw_guiFocusRules(Env *e)
{
	SDL_Rect rect;
	float scroll;

	NOT(e);

	drawScrollingBackground(&e->io);

	scroll = -e->gui.rules * 320;

	rect.x = 49;
	rect.y = 0;
	rect.w = SCREEN_WIDTH - rect.x * 2;
	rect.h = SCREEN_HEIGHT;
	SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x50, 0x50, 0xa0, 0xff));

	rect.x = 50;
	rect.y = 0;
	rect.w = SCREEN_WIDTH - rect.x * 2;
	rect.h = SCREEN_HEIGHT;
	SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x00, 0x00, 0x40, 0xff));
	

	rect.x += rect.w - 6;
	rect.w = 3;
	rect.h = 45;
	rect.y = e->gui.rules * (SCREEN_HEIGHT - rect.h - 4) + 2 - 1;
	rect.h = 47;
	SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x00, 0x00, 0x00, 0xff));
	
	rect.x += 1;
	rect.w = 3;
	rect.h = 45;
	rect.y = e->gui.rules * (SCREEN_HEIGHT - rect.h - 4) + 2;
	SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0xff, 0xff, 0xff, 0xff));
	
	surfaceDraw(e->io.screen, e->io.rulesTitle, 0 , scroll);
	strDraw(e->io.screen, &e->io.normalFont, "Test", 60, scroll + 60);
}

void draw_guiFocusGameGUI(Env *e);

int placeValues(int x)
{
	int i = 0;
	int j = 10;

	for (i = 0; x >= j; i ++) {
		j *= 10;
	}
	
	return i;
}

void draw_guiFocusSettings(Env *e)
{
	Font *f;
	int i;

	NOT(e);
	
	if (e->gui.settings.previous == guiFocusMenu) {
		drawScrollingBackground(&e->io);
	} else {
		surfaceDraw(e->io.screen, e->io.menuBg, 0, 0);
	}
	surfaceDraw(e->io.screen, e->io.settingsTitle, 0, 0);

	drawMenuView(e->io.screen, &e->io.settingsMV);
	for (i = 0; i < settingsFocusCount; i++) {
		f = i == e->gui.settings.menu.focus ? &e->io.highlightFont : &e->io.normalFont;
		if (i == settingsFocusMusic || i == settingsFocusSfx) {
			int v;
			int ii = interval(e->io.time, 0.2f);
			v = e->gui.settings.vol[i - settingsFocusMusic];
			drawNum(e->io.screen, 183, i * e->io.normalFont.height * 2 + 78, v, f);
			
			if (i == e->gui.settings.menu.focus) {
				if (v > 0) {
					strDraw(e->io.screen, f, "<", 174 - ii - 6 * placeValues(v), i * e->io.normalFont.height * 2 + 78);
				}
				if (v < MAX_GUI_VOLUME) {
					strDraw(e->io.screen, f, ">", 190 + ii, i * e->io.normalFont.height * 2 + 78);
				}
			}
		}
	}
}

void draw_guiFocusControls(Env *e)
{
	static char *hardwareKeyText[hardwareKeyCount] =
	{
		"Start",
		"Select",
		"Up",
		"Down",
		"Left",
		"Right",
		"A",
		"B",
		"X",
		"Y",
		"L",
		"R"
	};

	static char *uknown = "?";
	static char *shuffle = "Shake";
	static char *noShuffle = "Shake N/A";
	
	int i;
	Font *f;

	NOT(e);
	
	if (e->gui.settings.previous == guiFocusMenu) {
		drawScrollingBackground(&e->io);
	} else {
		surfaceDraw(e->io.screen, e->io.menuBg, 0, 0);
	}

	surfaceDraw(e->io.screen, e->io.controlsTitle, 0, 0);

	drawMenuViewRight(e->io.screen, &e->io.controlsMV);
	
	for (i = 0; i < gameKeyCount; i++) {
		HardwareKeyType hkt;
		char *text = uknown;

		hkt =  e->io.controls.game.key[i];

		if (hkt >= 0 && hkt < hardwareKeyCount) {
			text = hardwareKeyText[hkt];
		}

		if (i == gameKeyShuffle) {
			text = e->io.accelExists ? shuffle : noShuffle;
		}

		f = &e->io.normalFont;
		
		if (e->gui.controlsMenu.menu.focus == i) {
			f = &e->io.highlightFont;
		}


		if (!changableGameKey(e->gui.controlsMenu.menu.focus)) {
			f = &e->io.normalFont;

		} else {
			if (i == e->gui.controlsMenu.menu.focus) {
				int ii = interval(e->io.time, 0.2f);
				strDraw(e->io.screen, f, "<", -ii + 157, e->io.controlsMV.pos.y + i * e->io.controlsMV.spacing.y);
				strDraw(e->io.screen, f, ">", ii + 168 + strlen(text) * (f->width + f->spacing), e->io.controlsMV.pos.y + i * e->io.controlsMV.spacing.y);
			}
		}
		
		if (e->gui.controlsMenu.dupKey[i]) {
			f = &e->io.highlightFont;
		}
		
		strDraw(e->io.screen, f, text, 166, e->io.controlsMV.pos.y + i * e->io.controlsMV.spacing.y);
	}
	
}

void draw_guiFocusGameGUI(Env *e)
{
	NOT(e);

	guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove, &e->io.controls.game); 

	
	printf("%f\n", e->gui.gameGui.lastInput);

	if (e->gui.gameGui.lastInput > 7.0f) {
		if (e->gui.transMove.type == transMoveDiscard) {
			int y;
			SDL_Rect rect;
			Coor *pos;
			
			pos = &e->gui.gameGui.boardWidget.pos;

			rect.w = (e->io.normalFont.width + e->io.normalFont.spacing) * 16 + 6;
			rect.h = e->io.normalFont.height * 5 + 6;

			rect.x = pos->x;
			rect.x += (BOARD_X * TILE_WIDTH - rect.w) / 2;

			rect.y = pos->y;
			rect.y += BOARD_Y * TILE_HEIGHT;
			rect.y -= rect.h;

			rect.x -= 2;
			rect.y -= 2;
			rect.w += 4;
			rect.h += 4;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0xe0, 0xe0, 0x00, 255));

			rect.x += 1;
			rect.y += 1;
			rect.w -= 2;
			rect.h -= 2;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0xc0, 0x80, 0x00, 255));

			rect.x += 1;
			rect.y += 1;
			rect.w -= 2;
			rect.h -= 2;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x00, 0x00, 0x00, 255));
			

			y = e->io.normalFont.height + 1;
			rect.x += 5;

			rect.y += y;
			surfaceDraw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeySelect]], rect.x, rect.y);
			rect.y += y;
			surfaceDraw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyPlay]], rect.x, rect.y);
			rect.y += y;
			surfaceDraw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyRecall]], rect.x, rect.y);
			rect.y += y;
			surfaceDraw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyMode]], rect.x, rect.y);

			rect.y -= 4 * y;
			rect.x += 18;

			strDraw(e->io.screen, &e->io.normalFont, "Move with D-Pad", rect.x - 18, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Toggle tile", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Play move", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Recall tiles", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Place mode", rect.x, rect.y);
		} else {
			/* current is transMovePlace[*] */
			int y;
			SDL_Rect rect;
			Coor *idx, *pos;
			
			idx  = &e->gui.gameGui.boardWidget.index;
			pos = &e->gui.gameGui.boardWidget.pos;

			rect.w = (e->io.normalFont.width + e->io.normalFont.spacing) * 16 + 6;
			rect.h = e->io.normalFont.height * 5 + 6;

			rect.x = pos->x;
			if (idx->x < BOARD_X / 2) {
				rect.x += BOARD_X * TILE_WIDTH;
				rect.x -= rect.w; 
			}

			rect.y = pos->y;
			if (idx->y < BOARD_Y / 2) {
				rect.y += BOARD_Y * TILE_HEIGHT;
				rect.y -= rect.h;
			} 

			rect.x -= 2;
			rect.y -= 2;
			rect.w += 4;
			rect.h += 4;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0xe0, 0xe0, 0x00, 255));

			rect.x += 1;
			rect.y += 1;
			rect.w -= 2;
			rect.h -= 2;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0xc0, 0x80, 0x00, 255));

			rect.x += 1;
			rect.y += 1;
			rect.w -= 2;
			rect.h -= 2;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x00, 0x00, 0x00, 255));

			y = e->io.normalFont.height + 1;
			rect.x += 5;

			surfaceDraw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyPlay]], rect.x, rect.y);
			rect.y += y;
			surfaceDraw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeySelect]], rect.x, rect.y);
			rect.y += y;
			surfaceDraw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyRecall]], rect.x, rect.y);
			rect.y += y;
			surfaceDraw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyCancel]], rect.x, rect.y);
			rect.y += y;
			surfaceDraw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyMode]], rect.x, rect.y);

			rect.y -= 4 * y;
			rect.x += 18;

			strDraw(e->io.screen, &e->io.normalFont, "Play move", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Place tile", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Recall tiles", rect.x, rect.y);
			rect.y += y; 
			strDraw(e->io.screen, &e->io.normalFont, "Cancel tile", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Discard mode", rect.x, rect.y);
		}
	}	
}

void draw_guiFocusGameMenu(Env *e)
{
	NOT(e);

	guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove, &e->io.controls.game); 
	surfaceDraw(e->io.screen, e->io.menuBg, 0, 0); 
	drawMenuView(e->io.screen, &e->io.gameMenuMV);

	surfaceDraw(e->io.screen, e->io.pauseTitle, 0, 0);
	/*strDraw(e->io.screen, &e->io.normalFont, "- Pause -", SCREEN_WIDTH / 2 - 24, 18);*/
}

void draw_guiFocusPlayMenu(Env *e)
{
	NOT(e);

	drawScrollingBackground(&e->io);
	drawMenuView(e->io.screen, &e->io.playMenuMV);
	surfaceDraw(e->io.screen, e->io.chooseGameTitle, 0, 0);
}

void draw_guiGameAIPause(Env *e)
{
	const char *text = "Player 2 thinking...";
	NOT(e);
		
	draw_guiFocusGameGUI(e);
	drawFader(&e->io, 196);
	strDraw(e->io.screen, &e->io.normalFont, text, (SCREEN_WIDTH - strlen(text) * (e->io.normalFont.width + e->io.normalFont.spacing)) / 2, 80);
}

void draw_guiFocusGameHotseatPause(Env *e)
{
	char text[32];

	NOT(e);


	surfaceDraw(e->io.screen, e->io.gmBack, 0, 0);
	/*guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove, &e->controls.game);  */
	drawFader(&e->io, 196);

	sprintf(text, "PLAYER %d", e->game.turn+1);
	strDraw(e->io.screen, &e->io.normalFont, text, (SCREEN_WIDTH - 7 * e->io.normalFont.width) / 2, 100);
	surfaceDraw(e->io.screen, e->io.pressStart, (320 - e->io.pressStart->w) / 2, 120);
}

void draw_guiFocusGameOver(Env *e)
{
	NOT(e);

	drawScrollingBackground(&e->io);
	surfaceDraw(e->io.screen, e->io.gmBack, 0, 0);
	/*guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove, &e->controls.game);  */
	drawFader(&e->io, 196);
	guiDrawBoard(&e->io, &e->gui.gameGui.boardWidget, &e->game, &e->gui.transMove, &e->gui.gameGui.lastMove);
	/*surfaceDraw(e->io.screen, e->io.menuBg, 0, 0);*/
	drawFader(&e->io, 196);
	strDraw(e->io.screen, &e->io.normalFont, "- Game Over -", SCREEN_WIDTH / 2 - 36, 18);
}

void draw_guiFocusGameAreYouSureQuit(Env *e)
{
/*
	const int orgX = (SCREEN_WIDTH - e->io.areYouSureQuit->w)/2;
	const int orgY = 80;
*/

	NOT(e);

	surfaceDraw(e->io.screen, e->io.menuBg, 0, 0);
	
	drawMenuView(e->io.screen, &e->io.yesNoMV);
	surfaceDraw(e->io.screen, e->io.areYouSureTitle, 0, 0);
}

void draw(Env *e)
{
	NOT(e);

	SDL_FillRect(e->io.screen, NULL, 0);
	switch (e->gui.focus) {
	case guiFocusTitle: draw_guiFocusTitle(e); break;
	case guiFocusMenu: draw_guiFocusMenu(e); break;
	case guiFocusRules: draw_guiFocusRules(e); break;
	case guiFocusSettings: draw_guiFocusSettings(e); break;
	case guiFocusControls: draw_guiFocusControls(e); break;
	case guiFocusGameGUI: draw_guiFocusGameGUI(e); break;
	case guiFocusGameMenu: draw_guiFocusGameMenu(e); break;
	case guiFocusPlayMenu: draw_guiFocusPlayMenu(e); break;
	case guiFocusGameAIPause: draw_guiGameAIPause(e); break;
	case guiFocusGameHotseatPause: draw_guiFocusGameHotseatPause(e); break;
	case guiFocusGameOver: draw_guiFocusGameOver(e); break;
	case guiFocusGameAreYouSureQuit: draw_guiFocusGameAreYouSureQuit(e); break;
	default: break;
	}
	SDL_Flip(e->io.screen);
}

