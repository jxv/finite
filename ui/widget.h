#ifndef FINITE_WIDGET_H
#define FINITE_WIDGET_H

#include "gui.h"

void mkRackWidget(struct GridWidget *);
void mkBoardWidget(struct GridWidget *);
void mkChoiceWidget(struct GridWidget *);
void boardWidgetControls(struct Cmd *, struct GameGUI *, struct Controls *);
void choiceWidgetControls(struct Cmd *, struct GameGUI *, struct Controls *);
void rackWidgetControls(struct Cmd *, struct GameGUI *, struct Controls *);
void updateBoardWidget(struct GridWidget *, struct TransMove *);
void updateRackWidget(struct GridWidget *, struct TransMove *);
void updateChoiceWidget(struct GridWidget *, struct TransMove *);
void boardWidgetDraw(struct IO *, struct GridWidget *, struct Board *, struct Coor, struct Coor);
void rackWidgetDraw(struct IO *, struct TransMove *, struct GridWidget *, struct Coor, struct Coor, struct Player *);
void choiceWidgetDraw(struct IO *, struct TransMove *, struct GridWidget *, struct Coor, struct Coor);
void gridWidgetDraw(SDL_Surface *, struct GridWidget *, struct Coor, struct Coor);

#endif

