#include "widget.h"

void mkGridWidgetByDim(GridWidget *gw)
{
	int i, y, x;

	NOT(gw);

	gw->index.x = 0;
	gw->index.y = 0;
	gw->button  = memAlloc(sizeof(bool*) * gw->height);
	for (i = 0; i < gw->height; i++) {
		gw->button[i] = memAlloc(sizeof(bool) * gw->width);
	}

	for (y = 0; y < gw->height; y++) {
		for (x = 0; x < gw->width; x++) {
			gw->button[y][x] = false;
			gw->button[y][x] = false;
		}
	}
}

void mkRackWidget(GridWidget *gw)
{
	NOT(gw);
	
	gw->width = RACK_SIZE;
	gw->height = 1;
	mkGridWidgetByDim(gw);

	gw->pos.x = 180;
	gw->pos.y = 219;
}

void mkBoardWidget(GridWidget *gw)
{
	NOT(gw);
	
	gw->width  = BOARD_X;
	gw->height = BOARD_Y;
	mkGridWidgetByDim(gw);

	gw->pos.x = 132;
	gw->pos.y = 29;
}

void boardWidgetControls(Cmd *cmd, GameGUI *gg, Controls *c)
{
	GridWidget *bw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == gameGUIFocusBoard);

	bw = &gg->boardWidget;
	cmd->type = cmdInvalid;

	if (isPressedHeld(c, gameKeyPrevTile)) {
		cmd->type = cmdTilePrev;
		return;
	}
	if (isPressedHeld(c, gameKeyNextTile)) {
		cmd->type = cmdTileNext;
		return;
	}
	if (isPressed(c, gameKeyMode)) {
		cmd->type = cmdMode;
		return;
	}
	if (isPressedHeld(c, gameKeyPlay)) {
		cmd->type = cmdPlay;
		return;
	}
	if (isPressed(c, gameKeySelect) && bw->button[bw->index.y][bw->index.x]) {
		cmd->type = cmdBoardSelect;
		cmd->data.board = bw->index;
		return;
	}
	if (isPressedHeld(c, gameKeyUp)) {
		if (bw->index.y == 0 && bw->index.x >= 4 && bw->index.x <= 10) {
			cmd->type = cmdRack;
			cmd->data.rack = bw->index.x - 4;
		} else {
			cmd->type = cmdBoardUp;
		}
		return;
	}
	if (isPressedHeld(c, gameKeyDown)) {
		if (bw->index.y == (BOARD_Y - 1) && bw->index.x >= 4 && bw->index.x <= 10) {
			cmd->type = cmdRack;
			cmd->data.rack = bw->index.x - 4;
		} else {
			cmd->type = cmdBoardDown;
		}
		return;
	}
	if (isPressedHeld(c, gameKeyLeft)) {
		cmd->type = cmdBoardLeft;
		return;
	}
	if (isPressedHeld(c, gameKeyRight)) {
		cmd->type = cmdBoardRight;
		return;
	}
	if (isPressed(c, gameKeyCancel)) {
		cmd->type = cmdBoardCancel;
		cmd->data.board = bw->index;
		return;
	}
	if (isPressedHeld(c, gameKeyRecall)) {
		cmd->type = cmdRecall;
		return;
	}
}

void rackWidgetControls(Cmd *cmd, GameGUI *gg, Controls *c)
{
	GridWidget *rw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == gameGUIFocusRack);
	
	rw = &gg->rackWidget;
	rw->index.y = 0;
	cmd->type = cmdInvalid;

	if (isPressedHeld(c, gameKeyPrevTile)) {
		cmd->type = cmdTilePrev;
		return;
	}
	if (isPressedHeld(c, gameKeyNextTile)) {
		cmd->type = cmdTileNext;
		return;
	}
	if (isPressed(c, gameKeySelect) && rw->button[rw->index.y][rw->index.x]) {
		cmd->type = cmdRackSelect;
		cmd->data.rack = rw->index.x;
		return;
	}
	if (isPressed(c, gameKeyRecall)) {
		cmd->type = cmdRecall;
		return;
	}
	if (isPressed(c, gameKeyPlay)) {
		cmd->type = cmdPlay;
		return;
	}
	if (isPressed(c, gameKeyMode)) {
		cmd->type = cmdMode;
		return;
	}
	if (isPressed(c, gameKeyLeft)) {
		assert(rw->index.x >= 0);
		if (rw->index.x == 0) {
			cmd->type = cmdRack;
			cmd->data.rack = RACK_SIZE - 1;
		} else {
			cmd->type = cmdRackLeft;
		}
		return;
	}
	if (isPressed(c, gameKeyRight)) {
		assert(rw->index.x <=  RACK_SIZE);
		if (rw->index.x + 1 == RACK_SIZE) {
			cmd->type = cmdRack;
			cmd->data.rack = 0;
		} else {
			cmd->type = cmdRackRight;
		}
		return;
	}
	if (isPressedHeld(c, gameKeyDown)) {
		cmd->type = cmdBoard;
		cmd->data.board.x = rw->index.x + 4;
		cmd->data.board.y = 0;
		return;
	}
	if (isPressedHeld(c, gameKeyUp)) {
		cmd->type = cmdBoard;
		cmd->data.board.x = rw->index.x + 4;
		cmd->data.board.y = BOARD_Y - 1;
		return;
	}
	if (isPressed(c, gameKeyCancel)) {
		cmd->type = cmdRackCancel;
		cmd->data.rack = rw->index.x;
	}
}

