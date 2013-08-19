#include <math.h>

#include "gui.h"
#include "draw.h"
#include "widget.h"

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
		if (c == '\n') {
			offset.x = x;
			offset.y += f->height;
			continue;
		}
		/* [32..126] are drawable ASCII chars */
		if (c >= 32 && c <= 126) {
			clip.x = f->width * (c - 32);
			SDL_BlitSurface(f->map, &clip, s, &offset);
		}
		offset.x += f->width + f->spacing;

	}
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

void drawGameAreYouSureQuit(Env *e)
{
	const int orgX = 150 - 8;
	const int orgY = 80;

	NOT(e);

	drawScrollingBackground(e);
	drawScoreBoard(&e->gui.scoreBoard, &e->io);
	
	surfaceDraw(e->io.screen, e->io.back, 0, 0);
	guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove); 
	SDL_FillRect(e->io.fader, 0, SDL_MapRGBA(e->io.fader->format, 0, 0, 0, 218));
	surfaceDraw(e->io.screen, e->io.fader, 0, 0);
	surfaceDraw(e->io.screen, e->io.areYouSureQuit, orgX - e->io.whiteFont.width * 11, orgY - e->io.whiteFont.height);
	drawMenuView(e->io.screen, &e->io.yesNoMV);
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
				drawNum(e->io.screen, 183, i * e->io.normalFont.height * 2 + 65, v, f);
				if (i == SETTINGS_FOCUS_MUSIC) {
				} else {
					assert(i == SETTINGS_FOCUS_SFX);
				}
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
		drawScoreBoard(&e->gui.scoreBoard, &e->io);
		surfaceDraw(e->io.screen, e->io.back, 0, 0);
		guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove); 
		SDL_FillRect(e->io.fader, 0, SDL_MapRGBA(e->io.fader->format, 0, 0, 0, 196));
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
		SDL_FillRect(e->io.fader, 0, SDL_MapRGBA(e->io.fader->format, 0, 0, 0, 196));
		SDL_SetAlpha(e->io.fader, SDL_SRCALPHA, 196);
		surfaceDraw(e->io.screen, e->io.fader, 0, 0);
		break;
	}
	case GUI_FOCUS_GAME_HOTSEAT_PAUSE: {
		drawScrollingBackground(e);
		surfaceDraw(e->io.screen, e->io.back, 0, 0);
		guiDrawBoard(&e->io, &e->gui.gameGui.boardWidget, &e->game, &e->gui.transMove);
		drawScoreBoard(&e->gui.scoreBoard, &e->io);
		SDL_FillRect(e->io.fader, 0, SDL_MapRGBA(e->io.fader->format, 0, 0, 0, 196));
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

