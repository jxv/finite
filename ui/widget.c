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
}

void mkChoiceWidget(GridWidget *gw)
{
	NOT(gw);
	
	gw->width = choiceCount;
	gw->height = 1;
	mkGridWidgetByDim(gw);
}

void mkBoardWidget(GridWidget *gw)
{
	NOT(gw);
	
	gw->width  = BOARD_X;
	gw->height = BOARD_Y;
	mkGridWidgetByDim(gw);
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

	if (c->l.type == keyStatePressed) {
		cmd->type = cmdTilePrev;
		return;
	}
	if (c->r.type == keyStatePressed) {
		cmd->type = cmdTileNext;
		return;
	}
	if (c->b.type == keyStatePressed) {
		cmd->type = cmdFocusBottom;
		return;
	}
	if (c->select.type == keyStatePressed) {
		cmd->type = cmdPlay;
		return;
	}
	if (c->a.type == keyStatePressed && bw->button[bw->index.y][bw->index.x]) {
		cmd->type = cmdBoardSelect;
		cmd->data.board = bw->index;
		return;
	}
	if (c->up.type == keyStatePressed) {
		cmd->type = cmdBoardUp;
		return;
	}
	if (c->down.type == keyStatePressed) {
		cmd->type = cmdBoardDown;
		return;
	}
	if (c->left.type == keyStatePressed) {
		cmd->type = cmdBoardLeft;
		return;
	}
	if (c->right.type == keyStatePressed) {
		cmd->type = cmdBoardRight;
		return;
	}
	if (c->x.type == keyStatePressed) {
		cmd->type = cmdBoardCancel;
		cmd->data.board = bw->index;
	}
	if (c->y.type == keyStatePressed) {
		cmd->type = cmdModeToggle;
		return;
	}
}

void choiceWidgetControls(Cmd *cmd, GameGUI *gg, Controls *c)
{
	GridWidget *cw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == gameGUIFocusChoice);

	cw = &gg->choiceWidget;
	cw->index.y = 0;
	cmd->type = cmdInvalid;

	if (c->l.type == keyStatePressed) {
		cmd->type = cmdTilePrev;
		return;
	}
	if (c->r.type == keyStatePressed) {
		cmd->type = cmdTileNext;
		return;
	}
	if (c->b.type == keyStatePressed) {
		cmd->type = cmdFocusTop;
		/*cmd->type = cmdRecall;*/
		return;
	}
	if (c->select.type == keyStatePressed) {
		cmd->type = cmdPlay;
		return;
	}
	if (c->y.type == keyStatePressed) {
		cmd->type = cmdModeToggle;
		return;
	}
	if (cw->button[cw->index.y][cw->index.x]) {
		if (c->a.type == keyStatePressed) {
			switch (cw->index.x) {
			case choiceRecall: cmd->type = cmdRecall; return;
			case choicePlay: cmd->type = cmdPlay; return;
			case choiceShuffle: cmd->type = cmdShuffle; return;
			case choiceMode: /* fall through */
			default: break;
			}
		}
		if (c->up.type == keyStatePressed) {
			switch (cw->index.x) {
			case choiceMode: cmd->type = cmdModeUp; return;
			case choiceRecall: /* fall through */
			case choicePlay:
			default: break;
			}
		}
		if (c->down.type == keyStatePressed) {
			switch (cw->index.x) {
			case choiceMode: cmd->type = cmdModeDown; return;
			case choiceRecall: /* fall through */
			case choicePlay:
			default: break;
			}
		}
	}
	if (c->left.type == keyStatePressed) {
		assert(cw->index.x >= 0);
		if (cw->index.x == 0) {
			cmd->type = cmdRack;
			cmd->data.rack = RACK_SIZE - 1;
		} else {
			cmd->type = cmdChoiceLeft;
		}
		return;
	}
	if (c->right.type == keyStatePressed) {
		assert(cw->index.x < choiceCount);
		if (cw->index.x + 1 == choiceCount) {
			cmd->type = cmdRack;
			cmd->data.rack = 0;
		} else {
			cmd->type = cmdChoiceRight;
		}
		return;
	}
	if (c->x.type == keyStatePressed) {
		cmd->type = cmdChoiceCancel;
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

	if (c->l.type == keyStatePressed) {
		cmd->type = cmdTilePrev;
		return;
	}
	if (c->r.type == keyStatePressed) {
		cmd->type = cmdTileNext;
		return;
	}
	if (c->a.type == keyStatePressed && rw->button[rw->index.y][rw->index.x]) {
		cmd->type = cmdRackSelect;
		cmd->data.rack = rw->index.x;
		return;
	}
	if (c->b.type == keyStatePressed) {
		cmd->type = cmdFocusTop;
		/* cmd->type = cmdRecall; */
		return;
	}
	if (c->select.type == keyStatePressed) {
		cmd->type = cmdPlay;
		return;
	}
	if (c->y.type == keyStatePressed) {
		cmd->type = cmdModeToggle;
		return;
	}
	if (c->left.type == keyStatePressed) {
		assert(rw->index.x >= 0);
		if (rw->index.x == 0) {
			cmd->type = cmdChoice;
			cmd->data.choice = choiceCount - 1;
		} else {
			cmd->type = cmdRackLeft;
		}
		return;
	}
	if (c->right.type == keyStatePressed) {
		assert(rw->index.x <=  RACK_SIZE);
		if (rw->index.x + 1 == RACK_SIZE) {
			cmd->type = cmdChoice;
			cmd->data.choice = 0;
		} else {
			cmd->type = cmdRackRight;
		}
		return;
	}
	if (c->x.type == keyStatePressed) {
		cmd->type = cmdRackCancel;
		cmd->data.rack = rw->index.x;
	}
}