void updateBoardWidget(GridWidget *bw, TransMove *tm, board_t *b)
{
coor_t idx;

	NOT(bw);
	NOT(tm);
	NOT(b);
	
	switch (tm->type) {
	case transMovePlace: {
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				bw->button[idx.y][idx.x] = b->tile[idx.y][idx.x].type == TILE_NONE;
			}
		}
		break;
	}
	case transMovePlaceWild: {
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				bw->button[idx.y][idx.x] = idx.x == bw->index.x && idx.y == bw->index.y;
			}
		}
		break;
	}
	case transMovePlaceEnd: {
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				bw->button[idx.y][idx.x] = false;
			}
		}
		break;
	}
	case transMoveDiscard: {
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				bw->button[idx.y][idx.x] = false;
			}
		}
		break;
	}
	case transMoveSkip: {
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				bw->button[idx.y][idx.x] = false;
			}
		}
		break;
	}
	default: break;
	}
}

void updateRackWidget(GridWidget *rw, TransMove *tm)
{
coor_t idx;
	tile_tag_t tt;

	NOT(rw);
	NOT(tm);
	assert(tm->adjust.type == ADJUST_RACK);

	idx.y = 0;
	switch (tm->type) {
	case transMovePlace: {
		for (idx.x = 0; idx.x < RACK_SIZE; idx.x++) {
			rw->button[idx.y][idx.x] = true; 
		}	
		break;
	}
	case transMovePlaceEnd: {
		for (idx.x = 0; idx.x < RACK_SIZE; idx.x++) {
			rw->button[idx.y][idx.x] = false; 
		}	
		break;
	}
	case transMoveDiscard: {
		for (idx.x = 0; idx.x < RACK_SIZE; idx.x++) {
			tt = tm->adjust.data.tile[idx.x].type;
			assert(tt == TILE_NONE || tt == TILE_WILD || tt == TILE_LETTER);
			rw->button[idx.y][idx.x] = tt != TILE_NONE; 
		}
		break;
	}
	case transMoveSkip: {
		for (idx.x = 0; idx.x < RACK_SIZE; idx.x++) {
			rw->button[idx.y][idx.x] = false;
		}
	}
	default: break;
	}
}

void updateGameGUIViaCmd(GameGUI *gg, Cmd *c, TransMoveType tmt)
{
	GridWidget *rw, *bw;

	NOT(gg);
	NOT(c);

	rw = &gg->rackWidget;
	bw = &gg->boardWidget;

	switch (gg->focus) {
	case gameGUIFocusBoard: {
		if (tmt == transMovePlaceWild) {
			break;
		}
		switch (c->type) {
		case cmdBoardUp: {
			bw->index.y += BOARD_Y;
			bw->index.y--;
			bw->index.y %= BOARD_Y;
			break;
		}
		case cmdBoardDown: {
			bw->index.y++;
			bw->index.y %= BOARD_Y;
			break;
		}
		case cmdBoardLeft: {
			bw->index.x += BOARD_X;
			bw->index.x--;
			bw->index.x %= BOARD_X;
			break;
		}
		case cmdBoardRight: {
			bw->index.x++;
			bw->index.x %= BOARD_X;
			break;
		}
		default: break;
		}
		break;
	}
	case gameGUIFocusRack: {
		switch (c->type) {
		case cmdRackLeft: {
			rw->index.x += RACK_SIZE;
			rw->index.x--;
			rw->index.x %= RACK_SIZE;
			break;
		}
		case cmdRackRight: {
			rw->index.x++;
			rw->index.x %= RACK_SIZE;
			break;
		}
		default: break;
		}
		break;
	}
	default: break;
	}
}

