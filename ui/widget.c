#include "widget.h"

void mkGridWidgetByDim(struct GridWidget *gw)
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

void mkRackWidget(struct GridWidget *gw)
{
	NOT(gw);
	
	gw->width = RACK_SIZE;
	gw->height = 1;
	mkGridWidgetByDim(gw);
}

void mkChoiceWidget(struct GridWidget *gw)
{
	NOT(gw);
	
	gw->width = CHOICE_COUNT;
	gw->height = 1;
	mkGridWidgetByDim(gw);
}

void mkBoardWidget(struct GridWidget *gw)
{
	NOT(gw);
	
	gw->width  = BOARD_X;
	gw->height = BOARD_Y;
	mkGridWidgetByDim(gw);
}

void boardWidgetControls(struct Cmd *cmd, struct GameGUI *gg, struct Controls *c)
{
	struct GridWidget *bw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == GUI_FOCUS_BOARD);

	bw = &gg->boardWidget;
	cmd->type = CMD_INVALID;

	if (c->l.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_TILE_PREV;
		return;
	}
	if (c->r.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_TILE_NEXT;
		return;
	}
	if (c->b.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_FOCUS_BOTTOM;
		return;
	}
	if (c->a.type == KEY_STATE_PRESSED && bw->button[bw->index.y][bw->index.x]) {
		cmd->type = CMD_BOARD_SELECT;
		cmd->data.board = bw->index;
		return;
	}
	if (c->up.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_BOARD_UP;
		return;
	}
	if (c->down.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_BOARD_DOWN;
		return;
	}
	if (c->left.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_BOARD_LEFT;
		return;
	}
	if (c->right.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_BOARD_RIGHT;
		return;
	}
	if (c->x.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_BOARD_CANCEL;
		cmd->data.board = bw->index;
	}
}

void choiceWidgetControls(struct Cmd *cmd, struct GameGUI *gg, struct Controls *c)
{
	struct GridWidget *cw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == GUI_FOCUS_CHOICE);

	cw = &gg->choiceWidget;
	cw->index.y = 0;
	cmd->type = CMD_INVALID;

	if (c->l.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_TILE_PREV;
		return;
	}
	if (c->r.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_TILE_NEXT;
		return;
	}
	if (c->b.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_FOCUS_TOP;
		return;
	}
	if (cw->button[cw->index.y][cw->index.x]) {
		if (c->a.type == KEY_STATE_PRESSED) {
			switch (cw->index.x) {
			case CHOICE_RECALL: cmd->type = CMD_RECALL; return;
			case CHOICE_PLAY: cmd->type = CMD_PLAY; return;
			case CHOICE_MODE: /* fall through */
			default: break;
			}
		}
		if (c->up.type == KEY_STATE_PRESSED) {
			switch (cw->index.x) {
			case CHOICE_MODE: cmd->type = CMD_MODE_UP; return;
			case CHOICE_RECALL: /* fall through */
			case CHOICE_PLAY:
			default: break;
			}
		}
		if (c->down.type == KEY_STATE_PRESSED) {
			switch (cw->index.x) {
			case CHOICE_MODE: cmd->type = CMD_MODE_DOWN; return;
			case CHOICE_RECALL: /* fall through */
			case CHOICE_PLAY:
			default: break;
			}
		}
	}
	if (c->left.type == KEY_STATE_PRESSED) {
		assert(cw->index.x >= 0);
		if (cw->index.x == 0) {
			cmd->type = CMD_RACK;
			cmd->data.rack = RACK_SIZE - 1;
		} else {
			cmd->type = CMD_CHOICE_LEFT;
		}
		return;
	}
	if (c->right.type == KEY_STATE_PRESSED) {
		assert(cw->index.x < CHOICE_COUNT);
		if (cw->index.x + 1 == CHOICE_COUNT) {
			cmd->type = CMD_RACK;
			cmd->data.rack = 0;
		} else {
			cmd->type = CMD_CHOICE_RIGHT;
		}
		return;
	}
	if (c->x.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_CHOICE_CANCEL;
	}
}

