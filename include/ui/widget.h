#ifndef FINITE_WIDGET_H
#define FINITE_WIDGET_H

#include "gui.h"

void mkRackWidget(GridWidget *);
void mkBoardWidget(GridWidget *);
void boardWidgetControls(Cmd *, GameGUI *, Controls *);
void rackWidgetControls(Cmd *, GameGUI *, Controls *);
void updateBoardWidget(GridWidget *, TransMove *, board_t *);
void updateRackWidget(GridWidget *, TransMove *);
void boardWidgetDraw(IO *, GridWidget *, player_t *, board_t *, TransMove *,
		     LastMove *, coor_t);
void boardWidgetDrawWithoutTransMove(IO *, GridWidget *, board_t *,
				     LastMove *, coor_t);
void updateGameGUIViaCmd(GameGUI *, Cmd *, TransMoveType);
void rackWidgetDraw(IO *, TransMove *, GridWidget *, coor_t, player_t *);
void gridWidgetDraw(SDL_Surface *, GridWidget *, coor_t, coor_t);

#endif

