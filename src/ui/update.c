#include <math.h>
#include <pthread.h>

#include "gui.h"
#include "update.h"
#include "init.h"
#include "widget.h"
#include "print.h"

void actionToLastMove(LastMove *lm, Action *a);

bool intervalTick(float lapsed, float interval)
{
	float v;
	v = interval == 0 ? 0 : (lapsed / interval) - floorf(lapsed / interval);
	v *= interval;
	v = v > 0 ? v : -v;
	return v < SPF;  
}

void toTransScreenFadeBlack(GUI *g, GUIFocusType next, float time)
{
	g->next = guiFocusTransScreen;
	g->transScreen.type = transScreenFadeBlack;
	g->transScreen.elapsed = 0;
	g->transScreen.time = time;
	g->transScreen.focus = g->focus;
	g->transScreen.next = next;
}

void toTransScreenFadePause(GUI *g, GUIFocusType next, float time)
{
	g->next = guiFocusTransScreen;
	g->transScreen.type = transScreenFadePause;
	g->transScreen.elapsed = 0;
	g->transScreen.time = time;
	g->transScreen.focus = g->focus;
	g->transScreen.next = next;
}

void toTransScreenFadePausePixelate(GUI *g, GUIFocusType next, float time)
{
	g->next = guiFocusTransScreen;
	g->transScreen.type = transScreenFadePausePixelate;
	g->transScreen.elapsed = 0;
	g->transScreen.time = time;
	g->transScreen.focus = g->focus;
	g->transScreen.next = next;
}

void axisStateUpdate(AxisState *as)
{
	NOT(as);

	if ((as->value > 0 ? as->value : -as->value) > as->deadZone) {
		switch (as->type) {
		case axisStateInDeadZone: {
			as->type = axisStateExitDeadZone;
			as->time = 0.0f;
			break;
		}
		case axisStateExitDeadZone: {
			as->type = axisStateOutDeadZone;
			as->time = 0.0f;
			break;
		}
		case axisStateOutDeadZone: {
			as->type = axisStateOutDeadZone;
			as->time += SPF;
			break;
		}
		case axisStateEnterDeadZone: {
			as->type = axisStateExitDeadZone;
			as->time = 0.0f;
			break;
		}
		default: break;
		}
	} else {
		switch (as->type) {
		case axisStateInDeadZone: {
			as->type = axisStateInDeadZone;
			as->time += SPF;
			break;
		}
		case axisStateExitDeadZone: {
			as->type = axisStateEnterDeadZone;
			as->time = 0.0f;
			break;
		}
		case axisStateOutDeadZone: {
			as->type = axisStateEnterDeadZone;
			as->time = 0.0f;
			break;
		}
		case axisStateEnterDeadZone: {
			as->type = axisStateInDeadZone;
			as->time = 0.0f;
			break;
		}
		default: break;
		}
	}
}

void keyStateUpdate(KeyState *ks, bool touched)
{
	NOT(ks);

	if (touched) {
		switch(ks->type) {
		case keyStateUntouched: {
			ks->type = keyStatePressed;
			ks->time = 0.0f;
			break;
		}
		case keyStatePressed: {
			ks->type = keyStateHeld;
			ks->time = 0.0f;
			break;
		}
		case keyStateHeld: {
			ks->time += SPF;
			break;
		}
		case keyStateReleased: {
			ks->type = keyStatePressed;
			ks->time = 0.0f;
			break;
		}
		default: break;
		}
	} else {
		switch(ks->type) {
		case keyStateUntouched: {
			ks->time += SPF;
			break;
		}
		case keyStatePressed: {
			ks->type = keyStateReleased;
			ks->time = 0.0f;
			break;
		}
		case keyStateHeld: {
			ks->type = keyStateReleased;
			ks->time = 0.0f;
			break;
		}
		case keyStateReleased: {
			ks->type = keyStateUntouched;
			ks->time = 0.0f;
			break;
		}
		default: break;
		}
	}
}

bool isPressed(Controls *c, GameKeyType gkt)
{
	bool axis;

	NOT(c);
	
	if (!(gkt >= 0 && gkt < gameKeyCount)) {
		return false;
	}
	
	assert(gkt >= 0);
	assert(gkt < gameKeyCount);

	axis = false;

	if (gkt == gameKeyUp) {
		axis = c->hardware.axisY.type == axisStateExitDeadZone && c->hardware.axisY.value > 0;
	}
	if (gkt == gameKeyDown) {
		axis = c->hardware.axisY.type == axisStateExitDeadZone && c->hardware.axisY.value < 0;
	}
	if (gkt == gameKeyRight) {
		axis = c->hardware.axisX.type == axisStateExitDeadZone && c->hardware.axisX.value > 0;
	}
	if (gkt == gameKeyLeft) {
		axis = c->hardware.axisX.type == axisStateExitDeadZone && c->hardware.axisX.value < 0;
	}

	return c->hardware.key[c->game.key[gkt]].type == keyStatePressed || axis;
}