void rackWidgetControls(struct Cmd *cmd, struct GameGUI *gg, struct Controls *c)
{
	struct GridWidget *rw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == GUI_FOCUS_RACK);
	
	rw = &gg->rackWidget;
	rw->index.y = 0;
	cmd->type = CMD_INVALID;

	if (c->l.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_TILE_PREV;
		return;
	}
	if (c->r.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_TILE_NEXT;
		return;
	}
	if (c->a.type == KEY_STATE_PRESSED && rw->button[rw->index.y][rw->index.x]) {
		cmd->type = CMD_RACK_SELECT;
		cmd->data.rack = rw->index.x;
		return;
	}
	if (c->b.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_FOCUS_TOP;
		return;
	}
	if (c->left.type == KEY_STATE_PRESSED) {
		assert(rw->index.x >= 0);
		if (rw->index.x == 0) {
			cmd->type = CMD_CHOICE;
			cmd->data.choice = CHOICE_COUNT - 1;
		} else {
			cmd->type = CMD_RACK_LEFT;
		}
		return;
	}
	if (c->right.type == KEY_STATE_PRESSED) {
		assert(rw->index.x <=  RACK_SIZE);
		if (rw->index.x + 1 == RACK_SIZE) {
			cmd->type = CMD_CHOICE;
			cmd->data.choice = 0;
		} else {
			cmd->type = CMD_RACK_RIGHT;
		}
		return;
	}
	if (c->x.type == KEY_STATE_PRESSED) {
		cmd->type = CMD_RACK_CANCEL;
		cmd->data.rack = rw->index.x;
	}
}

