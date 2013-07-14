#ifndef FINITE_WIDGET_H
#define FINITE_WIDGET_H

#include "gui.h"

void mkRackWidget(struct GridWidget *);
void mkBoardWidget(struct GridWidget *);
void mkChoiceWidget(struct GridWidget *);
void boardWidgetControls(struct Cmd *, struct GameGUI *, struct Controls *);
void choiceWidgetControls(struct Cmd *, struct GameGUI *, struct Controls *);
void rackWidgetControls(struct Cmd *, struct GameGUI *, struct Controls *);
void updateBoardWidget(struct GridWidget *, struct TransMove *, struct Board *);
void updateRackWidget(struct GridWidget *, struct TransMove *);
void updateChoiceWidget(struct GridWidget *, struct TransMove *);
void boardWidgetDraw(struct IO *, struct GridWidget *, struct Player *, struct Board *, struct TransMove *, struct Coor, struct Coor);
void updateGameGUI(struct GameGUI *, struct Cmd *, TransMoveType);
void rackWidgetDraw(struct IO *, struct TransMove *, struct GridWidget *, struct Coor, struct Coor, struct Player *);
void choiceWidgetDraw(struct IO *, struct TransMove *, struct GridWidget *, struct Coor, struct Coor);
void gridWidgetDraw(SDL_Surface *, struct GridWidget *, struct Coor, struct Coor);

#endif