void updateBoardWidget(GridWidget *bw, TransMove *tm, Board *b)
{
	Coor idx;

	NOT(bw);
	NOT(tm);
	NOT(b);
	
	switch (tm->type) {
	case transMovePlace: {
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				bw->button[idx.y][idx.x] = b->tile[idx.y][idx.x].type == tileNone;
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
	Coor idx;
	TileType tt;

	NOT(rw);
	NOT(tm);
	assert(tm->adjust.type == adjustRack);

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
			assert(tt == tileNone || tt == tileWild || tt == tileLetter);
			rw->button[idx.y][idx.x] = tt != tileNone; 
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

void updateChoiceWidget(GridWidget *cw, TransMove *tm)
{
	NOT(cw);
	NOT(tm);
	
	switch (tm->type) {
	case transMovePlace: {
		cw->button[0][choiceRecall] = tm->place.num > 0;
		cw->button[0][choiceMode] = true;
		cw->button[0][choicePlay] = true;
		cw->button[0][choiceShuffle] = true;
		break;
	}
	case transMovePlaceEnd: {
		cw->button[0][choiceRecall] = true;
		cw->button[0][choiceMode] = true;
		cw->button[0][choicePlay] = true; 
		cw->button[0][choiceShuffle] = false;
		break;
	}
	case transMoveDiscard: {
		cw->button[0][choiceRecall] = tm->discard.num > 0;
		cw->button[0][choiceMode] = true;
		cw->button[0][choicePlay] = true;
		cw->button[0][choiceShuffle] = true;
		break;
	}
	case transMoveSkip: {
		cw->button[0][choiceRecall] = false;
		cw->button[0][choiceMode] = true;
		cw->button[0][choicePlay] = true;
		cw->button[0][choiceShuffle] = true;
		break;
	}
	default: break;
	}

}

void updateGameGUIViaCmd(GameGUI *gg, Cmd *c, TransMoveType tmt)
{
	GridWidget *rw, *bw, *cw;

	NOT(gg);
	NOT(c);

	rw = &gg->rackWidget;
	bw = &gg->boardWidget;
	cw = &gg->choiceWidget;

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
	case gameGUIFocusChoice: {
		switch (c->type) {
		case cmdChoiceLeft: {
			cw->index.x += choiceCount;
			cw->index.x--;
			cw->index.x %= choiceCount;
			break;
		}
		case cmdChoiceRight: {
			cw->index.x++;
			cw->index.x %= choiceCount;
			break;
		}
		default: break;
		}
		break;
	}
	default: break;
	}
}

void boardWidgetDraw(IO *io, GridWidget *bw, Player *p, Board *b, TransMove *tm, Coor pos, Coor dim)
{
	Tile *t;
	SDL_Surface *ts;
	Coor idx;
	int i;

	NOT(io);
	NOT(bw);
	NOT(p);
	NOT(b);
	NOT(tm);

	for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
		for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
			surfaceDraw(io->screen, io->sq[b->sq[idx.y][idx.x]], idx.x * 14 + 106, idx.y * 14 + 6);
			t = &b->tile[idx.y][idx.x];
			if (t->type != tileNone) {
				ts = io->tile[t->type][t->letter][tileLookNormal];
				surfaceDraw(io->screen, ts, idx.x * dim.x + pos.x, idx.y * dim.y + pos.y);
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
			if (validBoardIdx(idx)) {
				t = &p->tile[tm->adjust.data.tile[tm->place.rackIdx[idx.y][idx.x]].idx];
				ts = io->tile[t->type][t->letter][tileLookHold];
				surfaceDraw(io->screen, ts, idx.x * dim.x + pos.x, idx.y * dim.y + pos.y);
			}
		}
		break;
	}
	default: break;
	}
}

