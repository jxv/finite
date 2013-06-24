#ifndef FINITE_WIDGET_H
#define FINITE_WIDGET_H


#include "gui.h"


void mkRackWidget(struct gridWidget*);
void mkBoardWidget(struct gridWidget*);
void mkChoiceWidget(struct gridWidget*);

void boardWidgetControls(struct cmd*, struct gameGui*, struct controls*);
void choiceWidgetControls(struct cmd*, struct gameGui*, struct controls*);
void rackWidgetControls(struct cmd*, struct gameGui*, struct controls*);

void updateBoardWidget(struct gridWidget*, struct transMove*);
void updateRackWidget(struct gridWidget*, struct transMove*);
void updateChoiceWidget(struct gridWidget*, struct transMove*);

void boardWidgetDraw(struct io*, struct gridWidget*, struct board*, struct coor, struct coor);
void rackWidgetDraw(struct io*, struct transMove*, struct gridWidget*, struct coor, struct coor, struct player*);
void choiceWidgetDraw(struct io*, struct transMove*, struct gridWidget*, struct coor, struct coor);

void gridWidgetDraw(SDL_Surface*, struct gridWidget*, struct coor, struct coor);


#endif


