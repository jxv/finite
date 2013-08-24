#include <math.h>
#include <dosk.h>
#include "gui.h"
#include "update.h"
#include "init.h"
#include "widget.h"
#include "print.h"

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
	} while (validBoardIdx(mmp->boardIdx[mmp->idx]) || tm->adjust.data.tile[mmp->idx].type == tileNone);
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
				if (validBoardIdx(tm->place.boardIdx[i])) {
						tm->place.rackIdx
						[tm->place.boardIdx[i].y]
						[tm->place.boardIdx[i].x] = j;
				}
				if (validBoardIdx(tm->place.boardIdx[j])) {
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
		if (validRackIdx(*idx)) {
			int a0, a1;
			TileAdjust b0, b1;
			a0 = mmp->rackIdx[c->data.board.y][c->data.board.x];
			a1 = mmp->idx;
			b0 = tm->adjust.data.tile[a0];
			b1 = tm->adjust.data.tile[a1];
			tm->adjust.data.tile[a0] = b1;
			tm->adjust.data.tile[a1] = b0;

			if (tm->adjust.data.tile[a0].type == tileWild) {
				tm->type = transMovePlaceWild;
				mmp->idx = a0;
			}
		} else {
			*idx = mmp->idx;
			
			assert(mmp->boardIdx[mmp->idx].x == -1 && mmp->boardIdx[mmp->idx].y == -1);;
			mmp->boardIdx[mmp->idx] = c->data.board;
			if (tm->adjust.data.tile[*idx].type == tileLetter) {
				mmp->num++;
				assert(mmp->num > 0 && mmp->num <= adjustTileCount(&tm->adjust));
				if (adjustTileCount(&tm->adjust) == mmp->num) {
					tm->type = transMovePlaceEnd;
				} else {
					findNextMoveModePlaceIdx(tm);
				}
			} else {
				assert(tm->adjust.data.tile[*idx].type == tileWild);
				tm->type = transMovePlaceWild;
			}
		} 
		return true;
	}
	case cmdRackSelect: {
		TileType t;

		assert(c->data.rack >= 0);
		assert(c->data.rack < RACK_SIZE);
		t = tm->adjust.data.tile[c->data.rack].type;
		assert(t == tileNone || t == tileLetter || t == tileWild);
		if (t != tileNone && mmp->idx != c->data.rack) {
			adjustSwap(&tm->adjust, mmp->idx, c->data.rack);
			if (validBoardIdx(mmp->boardIdx[c->data.rack])) {
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
	
		if (validRackIdx(rIdx)) {
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
		} while(validBoardIdx(mmp->boardIdx[mmp->idx]));
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
	
		} while (validBoardIdx(mmp->boardIdx[mmp->idx]));
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
	tm->type = (adjustTileCount(&tm->adjust) == mmp->num) ? transMovePlaceEnd : transMovePlace;

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
		assert(mmp->num > 0 && mmp->num <= adjustTileCount(&tm->adjust));
		if (adjustTileCount(&tm->adjust) == mmp->num) {
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
	
		if (validRackIdx(rIdx)) {
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
	mkAdjust(&tm->adjust, p);
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
		if (!validBoardIdx(idx)) {
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
		m->type = MOVE_PLACE;
		moveModePlaceToMovePlace(&m->data.place, &tm->place, &tm->adjust);
		return true;
	}
	case transMoveDiscardPlay: {
		m->type = MOVE_DISCARD;
		moveModeDiscardToMoveDiscard(&m->data.discard, &tm->discard, &tm->adjust);
		return true;
	}
	case transMoveSkipPlay: m->type = MOVE_SKIP; return true;
	case transMoveQuit: m->type = MOVE_QUIT; return true;
	default: m->type = MOVE_INVALID; break;
	}
	return false;
}

void updateTitle(GUI *g, Controls *c)
{
	NOT(g);
	NOT(c);

	if (c->hardware.key[hardwareKeyStart].type == keyStatePressed) {
		g->next = guiFocusMenu;
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
	float delayTime = 0.33f;

	NOT(m);
	NOT(c);

	if (m->focus < 0 && m->focus >= m->max) {
		m->focus = m->init;
	}

	if (c->hardware.key[hardwareKeyUp].type == keyStatePressed ||
			(c->hardware.key[hardwareKeyUp].type == keyStateHeld && c->hardware.key[hardwareKeyUp].time >= delayTime) ||
			(c->hardware.axisY.type == axisStateExitDeadZone &&  c->hardware.axisY.value > 0)) {
		m->focus += m->max;
		m->focus--;
		m->focus %= m->max;
		return;
	}
	if (c->hardware.key[hardwareKeyDown].type == keyStatePressed ||
			(c->hardware.axisY.type == axisStateExitDeadZone &&  c->hardware.axisY.value < 0)) {
		m->focus++;
		m->focus %= m->max;
	}
}

bool updateMenu(GUI *g, Controls *c)
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
		case menuFocusExit: return true;
		default: break;
		}

	}
	if (goBack(c)) {
		g->next = guiFocusTitle;
	}
	return false;
}