void updateBoardWidget(struct GridWidget *bw, struct TransMove *tm, struct Board *b)
{
	struct Coor idx;

	NOT(bw);
	NOT(tm);
	NOT(b);
	
	switch (tm->type) {
	case TRANS_MOVE_PLACE: {
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				bw->button[idx.y][idx.x] = b->tile[idx.y][idx.x].type == TILE_NONE;
			}
		}
		break;
	}
	case TRANS_MOVE_PLACE_WILD: {
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				bw->button[idx.y][idx.x] = idx.x == bw->index.x && idx.y == bw->index.y;
			}
		}
		break;
	}
	case TRANS_MOVE_PLACE_END: {
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				bw->button[idx.y][idx.x] = false;
			}
		}
		break;
	}
	case TRANS_MOVE_DISCARD: {
		for (idx.y = 0; idx.y < BOARD_Y; idx.y++) {
			for (idx.x = 0; idx.x < BOARD_X; idx.x++) {
				bw->button[idx.y][idx.x] = false;
			}
		}
		break;
	}
	case TRANS_MOVE_SKIP: {
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

void updateRackWidget(struct GridWidget *rw, struct TransMove *tm)
{
	struct Coor idx;
	TileType tt;

	NOT(rw);
	NOT(tm);
	assert(tm->adjust.type == ADJUST_RACK);

	idx.y = 0;
	switch (tm->type) {
	case TRANS_MOVE_PLACE: {
		for (idx.x = 0; idx.x < RACK_SIZE; idx.x++) {
			rw->button[idx.y][idx.x] = true; 
		}	
		break;
	}
	case TRANS_MOVE_PLACE_END: {
		for (idx.x = 0; idx.x < RACK_SIZE; idx.x++) {
			rw->button[idx.y][idx.x] = false; 
		}	
		break;
	}
	case TRANS_MOVE_DISCARD: {
		for (idx.x = 0; idx.x < RACK_SIZE; idx.x++) {
			tt = tm->adjust.data.tile[idx.x].type;
			assert(tt == TILE_NONE || tt == TILE_WILD || tt == TILE_LETTER);
			rw->button[idx.y][idx.x] = tt != TILE_NONE; 
		}
		break;
	}
	case TRANS_MOVE_SKIP: {
		for (idx.x = 0; idx.x < RACK_SIZE; idx.x++) {
			rw->button[idx.y][idx.x] = false;
		}
	}
	default: break;
	}
}

void updateChoiceWidget(struct GridWidget *cw, struct TransMove *tm)
{
	NOT(cw);
	NOT(tm);
	
	switch (tm->type) {
	case TRANS_MOVE_PLACE: {
		cw->button[0][CHOICE_RECALL] = tm->data.place.num > 0;
		cw->button[0][CHOICE_MODE] = true;
		cw->button[0][CHOICE_PLAY] = true;
		break;
	}
	case TRANS_MOVE_PLACE_END: {
		cw->button[0][CHOICE_RECALL] = true;
		cw->button[0][CHOICE_MODE] = false;
		cw->button[0][CHOICE_PLAY] = true; 
		break;
	}
	case TRANS_MOVE_DISCARD: {
		cw->button[0][CHOICE_RECALL] = tm->data.discard.num > 0;
		cw->button[0][CHOICE_MODE] = true;
		cw->button[0][CHOICE_PLAY] = true;
		break;
	}
	case TRANS_MOVE_SKIP: {
		cw->button[0][CHOICE_RECALL] = false;
		cw->button[0][CHOICE_MODE] = true;
		cw->button[0][CHOICE_PLAY] = true;
		break;
	}
	default: break;
	}

}

void updateGameGUI(struct GameGUI *gg, struct Cmd *c, TransMoveType tmt)
{
	struct GridWidget *rw, *bw, *cw;

	NOT(gg);
	NOT(c);

	rw = &gg->rackWidget;
	bw = &gg->boardWidget;
	cw = &gg->choiceWidget;

	switch (gg->focus) {
	case GUI_FOCUS_BOARD: {
		if (tmt == TRANS_MOVE_PLACE_WILD) {
			break;
		}
		switch (c->type) {
		case CMD_BOARD_UP: {
			bw->index.y += BOARD_Y;
			bw->index.y--;
			bw->index.y %= BOARD_Y;
			break;
		}
		case CMD_BOARD_DOWN: {
			bw->index.y++;
			bw->index.y %= BOARD_Y;
			break;
		}
		case CMD_BOARD_LEFT: {
			bw->index.x += BOARD_X;
			bw->index.x--;
			bw->index.x %= BOARD_X;
			break;
		}
		case CMD_BOARD_RIGHT: {
			bw->index.x++;
			bw->index.x %= BOARD_X;
			break;
		}
		default: break;
		}
		break;
	}
	case GUI_FOCUS_RACK: {
		switch (c->type) {
		case CMD_RACK_LEFT: {
			rw->index.x += RACK_SIZE;
			rw->index.x--;
			rw->index.x %= RACK_SIZE;
			break;
		}
		case CMD_RACK_RIGHT: {
			rw->index.x++;
			rw->index.x %= RACK_SIZE;
			break;
		}
		default: break;
		}
		break;
	}
	case GUI_FOCUS_CHOICE: {
		switch (c->type) {
		case CMD_CHOICE_LEFT: {
			cw->index.x += CHOICE_COUNT;
			cw->index.x--;
			cw->index.x %= CHOICE_COUNT;
			break;
		}
		case CMD_CHOICE_RIGHT: {
			cw->index.x++;
			cw->index.x %= CHOICE_COUNT;
			break;
		}
		default: break;
		}
		break;
	}
	default: break;
	}
}

void boardWidgetDraw(struct IO *io, struct GridWidget *bw, struct Player *p, struct Board *b, struct TransMove *tm, struct Coor pos, struct Coor dim)
{
	struct Tile *t;
	struct SDL_Surface *ts;
	struct Coor idx;
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
			if (t->type != TILE_NONE) {
				ts = io->tile[t->type][t->letter][TILE_LOOK_NORMAL];
				surfaceDraw(io->screen, ts, idx.x * dim.x + pos.x, idx.y * dim.y + pos.y);
			}
		}
	}

	switch (tm->type) {
	case TRANS_MOVE_PLACE:
	case TRANS_MOVE_PLACE_WILD:
	case TRANS_MOVE_PLACE_END: {
		int j;
		for (i = 0; i < RACK_SIZE; i++) {
			j = tm->adjust.data.tile[i].idx;
			idx = tm->data.place.boardIdx[j];
			if (validBoardIdx(idx)) {
				t = &p->tile[tm->adjust.data.tile[tm->data.place.rackIdx[idx.y][idx.x]].idx];
				ts = io->tile[t->type][t->letter][TILE_LOOK_HOLD];
				surfaceDraw(io->screen, ts, idx.x * dim.x + pos.x, idx.y * dim.y + pos.y);
			}
		}
		break;
	}
	default: break;
	}
}

