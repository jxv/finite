#ifndef FINITE_WIDGET_H
#define FINITE_WIDGET_H

#include "gui.h"

void mkRackWidget(GridWidget *);
void mkBoardWidget(GridWidget *);
void boardWidgetControls(Cmd *, GameGUI *, Controls *);
void rackWidgetControls(Cmd *, GameGUI *, Controls *);
void updateBoardWidget(GridWidget *, TransMove *, Board *);
void updateRackWidget(GridWidget *, TransMove *);
void boardWidgetDraw(IO *, GridWidget *, Player *, Board *, TransMove *, LastMove *, Coor);
void updateGameGUIViaCmd(GameGUI *, Cmd *, TransMoveType);
void rackWidgetDraw(IO *, TransMove *, GridWidget *, Coor, Player *);
void gridWidgetDraw(SDL_Surface *, GridWidget *, Coor, Coor);

#endif

