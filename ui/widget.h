#ifndef FINITE_WIDGET_H
#define FINITE_WIDGET_H


#include "gui.h"


void		mkRackWidget(struct gridWidget*);
void		mkBoardWidget(struct gridWidget*);
void		mkChoiceWidget(struct gridWidget*);
void		gridWidgetDraw(SDL_Surface*, struct gridWidget*, struct coor, struct coor);


#endif