void rackWidgetDraw(struct IO *io, struct TransMove *tm, struct GridWidget *rw, struct Coor pos, struct Coor dim, struct Player *p)
{
	int i;
	struct Tile *t;
	TileType tt;
	SDL_Surface *s;

	NOT(io);
	NOT(tm);
	NOT(p);

	switch (tm->type) {
	case TRANS_MOVE_PLACE: {
		for (i = 0; i < RACK_SIZE; i++) {
			t = &p->tile[tm->adjust.data.tile[i].idx];
			if (t->type == TILE_NONE) {
				continue;
			}
			s = NULL;
			if (tm->data.place.idx != i) {
				if (!validBoardIdx(tm->data.place.boardIdx[i])) {
					s = t->type == TILE_WILD ? io->wild[TILE_LOOK_NORMAL] : io->tile[t->type][t->letter][TILE_LOOK_NORMAL];
				}
			} else {
				s = t->type == TILE_WILD ? io->wild[TILE_LOOK_HOLD] : io->tile[t->type][t->letter][TILE_LOOK_HOLD];
			}
			if (s) {
				surfaceDraw(io->screen, s, i * dim.x + 164, 220);
			}
		}
		break;
	}
	case TRANS_MOVE_PLACE_END: {
		break;
	}
	case TRANS_MOVE_DISCARD: {
		for (i = 0; i < RACK_SIZE; i++) {
			t = &p->tile[tm->adjust.data.tile[i].idx];
			if (t->type == TILE_NONE) {
				continue;
			}
			tt = tm->data.discard.rack[i] ? TILE_LOOK_DISABLE : TILE_LOOK_NORMAL;
			s = t->type == TILE_WILD ? io->wild[tt] : io->tile[TILE_LETTER][t->letter][tt];
			surfaceDraw(io->screen, s, i * dim.x + 164, 220);
			
		}
		break;
	}
	default: {
		for (i = 0; i < RACK_SIZE; i++) {
			t = &p->tile[tm->adjust.data.tile[i].idx];
			if (t->type == TILE_NONE) {
				continue;
			}
			s = t->type == TILE_WILD ? io->wild[TILE_LOOK_NORMAL] : io->tile[TILE_LETTER][t->letter][TILE_LOOK_NORMAL];
			surfaceDraw(io->screen, s, i * dim.x + 164, 220);
		}
		break;
	}
	}
}

void choiceWidgetDraw(struct IO *io, struct TransMove *tm, struct GridWidget *cw, struct Coor pos, struct Coor dim)
{
	bool mode, recall, play;
	ModeType type;

	NOT(io);
	NOT(tm);
	NOT(cw);

	type = MODE_INVALID;
	recall = cw->button[0][CHOICE_RECALL];
	mode = cw->button[0][CHOICE_MODE]; 
	play = cw->button[0][CHOICE_PLAY];

	switch (tm->type) {
	case TRANS_MOVE_PLACE:
	case TRANS_MOVE_PLACE_WILD:
	case TRANS_MOVE_PLACE_END: type = MODE_PLACE; break;
	case TRANS_MOVE_DISCARD: type = MODE_DISCARD; break;
	case TRANS_MOVE_SKIP: type = MODE_SKIP; break;
	default: break;
	}

	if (mode) {
		surfaceDraw(io->screen, io->mode, 119, 217);
		switch (type) {
		case MODE_PLACE: surfaceDraw(io->screen, io->place, 119, 220); break;
		case MODE_DISCARD: surfaceDraw(io->screen, io->discard, 119, 220); break;
		case MODE_SKIP: surfaceDraw(io->screen, io->skip, 119, 220); break;
		default: break;
		}
	} else {
		surfaceDraw(io->screen, io->modeDisable, 119, 217);
		switch (type) {
		case MODE_PLACE: surfaceDraw(io->screen, io->placeDisable, 119, 220); break;
		case MODE_DISCARD: surfaceDraw(io->screen, io->discardDisable, 119, 220); break;
		case MODE_SKIP:	surfaceDraw(io->screen, io->skip, 119, 220); break;
		default: break;
		}
	}
	surfaceDraw(io->screen, recall ? io->recall : io->recallDisable, 105, 220);
	surfaceDraw(io->screen, play ? io->play : io->playDisable, 133, 220);
}

void gridWidgetDraw(SDL_Surface *s, struct GridWidget *gw, struct Coor pos, struct Coor dim)
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