void rackWidgetDraw(IO *io, TransMove *tm, GridWidget *rw, Coor pos, Coor dim, Player *p)
{
	int i, offset;
	Tile *t;
	TileType tt;
	SDL_Surface *s;

	NOT(io);
	NOT(tm);
	NOT(p);

	offset = 176;

	switch (tm->type) {
	case transMovePlace:
	case transMovePlacePlay:
	case transMovePlaceWild: {
		for (i = 0; i < RACK_SIZE; i++) {
			t = &p->tile[tm->adjust.data.tile[i].idx];
			if (t->type == tileNone) {
				continue;
			}
			s = NULL;
			if (tm->place.idx != i) {
				if (!validBoardIdx(tm->place.boardIdx[i])) {
					tt = tileLookNormal;
					s = t->type == tileWild ? io->wild[tt] : io->tile[t->type][t->letter][tt];
				}
			} else {
				tt = tileLookHold;
				s = t->type == tileWild ? io->wild[tt] : io->tile[t->type][t->letter][tt];
			}
			if (s) {
				surfaceDraw(io->screen, s, i * dim.x + offset, 220);
			}
		}
		break;
	}
	case transMovePlaceEnd: {
		break;
	}
	case transMoveDiscard: {
		for (i = 0; i < RACK_SIZE; i++) {
			t = &p->tile[tm->adjust.data.tile[i].idx];
			if (t->type == tileNone) {
				continue;
			}
			VALID_TILE(*t);
			tt = tm->discard.rack[i] ? tileLookDisable : tileLookNormal;
			s = t->type == tileWild ? io->wild[tt] : io->tile[tileLetter][t->letter][tt];
			surfaceDraw(io->screen, s, i * dim.x + offset, 220);
			
		}
		break;
	}
	default: {
		for (i = 0; i < RACK_SIZE; i++) {
			t = &p->tile[tm->adjust.data.tile[i].idx];
			if (t->type == tileNone) {
				continue;
			}
			s = t->type == tileWild ? io->wild[tileLookNormal] : io->tile[tileLetter][t->letter][tileLookNormal];
			surfaceDraw(io->screen, s, i * dim.x + offset, 220);
		}
		break;
	}
	}
}

void choiceWidgetDraw(IO *io, TransMove *tm, GridWidget *cw, Coor pos, Coor dim)
{
	bool mode, recall, play, shuffle;
	ModeType type;

	NOT(io);
	NOT(tm);
	NOT(cw);

	type = modeInvalid; 
	recall = cw->button[0][choiceRecall];
	mode = cw->button[0][choiceMode]; 
	play = cw->button[0][choicePlay];
	shuffle = cw->button[0][choiceShuffle];

	switch (tm->type) {
	case transMovePlace:
	case transMovePlaceWild:
	case transMovePlacePlay:
	case transMovePlaceEnd: type = modePlace; break;
	case transMoveDiscard: type = modeDiscard; break;
	case transMoveSkip: type = modeSkip; break;
	default: break;
	}

	if (mode) {
		surfaceDraw(io->screen, io->mode, 119, 217);
		switch (type) {
		case modePlace: surfaceDraw(io->screen, io->place, 119, 220); break;
		case modeDiscard: surfaceDraw(io->screen, io->discard, 119, 220); break;
		case modeSkip: surfaceDraw(io->screen, io->skip, 119, 220); break;
		default: break;
		}
	} else {
		surfaceDraw(io->screen, io->modeDisable, 119, 217);
		switch (type) {
		case modePlace: surfaceDraw(io->screen, io->placeDisable, 119, 220); break;
		case modeDiscard: surfaceDraw(io->screen, io->discardDisable, 119, 220); break;
		case modeSkip:	surfaceDraw(io->screen, io->skip, 119, 220); break;
		default: break;
		}
	}
	surfaceDraw(io->screen, recall ? io->recall : io->recallDisable, 105, 220);
	surfaceDraw(io->screen, play ? io->play : io->playDisable, 133, 220);
	surfaceDraw(io->screen, shuffle ? io->shuffle : io->shuffleDisable, 147, 220);
}

void gridWidgetDraw(SDL_Surface *s, GridWidget *gw, Coor pos, Coor dim)
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