void boardWidgetDraw(IO *io, GridWidget *bw, player_t *p, board_t *b, TransMove *tm, LastMove *lm, coor_t dim)
{
	tile_t *t;
	SDL_Surface *ts;
coor_t idx;
	TileLookType tlt;
	int i;

	NOT(io);
	NOT(bw);
	NOT(p);
	NOT(b);
	NOT(tm);

	for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
		for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
			surface_draw(io->screen, io->sq[b->sq[idx.y][idx.x]], idx.x * dim.x + bw->pos.x, idx.y * dim.y + bw->pos.y);
			t = &b->tile[idx.y][idx.x];
			if (t->type != TILE_NONE) {
				tlt = lm->type == lastMovePlace && lm->data.place[idx.y][idx.x] ? tileLookLast : tileLookNormal;
				ts = io->tile[t->type][t->letter][tlt];
				surface_draw(io->screen, ts, idx.x * dim.x + bw->pos.x, idx.y * dim.y + bw->pos.y);
			}
		}
	}

	switch (tm->type) {
	case transMovePlace:
	case transMovePlaceWild:
	case transMovePlacePlay:
	case transMovePlaceEnd: {
		int j;
		for (i = 0; i < RACK_SIZE; i++) {
			j = tm->adjust.data.tile[i].idx;
			idx = tm->place.boardIdx[j];
			if (valid_board_idx(idx)) {
				t = &p->tile[tm->adjust.data.tile[tm->place.rackIdx[idx.y][idx.x]].idx];
				ts = io->tile[t->type][t->letter][tileLookHold];
				surface_draw(io->screen, ts, idx.x * dim.x + bw->pos.x, idx.y * dim.y + bw->pos.y);
			}
		}
		break;
	}
	default: break;
	}

}

void boardWidgetDrawWithoutTransMove(IO *io, GridWidget *bw, board_t *b, LastMove *lm, coor_t dim)
{
	tile_t *t;
	SDL_Surface *ts;
coor_t idx;
	TileLookType tlt;

	NOT(io);
	NOT(bw);
	NOT(b);

	for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
		for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
			surface_draw(io->screen, io->sq[b->sq[idx.y][idx.x]], idx.x * dim.x + bw->pos.x, idx.y * dim.y + bw->pos.y);
			t = &b->tile[idx.y][idx.x];
			if (t->type != TILE_NONE) {
				tlt = lm->type == lastMovePlace && lm->data.place[idx.y][idx.x] ? tileLookLast : tileLookNormal;
				ts = io->tile[t->type][t->letter][tlt];
				surface_draw(io->screen, ts, idx.x * dim.x + bw->pos.x, idx.y * dim.y + bw->pos.y);
			}
		}
	}

}

void rackWidgetDraw(IO *io, TransMove *tm, GridWidget *rw, coor_t dim,
		    player_t *p)
{
	tile_t *t;
	tile_tag_t tt;
	SDL_Surface *s;

	NOT(io);
	NOT(tm);
	NOT(p);

	switch (tm->type) {
	case transMovePlace:
	case transMovePlacePlay:
	case transMovePlaceWild:
		for (int i = 0; i < RACK_SIZE; i++) {
			t = &p->tile[tm->adjust.data.tile[i].idx];
			if (t->type == TILE_NONE)
				continue;
			s = NULL;
			if (tm->place.idx != i) {
				if (!valid_board_idx(tm->place.boardIdx[i])) {
					tt = tileLookNormal;
					s = t->type == TILE_WILD
						? io->wild[tt]
						: io->tile[t->type][t->letter]
						          [tt];
				}
			} else {
				tt = tileLookHold;
				s = t->type == TILE_WILD
					? io->wild[tt]
					: io->tile[t->type][t->letter][tt];
			}
			if (s)
				surface_draw(io->screen, s,
					     i * dim.x + rw->pos.x, rw->pos.y);
		}
		break;
	case transMovePlaceEnd:
		break;
	case transMoveDiscard:
		for (int i = 0; i < RACK_SIZE; i++) {
			t = &p->tile[tm->adjust.data.tile[i].idx];
			if (t->type == TILE_NONE)
				continue;
			tt = tm->discard.rack[i]
				? tileLookDisable : tileLookNormal;
			s = t->type == TILE_WILD
				? io->wild[tt]
				: io->tile[TILE_LETTER][t->letter][tt];
			surface_draw(io->screen, s, i * dim.x + rw->pos.x,
				     rw->pos.y);
		}
		break;
	default:
		for (int i = 0; i < RACK_SIZE; i++) {
			t = &p->tile[tm->adjust.data.tile[i].idx];
			if (t->type == TILE_NONE)
				continue;
			s = t->type == TILE_WILD
				? io->wild[tileLookNormal]
				: io->tile[TILE_LETTER][t->letter]
				          [tileLookNormal];
			surface_draw(io->screen, s, i * dim.x + rw->pos.x,
				     rw->pos.y);
		}
		break;
	}
}

void gridWidgetDraw(SDL_Surface *s, GridWidget *gw, coor_t pos, coor_t dim)
{
	int y, x;
	SDL_Rect clip;
	Uint32 red, green, color;
	
	NOT(gw);

	clip.w = dim.x - 1;
	clip.h = dim.y - 1;
	red   = SDL_MapRGB(s->format, 255, 0, 0);
	green = SDL_MapRGB(s->format, 0, 255, 0);
	for (y = 0; y < gw->height; y++) {
		for (x = 0; x < gw->width; x++) {
			color  = gw->button[y][x] ? green : red;
			clip.x = pos.x + dim.x * x;
			clip.y = pos.y + dim.y * y;
			SDL_FillRect(s, &clip, color);
		}
	}
}