bool isPressedHeld(Controls *c, GameKeyType gkt)
{
	bool axis;
	const float delayTime = 0.33f; /* secs */
	KeyState *ks;
	AxisState *as;

	NOT(c);
	
	if (!(gkt >= 0 && gkt < gameKeyCount)) {
		return false;
	}
	
	assert(gkt >= 0);
	assert(gkt < gameKeyCount);

	axis = false;
	
	if (gkt == gameKeyUp) {
		as = &c->hardware.axisY;
		axis = as->type == axisStateOutDeadZone && as->value > 0 && as->time >= delayTime;
	}
	if (gkt == gameKeyDown) {
		as = &c->hardware.axisY;
		axis = as->type == axisStateOutDeadZone && as->value < 0 && as->time >= delayTime;
	}
	if (gkt == gameKeyRight) {
		as = &c->hardware.axisX;
		axis = as->type == axisStateOutDeadZone && as->value > 0 && as->time >= delayTime;
	}
	if (gkt == gameKeyLeft) {
		as = &c->hardware.axisX;
		axis = as->type == axisStateOutDeadZone && as->value < 0 && as->time >= delayTime;
	}

	ks = &c->hardware.key[c->game.key[gkt]];

	return isPressed(c, gkt) || (ks->type == keyStateHeld && ks->time >= delayTime) || axis;
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
	assert(tm->type == transMovePlace);

	mmp = &tm->place;

	do {
		mmp->idx++;
		mmp->idx %= RACK_SIZE;
	} while (valid_board_idx(mmp->boardIdx[mmp->idx]) ||
                        tm->adjust.data.tile[mmp->idx].type == TILE_NONE);
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
				if (valid_board_idx(tm->place.boardIdx[i])) {
						tm->place.rackIdx
						[tm->place.boardIdx[i].y]
						[tm->place.boardIdx[i].x] = j;
				}
				if (valid_board_idx(tm->place.boardIdx[j])) {
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
	assert(tm->type == transMovePlace);
	assert(c->type != cmdQuit);
	
	mmp = &tm->place;
	
	switch (c->type) {
	case cmdBoardSelect: {
		int *idx;

		idx = &mmp->rackIdx[c->data.board.y][c->data.board.x];
		if (valid_rack_idx(*idx)) {
			int a0, a1;
			TileAdjust b0, b1;
			a0 = mmp->rackIdx[c->data.board.y][c->data.board.x];
			a1 = mmp->idx;
			b0 = tm->adjust.data.tile[a0];
			b1 = tm->adjust.data.tile[a1];
			tm->adjust.data.tile[a0] = b1;
			tm->adjust.data.tile[a1] = b0;

			if (tm->adjust.data.tile[a0].type == TILE_WILD) {
				tm->type = transMovePlaceWild;
				mmp->idx = a0;
			}
		} else {
			*idx = mmp->idx;
			
			assert(mmp->boardIdx[mmp->idx].x == -1 && mmp->boardIdx[mmp->idx].y == -1);;
			mmp->boardIdx[mmp->idx] = c->data.board;
			if (tm->adjust.data.tile[*idx].type == TILE_LETTER) {
				mmp->num++;
				assert(mmp->num > 0 && mmp->num <= adjust_tile_count(tm->adjust.data.tile));
				if (adjust_tile_count(tm->adjust.data.tile) == mmp->num) {
					tm->type = transMovePlaceEnd;
				} else {
					findNextMoveModePlaceIdx(tm);
				}
			} else {
				assert(tm->adjust.data.tile[*idx].type == TILE_WILD);
				tm->type = transMovePlaceWild;
			}
		} 
		return true;
	}
	case cmdRackSelect: {
		tile_tag_t t;

		assert(c->data.rack >= 0);
		assert(c->data.rack < RACK_SIZE);
		t = tm->adjust.data.tile[c->data.rack].type;
		assert(t == TILE_NONE || t == TILE_LETTER || t == TILE_WILD);
		if (t != TILE_NONE && mmp->idx != c->data.rack) {
			adjust_swap(mmp->idx, c->data.rack,
                                tm->adjust.data.tile);
			if (valid_board_idx(mmp->boardIdx[c->data.rack])) {
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
	case cmdRecall: {
		tm->type = transMovePlace;
		clrMoveModePlace(mmp, b);
		return true;
	}
	case cmdBoardCancel: {
		Coor bIdx;
		int rIdx;
		
		bIdx = c->data.board;
		rIdx = mmp->rackIdx[bIdx.y][bIdx.x];
	
		if (valid_rack_idx(rIdx)) {
			assert(mmp->rackIdx[mmp->boardIdx[rIdx].y][mmp->boardIdx[rIdx].x] == rIdx);
			mmp->rackIdx[bIdx.y][bIdx.x] = -1;
			mmp->boardIdx[rIdx].x = -1;
			mmp->boardIdx[rIdx].y = -1;
			mmp->num--;
			return true;
		}
		break;
	}
	case cmdTilePrev: {
		do {
			mmp->idx += RACK_SIZE;
			mmp->idx--;
			mmp->idx %= RACK_SIZE;
		} while(valid_board_idx(mmp->boardIdx[mmp->idx]));
		return true;
	}
	case cmdTileNext: {
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
	
		} while (valid_board_idx(mmp->boardIdx[mmp->idx]));
		return true;
	}
	case cmdMode: {
		tm->type = transMoveDiscard;
		return true;
	}
	case cmdPlay: {
		tm->type = transMovePlacePlay;
		return true;
	}
	case cmdShuffle: {
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
	assert(tm->type == transMovePlacePlay);
	assert(c->type != cmdQuit);

	mmp = &tm->place;
	tm->type = (adjust_tile_count(tm->adjust.data.tile) == mmp->num) ? transMovePlaceEnd : transMovePlace;

	return true;
}

bool updateTransMovePlaceWild(TransMove *tm, Cmd *c, Board *b, Player *p)
{
	MoveModePlace *mmp;

	NOT(tm);
	NOT(c);
	NOT(b);
	NOT(p);
	assert(tm->type == transMovePlaceWild);
	
	mmp = &tm->place;
	
	switch (c->type) {
	case cmdBoardSelect: {
		mmp->num++;
		assert(mmp->num > 0 && mmp->num <= adjust_tile_count(tm->adjust.data.tile));
		if (adjust_tile_count(tm->adjust.data.tile) == mmp->num) {
			tm->type = transMovePlaceEnd;
		} else {
			tm->type = transMovePlace;
			findNextMoveModePlaceIdx(tm);
		}
		return true;
	}
	case cmdBoardCancel: {
		tm->type = transMovePlace;
		mmp->rackIdx[mmp->boardIdx[mmp->idx].y][mmp->boardIdx[mmp->idx].x] = -1;
		mmp->boardIdx[mmp->idx].x = -1;
		mmp->boardIdx[mmp->idx].y = -1;
		return true;
	}
	case cmdBoardUp: {
		int idx = tm->adjust.data.tile[mmp->idx].idx;
		p->tile[idx].letter += letterCount;
		p->tile[idx].letter--;
		p->tile[idx].letter %= letterCount;
		return true;
	}
	case cmdBoardDown: {
		int idx = tm->adjust.data.tile[mmp->idx].idx;
		p->tile[idx].letter++;
		p->tile[idx].letter %= letterCount;
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
	assert(tm->type == transMovePlaceEnd);
	assert(c->type != cmdQuit);
	
	mmp = &tm->place;
	
	switch (c->type) {
	case cmdBoardSelect:
	case cmdBoardCancel: {
		Coor bIdx;
		int rIdx;
		
		bIdx = c->data.board;
		rIdx = mmp->rackIdx[bIdx.y][bIdx.x];
	
		if (valid_rack_idx(rIdx)) {
			assert(mmp->boardIdx[rIdx].x == bIdx.x && mmp->boardIdx[rIdx].y == bIdx.y);
			mmp->rackIdx[bIdx.y][bIdx.x] = -1;
			mmp->boardIdx[rIdx].x = -1;
			mmp->boardIdx[rIdx].y = -1;
			mmp->num--;
			tm->type = transMovePlace;
			mmp->idx = rIdx;
			return true;
		} 
		break;
	}
	case cmdMode: {
		tm->type = transMoveDiscard;
		return true;
	}
	case cmdRecall: {
		tm->type = transMovePlace;
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
	assert(tm->type == transMoveDiscard);
	assert(c->type != cmdBoardSelect);
	assert(c->type != cmdQuit);
	
	mmd = &tm->discard;
	
	switch (c->type) {
	case cmdRackSelect: {
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
	case cmdRecall: {
		clrMoveModeDiscard(mmd);
		return true;
	}
	case cmdPlay: {
		tm->type = transMoveDiscardPlay;
		return true;
	}
	case cmdMode: {
		tm->type = transMovePlace;
		return true;
	}
	case cmdRackCancel: {
		if (mmd->rack[c->data.rack]) {
			assert(mmd->num > 0);
			mmd->num--;
			mmd->rack[c->data.rack] = false;
		}
		return true;
	}
	case cmdShuffle: {
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
	assert(tm->type == transMoveSkip);
	assert(c->type != cmdBoardSelect);
	assert(c->type != cmdRackSelect);
	assert(c->type != cmdRecall);
	
	switch (c->type) {
	case cmdPlay: {
		tm->type = transMoveSkipPlay;
		return true;
	}
	case cmdShuffle: {
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
	case transMovePlace: return updateTransMovePlace(tm, c, b, p);
	case transMovePlaceWild: return updateTransMovePlaceWild(tm, c, b, p);
	case transMovePlaceEnd: return updateTransMovePlaceEnd(tm, c, b, p);
	case transMovePlacePlay: return updateTransMovePlacePlay(tm, c, b, p);
	case transMoveDiscard: return updateTransMoveDiscard(tm, c, b, p);
	case transMoveSkip: return updateTransMoveSkip(tm, c, b, p);
	case transMoveQuit: return updateTransMoveQuit(tm, c, b, p);
	case transMoveNone:
	case transMoveInvalid: 
	default: tm->type = transMoveInvalid; break;
	}
	return false;
}

void clrTransMove(TransMove *tm, int pidx, Player *p, Board *b)
{
	NOT(tm);

	tm->type = transMovePlace;
	tm->playerIdx = pidx;
	mk_adjust(p, &tm->adjust);
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
		if (!valid_board_idx(idx)) {
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
	case transMovePlacePlay: {
		m->type = movePlace;
		moveModePlaceToMovePlace(&m->data.place, &tm->place, &tm->adjust);
		return true;
	}
	case transMoveDiscardPlay: {
		m->type = moveDiscard;
		moveModeDiscardToMoveDiscard(&m->data.discard, &tm->discard, &tm->adjust);
		return true;
	}
	case transMoveSkipPlay: m->type = moveSkip; return true;
	case transMoveQuit: m->type = moveQuit; return true;
	default: m->type = moveInvalid; break;
	}
	return false;
}

void update_guiFocusTitle(GUI *g, Controls *c)
{
	NOT(g);
	NOT(c);

	if (c->hardware.key[hardwareKeyStart].type == keyStatePressed) {
		toTransScreenFadeBlack(g, guiFocusMenu, 0.25f);
	}
}

bool submitted(Controls *c)
{
	return c->hardware.key[hardwareKeyA].type == keyStatePressed
		|| c->hardware.key[hardwareKeyStart].type == keyStatePressed;
}

bool goBack(Controls *c)
{
	return c->hardware.key[hardwareKeyB].type == keyStatePressed;
}

void updateMenuWidget(MenuWidget *m, Controls *c)
{
	float tickTime = 0.1f;
	float delayTime = 0.5f;
	KeyState *ks;
	AxisState *as;

	NOT(m);
	NOT(c);

	if (m->next < 0 && m->next >= m->max) {
		m->next = m->init;
	}
	m->focus = m->next;

	ks = &c->hardware.key[hardwareKeyUp];
	as = &c->hardware.axisY;

	if (ks->type == keyStatePressed ||
	(ks->type == keyStateHeld && ks->time >= delayTime && intervalTick(ks->time, tickTime)) ||
	(as->type == axisStateExitDeadZone &&  as->value > 0)) {
		m->next += m->max;
		m->next --;
		m->next %= m->max;
		return;
	}

	ks = &c->hardware.key[hardwareKeyDown];

	if (ks->type == keyStatePressed ||
	(ks->type == keyStateHeld && ks->time >= delayTime && intervalTick(ks->time, tickTime))  ||
	(as->type == axisStateExitDeadZone &&  as->value < 0)) {
		m->next++;
		m->next %= m->max;
	}
}

bool update_guiFocusMenu(GUI *g, Controls *c)
{

	MenuWidget *m;

	NOT(g);
	NOT(c);

	m = &g->menu;

	updateMenuWidget(m, c);

	if (submitted(c)) {
		switch (m->focus) {
		case menuFocusPlay: g->next = guiFocusPlayMenu; break;
		case menuFocusSettings: g->next = guiFocusSettings; break;
		case menuFocusRules: g->next = guiFocusRules; break;
		case menuFocusExit: return true;
		default: break;
		}

	}
	if (goBack(c)) {
		toTransScreenFadeBlack(g, guiFocusTitle, 0.25f);
	}
	return false;
}

bool update_guiFocusRules(GUI *g, Controls *c)
{
	const float delayTime = 0.33f;
	const float move = 0.0005f / SPF;

	if ((c->hardware.axisY.type == axisStateOutDeadZone && c->hardware.axisY.value > 0) ||
	c->hardware.key[hardwareKeyUp].type == keyStatePressed ||
   	(c->hardware.key[hardwareKeyUp].type == keyStateHeld && c->hardware.key[hardwareKeyUp].time >= delayTime)) {
		g->rules -= move;
	}
	
	if ((c->hardware.axisY.type == axisStateOutDeadZone && c->hardware.axisY.value < 0) ||
	c->hardware.key[hardwareKeyDown].type == keyStatePressed ||
   	(c->hardware.key[hardwareKeyDown].type == keyStateHeld && c->hardware.key[hardwareKeyDown].time >= delayTime)) {
		g->rules += move;
	}

	if (g->rules < 0.0f) {
		g->rules = 0.0f;
	}

	if (g->rules > 1.0f) {
		g->rules = 1.0f;
	}

	if (goBack(c)) {
		g->next = guiFocusMenu;
	}

	return false;
}

int updateVolumes(int curVol, Controls *c)
{
	KeyState *ks;
	AxisState *as;
	int newVol;
	const float tickTime = 0.1f;
	const float delayTime = 0.5f;

	NOT(c);

	newVol = curVol;
	
	ks = &c->hardware.key[hardwareKeyLeft];
	as = &c->hardware.axisX;

	if (ks->type == keyStatePressed ||
	(ks->type == keyStateHeld && ks->time >= delayTime && intervalTick(ks->time, tickTime)) ||
	(as->type == axisStateExitDeadZone && as->value < 0)) {
		newVol --;
	}
 
	ks = &c->hardware.key[hardwareKeyRight];
	if (ks->type == keyStatePressed ||
	(ks->type == keyStateHeld && ks->time >= delayTime && intervalTick(ks->time, tickTime)) ||
	(as->type == axisStateExitDeadZone && as->value > 0)) {
		newVol ++;
	}

	newVol = newVol < 0 ? 0 : newVol;
	newVol = newVol > MAX_GUI_VOLUME ? MAX_GUI_VOLUME : newVol;

	return newVol;
}

void update_guiFocusSettings(GUI *g, Controls *c)
{
	Settings *s;

	NOT(g);
	NOT(c);

	s = &g->settings;
	
	updateMenuWidget(&s->menu, c);
	
	s->snd = false;
	if (goBack(c)) {
		g->next = s->previous;
		return;
	}
	
	switch (s->menu.focus) {
	case settingsFocusSfx: {
			int pvol;
			int idx = s->menu.focus - volMus;
			pvol = s->vol[idx];
			s->vol[idx] = updateVolumes(s->vol[idx], c); 
			if (pvol != s->vol[idx]) {
				s->snd = true;
			}
			break;
	}
	case settingsFocusControls: {
		if (submitted(c)) {
			g->next =  guiFocusControls;
		}
		break;
	}
	default: break;
	}


}

bool changableGameKey(GameKeyType gkt)
{
	return !(gkt == gameKeyUp || gkt == gameKeyDown || gkt == gameKeyLeft || gkt == gameKeyRight || gkt == gameKeyShuffle);
}

bool changableHardwareKey(HardwareKeyType hkt)
{
	return !(hkt == hardwareKeyUp || hkt == hardwareKeyDown || hkt == hardwareKeyLeft || hkt == hardwareKeyRight || hkt == hardwareKeyStart);
}

void clrDups(ControlsMenu *cm)
{
	int i;

	NOT(cm);

	for (i = 0; i < gameKeyCount; i++) {
		cm->dupKey[i] = false;
	}
	cm->dups = false;
}

bool gameControlsAreDifferent(ControlsMenu *cm, GameControls *gc)
{
	int i, j, k;

	NOT(cm);
	NOT(gc);

	k = 0;

	clrDups(cm);
	

	for (i = 0; i < gameKeyCount; i++) {
		for (j = i + 1; j < gameKeyCount; j++) {
			if (gc->key[i] == gc->key[j]) {
				cm->dupKey[i] = true;
				cm->dupKey[j] = true;
				k++;
			}
		}
	}

	cm->dups = k != 0;
	if (cm->dups) {
		cm->time = 1.0f;
	}

	return k == 0;
}

void update_guiFocusControls(GUI *g, Controls *c)
{
	const float tickTime = 0.1f;
	const float delayTime = 1.f;
	ControlsMenu *cm;

	NOT(g);
	NOT(c);
	
	cm = &g->controlsMenu;
	
	updateMenuWidget(&cm->menu, c);

	if (g->controlsMenu.time > 0) {
		g->controlsMenu.time -= SPF;
		if (g->controlsMenu.time <= 0) {
			clrDups(&g->controlsMenu);
			g->controlsMenu.time = 0;
		}
	}

	if (goBack(c) && gameControlsAreDifferent(&g->controlsMenu, &c->game)) {
		g->next = guiFocusSettings;
		return;
	}

	cm->snd = false;
	switch (cm->menu.focus) {
	case gameKeyPlay:
	case gameKeyRecall:
	case gameKeyMode:
	case gameKeySelect:
	case gameKeyCancel:
	case gameKeyPrevTile:
	case gameKeyNextTile: {
		KeyState *ks;

		ks = &c->hardware.key[hardwareKeyLeft];
		if (ks->type == keyStatePressed ||
	   	(ks->type == keyStateHeld && ks->time >= delayTime && intervalTick(ks->time, tickTime)) ||
		(c->hardware.axisX.type == axisStateExitDeadZone &&  c->hardware.axisX.value < 0)) {
			do {
				c->game.key[cm->menu.focus] += gameKeyCount;
				c->game.key[cm->menu.focus] --;
				c->game.key[cm->menu.focus] %= gameKeyCount; 
			} while (!changableHardwareKey(c->game.key[cm->menu.focus]));
			cm->snd = true;
		}

		ks = &c->hardware.key[hardwareKeyRight];
		if (ks->type == keyStatePressed ||
	   	(ks->type == keyStateHeld && ks->time >= delayTime) ||
		(c->hardware.axisX.type == axisStateExitDeadZone &&  c->hardware.axisX.value > 0)) {
			do {
				c->game.key[cm->menu.focus] ++;
				c->game.key[cm->menu.focus] %= gameKeyCount; 
			} while (!changableHardwareKey(c->game.key[cm->menu.focus]));
			cm->snd = true;
		}
		break;
	}

	case gameKeyShuffle:
	case gameKeyUp:
	case gameKeyDown:
	case gameKeyLeft:
	case gameKeyRight:
	default: break;
	}
}

void updateGameGUIWidgets(GameGUI *gg, TransMove *tm, Board *b)
{
	updateBoardWidget(&gg->boardWidget, tm, b); 
	updateRackWidget(&gg->rackWidget, tm);
}

void resetNewGameGui(GUI *g, Game *gm)
{
	Cmd c;

	NOT(g);
	NOT(gm);

	g->next = guiFocusGameGUI;
	clrTransMove(&g->transMove, gm->turn, &gm->player[gm->turn], &gm->board);
	c.type = cmdInvalid;
	updateTransMove(&g->transMove, &c, &gm->board, &gm->player[gm->turn]);
	updateGameGUIWidgets(&g->gameGui, &g->transMove, &gm->board);
	g->gameGui.lastInput = 0;
}

void initTextLog(TextLog *tl);

void updatePlayMenu(GUI *g, Controls *c, Game *gm)
{
	MenuWidget *m;

	NOT(g);
	NOT(c);
	NOT(gm);

	m = &g->playMenu;
	
	updateMenuWidget(m, c);

	if (goBack(c)) {
		g->next = guiFocusMenu;
		return;
	}

	if (submitted(c)) {
		switch (m->focus) {
		case playMenuFocusHumanVsHuman: {
			initGame1vs1Human(gm);
			initTextLog(&g->gameGui.textLog);
			resetNewGameGui(g, gm);
			initScoreBoard(&g->scoreBoard, gm);
			toTransScreenFadePausePixelate(g, guiFocusGameGUI, 1.0f);
			break;
		}
		case playMenuFocusHumanVsAI: {
			initGame1vs1HumanAI(gm);
			initTextLog(&g->gameGui.textLog);
			resetNewGameGui(g, gm);
			initScoreBoard(&g->scoreBoard, gm);
			toTransScreenFadePausePixelate(g, guiFocusGameGUI, 1.0f);
			break;
		}
		case playMenuFocusOptions: {
			g->next = guiFocusOptions;
			break;
		}
		default: break;
		}
	}
}

void update_guiFocusOptions(GUI *g, Controls *c, Game *gm)
{
	const float delayTime = 1.f;
	bool left, right;

	NOT(g);
	NOT(c);
	NOT(gm);

	left = false;
	right = false;

	updateMenuWidget(&g->options.menu, c);

	if (goBack(c)) {
		g->next = guiFocusPlayMenu;
		return;
	}

	left = c->hardware.key[hardwareKeyLeft].type == keyStatePressed ||
		(c->hardware.key[hardwareKeyLeft].type == keyStateHeld && c->hardware.key[hardwareKeyLeft].time >= delayTime) ||
		(c->hardware.axisX.type == axisStateExitDeadZone &&  c->hardware.axisX.value < 0);
	
	right = c->hardware.key[hardwareKeyRight].type == keyStatePressed ||
		(c->hardware.key[hardwareKeyRight].type == keyStateHeld && c->hardware.key[hardwareKeyLeft].time >= delayTime) ||
		(c->hardware.axisX.type == axisStateExitDeadZone &&  c->hardware.axisX.value > 0);

	g->options.snd = false;
	switch (g->options.menu.focus) {
	case optionsFocusAI: {
		if (left && g->options.ai > 1) {
			g->options.ai--;
			g->options.snd = true;
		}
		if (right && g->options.ai < 10) {
			g->options.ai++;
			g->options.snd = true;
		}
		break;
	}
	default: break;
	}

	gm->player[1].aiShare.difficulty = g->options.ai;
}

GUIFocusType nextGUIFocusByPlayerType(PlayerType pt)
{
	assert(pt >= 0 && pt < playerCount);

	switch (pt) {
	case playerHuman: return guiFocusGameHotseatPause;
	case playerAI: return guiFocusGameAIPause;
	default: break;
	}
	assert(false);
	return guiFocusGameGUI;
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

bool isAlphaChar(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isNumChar(char c)
{
	return c >= '0' && c <= '9';
}

bool isAlphaNumChar(char c)
{
	return isAlphaChar(c) || isNumChar(c);
}

void addStrToTextLog(TextLog *tl, char *str)
{
	int i, j, k, p;

	NOT(tl);
	NOT(str);

	k = strlen(str);
	i = 0;
	p = tl->head + tl->size;
	p %= tl->maxSize;

	while (i < k) {
		j = 0;
		while (j < tl->maxLen && i < k) {
			if (str[i] == '\n') {
				tl->line[p][j] = '\0';
				i++;
				break;
			}
			if (j + 1 == tl->maxLen && i + 1 < k) {
				if (isAlphaNumChar(str[i]) && isAlphaNumChar(str[i + 1])) {
					tl->line[p][j] = '-';
					tl->line[p][j + 1] = '\0';
					break;
				}
			}
			tl->line[p][j] = str[i];
			tl->line[p][j + 1] = '\0';
			i++;
			j++;
		}
		p++;
		p %= tl->maxSize;

		if (tl->size < tl->maxSize) {
			tl->size++;
		} else {
			tl->head++;
			tl->head %= tl->maxSize;
		}
	}
}

void addMoveToTextLog(TextLog *tl, Move *m, Player *p)
{
	char str[64];

	NOT(tl);
	NOT(m);

	str[0] = '\0';

	switch (m->type) {
	case movePlace: {
		char *numSmall = "PLAYER %d placed tile.";
		char *numBig = "PLAYER %d placed tiles.";
		sprintf(str, m->data.place.num > 1 ? numBig : numSmall, m->playerIdx + 1);
		break;
	}
	case moveDiscard: {
		char *numSmall = "PLAYER %d discarded tile.";
		char *numBig = "PLAYER %d discarded tiles.";
		sprintf(str, m->data.place.num > 1 ? numBig : numSmall, m->playerIdx + 1);
		break;
	}
	case moveSkip: sprintf(str, "PLAYER %d skipped turn.", m->playerIdx + 1); break;
	case moveQuit: sprintf(str, "PLAYER %d quit.", m->playerIdx + 1); break;
	default: break;
	}
	
	addStrToTextLog(tl, str);
}

void addActionToTextLog(TextLog *tl, Action *a)
{
	char str[64];

	NOT(tl);
	NOT(a);

	str[0] = '\0';

	switch (a->type) {
	case actionPlace: {
		char *singluar = "PLAYER %d scored %d points.";
		char *plural = "PLAYER %d scored %d points.";
		sprintf(str, a->data.place.num == 1 ? singluar : plural, a->playerIdx + 1, a->data.place.score);
		break;
	}
	case actionDiscard: {
		char *singluar = "PLAYER %d lost %d points.";
		char *plural = "PLAYER %d lost %d points.";
		sprintf(str, a->data.place.num == 1 ? singluar : plural, a->playerIdx + 1, -a->data.discard.score);
		break;
	}
	case actionSkip: {
		sprintf(str, "PLAYER %d skipped a move.", a->playerIdx + 1);
		break;
	}
	case actionQuit: {
		sprintf(str, "PLAYER %d quit.", a->playerIdx + 1);
		break;
	}
	default: break;
	}

	addStrToTextLog(tl, str);
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

	if (c->hardware.key[hardwareKeyStart].type == keyStatePressed) {
		g->next = guiFocusGameMenu;
		return;
	}
	
	if (tm->type == transMoveInvalid) {
		resetNewGameGui(g, gm);
	}
	
	switch (gg->focus) {
	case gameGUIFocusBoard: boardWidgetControls(&cmd, gg, c); break;
	case gameGUIFocusRack: rackWidgetControls(&cmd, gg, c); break;
	default: break;
	}
	updateGameGUIViaCmd(gg, &cmd, tm->type);
	
	switch (cmd.type) {
	case cmdFocusTop: {
		if (tm->type == transMovePlace ||
		    tm->type == transMovePlaceEnd ||
		    tm->type == transMovePlacePlay) {
			gg->focus = gameGUIFocusBoard;
		}
		break;
	}
	case cmdFocusBottom: {
		if (tm->type == transMovePlaceWild) {
			break;
		}
		gg->focus = gameGUIFocusRack;
		break;
	}
	case cmdBoard: {
		if (tm->type == transMoveDiscard || tm->type == transMoveDiscardPlay) {
			break;
		}
		gg->focus = gameGUIFocusBoard;
		gg->boardWidget.index = cmd.data.board;
		break;
	}
	case cmdRack: {
		if (tm->type == transMoveSkip) {
			break;
		}
		gg->focus = gameGUIFocusRack;
		gg->rackWidget.index.x = cmd.data.rack;
		gg->rackWidget.index.y = 0;
		break;
	}
	default: break;
	}

	if (cmd.type == cmdInvalid) {
		gg->lastInput += SPF;
	} else {
		gg->lastInput = 0;
	}

	/* printCmd(&c); */

	if (updateTransMove(tm, &cmd, &gm->board, &gm->player[gm->turn])) {
		/* printTransMove(&e->transMove); */
		updateBoardWidget(&gg->boardWidget, tm, &gm->board); 
		updateRackWidget(&gg->rackWidget, tm);
		
		if (tm->type == transMoveDiscard) {
			gg->focus = gameGUIFocusRack;
		}
	}

	transMoveToMove(&m, tm);

	mk_action(gm, &m, &a);
	apply_action(&a, gm);

	if (cmd.type == cmdPlay || cmd.type == cmdQuit)
		mkLog(&a, &l);

	if (a.type != actionInvalid) {
		addActionToTextLog(&gg->textLog, &a);
		actionToLastMove(&gg->lastMove, &a);
		apply_adjust(tm->adjust.data.tile, &gm->player[a.playerIdx]);
		/* log action */
		if (end_game(gm)) {
			g->next = guiFocusGameOver;
			/* log game over */
		} else {
			next_turn(gm);
			clrTransMove(tm, gm->turn, &gm->player[gm->turn], &gm->board);
			g->next = nextGUIFocusByPlayerType(gm->player[gm->turn].type);
		}
		tm->type = transMoveInvalid;
	} else {
		if (m.type != moveInvalid) {
			/* printActionErr(a.type); */
		}
	} 

	if (cmd.type == cmdPlay) {
		gg->validPlay = a.type != actionInvalid ? yes : no;
	} else {
		gg->validPlay = yesNoInvalid;
	}

	updateScoreBoard(&g->scoreBoard, gm, SPF);
}

void update_guiFocusGameMenu(GUI *g, Controls *c, Game *gm)
{
	MenuWidget *m;

	NOT(g);
	NOT(c);
	NOT(gm);

	m = &g->gameMenu;

	updateMenuWidget(m, c);

	if (goBack(c)) {
		g->next = guiFocusGameGUI;
		return;
	}

	if (submitted(c)) {
		switch (m->focus) {
		case gameMenuFocusResume: {
			g->next = guiFocusGameGUI;
			break;
		}
		case gameMenuFocusSettings: {
			g->settings.previous = g->focus;
			g->next = guiFocusSettings;
			break;
		}
		case gameMenuFocusSkip: {
			g->transMove.type = transMoveSkipPlay;
			next_turn(gm);
			g->next = nextGUIFocusByPlayerType(gm->player[gm->turn].type);
			break;
		}
		case gameMenuFocusQuit: {
			g->next = guiFocusGameAreYouSureQuit;
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
		g->next = guiFocusGameGUI;
	}
	updateScoreBoard(&g->scoreBoard, gm, SPF);
}

void *cbUpdateAi(void *data)
{
	Move m;
	AiShare *as; 
	Game *gm;

	NOT(data);

	as = data;
	gm = as->game;

	aiFindMove(&m, gm->turn, gm, NULL, &as->loading);

	mk_action(gm, &m, &as->action);

	as->loading = 1.f;

	as->shareEnd = true;

	return NULL;
}

void updateGameAIPause(GUI *g, Controls *c, Game *gm)
{
	NOT(g);
	NOT(c);
	NOT(gm);

	if (!gm->player[gm->turn].aiShare.shareStart) {
		pthread_t thread;

		gm->player[gm->turn].aiShare.shareStart = true;
		gm->player[gm->turn].aiShare.shareEnd = false;
		gm->player[gm->turn].aiShare.loading = 0.f;
		gm->player[gm->turn].aiShare.game = gm;

		if (pthread_create(&thread, NULL, cbUpdateAi, &gm->player[gm->turn].aiShare) != 0) {
			exit(2);
		}
	}

	if (gm->player[gm->turn].aiShare.shareEnd) {
		gm->player[gm->turn].aiShare.shareStart = false;
		gm->player[gm->turn].aiShare.shareEnd = false;
		gm->player[gm->turn].aiShare.loading = 1.f;

		addActionToTextLog(&g->gameGui.textLog, &gm->player[gm->turn].aiShare.action);
		actionToLastMove(&g->gameGui.lastMove, &gm->player[gm->turn].aiShare.action);
		apply_action(&gm->player[gm->turn].aiShare.action, gm);

		next_turn(gm);
		g->next = nextGUIFocusByPlayerType(gm->player[gm->turn].type);
	}

}

void updateGameAreYouSureQuit(GUI *g, Game *gm, Controls *c)
{
	NOT(g);
	NOT(c);
	NOT(gm);

	updateMenuWidget(&g->gameAreYouSureQuit, c);
	
	if (submitted(c)) {
		g->next = guiFocusGameMenu;
		if (g->gameAreYouSureQuit.focus == yes) {
			Move m;
			Action a;

			g->next = guiFocusGameOver;

			m.type = moveQuit;
			m.playerIdx = gm->turn;

			mk_action(gm, &m, &a);

			assert(a.type == ACTION_QUIT);

			apply_action(&a, gm);
		}
		return;
	}
	if (goBack(c)) {
		g->next = guiFocusGameMenu;
	}
}

void updateGameOver(GUI *g, Controls *c, Game *gm)
{
	NOT(g);
	NOT(c);
		
	if (c->hardware.key[hardwareKeyStart].type == keyStatePressed) {
		toTransScreenFadePausePixelate(g, guiFocusTitle, 1.0f);
	}
	updateScoreBoard(&g->scoreBoard, gm, SPF);
}

void update_guiFocusTransScreen(GUI *g, float timeStep)
{
	TransScreen *ts;

	NOT(g);

	ts = &g->transScreen;
	ts->elapsed += timeStep;

	if (ts->elapsed >= ts->time) {
		g->next = ts->next;
	}
}

void update(Env *e)
{
	NOT(e);

	e->gui.focus = e->gui.next;
	if (!e->gui.scoreBoard.stable) {
		updateScoreBoard(&e->gui.scoreBoard, &e->game, SPF);
	}
	switch (e->gui.focus) {
	case guiFocusTitle: update_guiFocusTitle(&e->gui, &e->io.controls); break;
	case guiFocusMenu: e->quit = update_guiFocusMenu(&e->gui, &e->io.controls); break;
	case guiFocusRules: e->quit = update_guiFocusRules(&e->gui, &e->io.controls); break;
	case guiFocusSettings: update_guiFocusSettings(&e->gui, &e->io.controls); break;
	case guiFocusControls: update_guiFocusControls(&e->gui, &e->io.controls); break;
	case guiFocusPlayMenu: updatePlayMenu(&e->gui, &e->io.controls, &e->game); break;
	case guiFocusOptions: update_guiFocusOptions(&e->gui, &e->io.controls, &e->game); break;
	case guiFocusGameGUI: updateGameGUI(&e->gui, &e->io.controls, &e->game); break;
	case guiFocusGameMenu: update_guiFocusGameMenu(&e->gui, &e->io.controls, &e->game); break;
	case guiFocusGameHotseatPause: updateGameHotseatPause(&e->gui, &e->io.controls, &e->game); break;
	case guiFocusGameAIPause: updateGameAIPause(&e->gui, &e->io.controls, &e->game); break;
	case guiFocusGameOver: updateGameOver(&e->gui, &e->io.controls, &e->game); break;
	case guiFocusGameAreYouSureQuit: updateGameAreYouSureQuit(&e->gui, &e->game, &e->io.controls); break;
	case guiFocusTransScreen: update_guiFocusTransScreen(&e->gui, SPF);
	default: break;
	}
	e->io.time += SPF;
}