int updateVolumes(int curVol, Controls *c)
{
	int newVol;

	NOT(c);

	newVol = curVol;
	
	if (c->hardware.key[hardwareKeyLeft].type == keyStatePressed) {
		newVol --;
	} 
	if (c->hardware.key[hardwareKeyRight].type == keyStatePressed) {
		newVol ++;
	}

	newVol = newVol < 0 ? 0 : newVol;
	newVol = newVol > MAX_GUI_VOLUME ? MAX_GUI_VOLUME : newVol;

	return newVol;
}

void updateSettings(GUI *g, Controls *c)
{
	Settings *s;

	NOT(g);
	NOT(c);

	s = &g->settings;
	
	updateMenuWidget(&s->menu, c);
	
	if (goBack(c)) {
		g->next = s->previous;
		return;
	}
	
	switch (s->menu.focus) {
	case settingsFocusMusic:
	case settingsFocusSfx: {
			int idx = s->menu.focus - volMus;
			s->vol[idx] = updateVolumes(s->vol[idx], c); 
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
	const float delayTime = 0.33f;
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

	switch (cm->menu.focus) {
	case gameKeyPlay:
	case gameKeyRecall:
	case gameKeyMode:
	case gameKeySelect:
	case gameKeyCancel:
	case gameKeyPrevTile:
	case gameKeyNextTile: {
		if (c->hardware.key[hardwareKeyLeft].type == keyStatePressed ||
	   	(c->hardware.key[hardwareKeyLeft].type == keyStateHeld && c->hardware.key[hardwareKeyLeft].time >= delayTime)) {
			do {
				c->game.key[cm->menu.focus] += gameKeyCount;
				c->game.key[cm->menu.focus] --;
				c->game.key[cm->menu.focus] %= gameKeyCount; 
			} while (!changableHardwareKey(c->game.key[cm->menu.focus]));
		}
		if (c->hardware.key[hardwareKeyRight].type == keyStatePressed ||
	   	(c->hardware.key[hardwareKeyRight].type == keyStateHeld && c->hardware.key[hardwareKeyRight].time >= delayTime)) {
			do {
				c->game.key[cm->menu.focus] ++;
				c->game.key[cm->menu.focus] %= gameKeyCount; 
			} while (!changableHardwareKey(c->game.key[cm->menu.focus]));
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
}

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
			resetNewGameGui(g, gm);
			initScoreBoard(&g->scoreBoard, gm);
			break;
		}
		case playMenuFocusHumanVsAI: {
			initGame1vs1HumanAI(gm);
			resetNewGameGui(g, gm);
			initScoreBoard(&g->scoreBoard, gm);
			break;
		}
		default: break;
		}
	}
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

	/* printCmd(&c); */

	if (updateTransMove(tm, &cmd, &gm->board, &gm->player[gm->turn])) {
		/* printTransMove(&e->transMove); */
		updateBoardWidget(&gg->boardWidget, tm, &gm->board); 
		updateRackWidget(&gg->rackWidget, tm);

		if (tm->type == transMovePlace || tm->type == transMovePlaceEnd) {
			gg->focus = gameGUIFocusBoard;
		}
		if (tm->type == transMoveDiscard) {
			gg->focus = gameGUIFocusRack;
		}
	}

	transMoveToMove(&m, tm);

	mkAction(&a, gm, &m);
	applyAction(gm, &a);

	if (cmd.type == cmdPlay || cmd.type == cmdQuit) {
		mkLog(&a, &l);
		/* printLog(&l); */
	}

	if (a.type != actionInvalid) {
		applyAdjust(&gm->player[a.playerIdx], &tm->adjust);
		if (endGame(gm)) {
			g->next = guiFocusGameOver;
		} else {
			nextTurn(gm);
			clrTransMove(tm, gm->turn, &gm->player[gm->turn], &gm->board);
			g->next = nextGUIFocusByPlayerType(gm->player[gm->turn].type);
		}
		tm->type = transMoveInvalid;
	} else {
		if (m.type != MOVE_INVALID) {
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
			nextTurn(gm);
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

void updateGameAIPause(GUI *g, Controls *c, Game *gm)
{
	Move m;
	Action a;

	NOT(g);
	NOT(c);
	NOT(gm);
	
	aiFindMove(&m, gm->turn, gm, NULL);
	mkAction(&a, gm, &m);
	applyAction(gm, &a);
	nextTurn(gm);
	g->next = nextGUIFocusByPlayerType(gm->player[gm->turn].type);

	updateScoreBoard(&g->scoreBoard, gm, SPF);
}

void updateGameAreYouSureQuit(GUI *g, Controls *c)
{
	NOT(g);
	NOT(c);

	updateMenuWidget(&g->gameAreYouSureQuit, c);
	
	if (submitted(c)) {
		g->next = g->gameAreYouSureQuit.focus == yes
				? guiFocusGameOver
				: guiFocusGameMenu;
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
		g->next = guiFocusTitle;
	}
	updateScoreBoard(&g->scoreBoard, gm, SPF);
}

void update(Env *e)
{
	NOT(e);

	e->gui.focus = e->gui.next;
	if (!e->gui.scoreBoard.stable) {
		updateScoreBoard(&e->gui.scoreBoard, &e->game, SPF);
		return;
	}
	switch (e->gui.focus) {
	case guiFocusTitle: updateTitle(&e->gui, &e->controls); break;
	case guiFocusMenu: e->quit = updateMenu(&e->gui, &e->controls); break;
	case guiFocusSettings: updateSettings(&e->gui, &e->controls); break;
	case guiFocusControls: update_guiFocusControls(&e->gui, &e->controls); break;
	case guiFocusPlayMenu: updatePlayMenu(&e->gui, &e->controls, &e->game); break;
	case guiFocusGameGUI: updateGameGUI(&e->gui, &e->controls, &e->game); break;
	case guiFocusGameMenu: update_guiFocusGameMenu(&e->gui, &e->controls, &e->game); break;
	case guiFocusGameHotseatPause: updateGameHotseatPause(&e->gui, &e->controls, &e->game); break;
	case guiFocusGameAIPause: updateGameAIPause(&e->gui, &e->controls, &e->game); break;
	case guiFocusGameOver: updateGameOver(&e->gui, &e->controls, &e->game); break;
	case guiFocusGameAreYouSureQuit: updateGameAreYouSureQuit(&e->gui, &e->controls); break;
	default: break;
	}
	e->io.time += 1.0f / ((float)(FPS));
}

