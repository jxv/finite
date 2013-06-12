#include "widget.h"


bool validRackId(int id)
{
	return id >= 0 && id < RACK_SIZE;
}


bool validGridId(struct coor c)
{
	return c.x >= 0 && c.y >= 0 && c.x < BOARD_X && c.y < BOARD_Y;
}


void onPressRack(void *cmdr, struct coor idx)
{
	NOT(cmdr);
}


void onPressChoice(void *cmdr, struct coor idx)
{
	NOT(cmdr);
}


void onPressBoard(void *cmdr, struct coor idx)
{
	NOT(cmdr);
}


void mkGridWidgetByDim(struct gridWidget *gw)
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


void mkRackWidget(struct gridWidget *gw)
{
	NOT(gw);
	
	gw->width   = RACK_SIZE;
	gw->height  = 1;
	gw->onPress = onPressRack;
	mkGridWidgetByDim(gw);
}


void mkChoiceWidget(struct gridWidget *gw)
{
	NOT(gw);
	
	gw->width   = CHOICE_COUNT;
	gw->height  = 1;
	gw->onPress = onPressChoice;
	mkGridWidgetByDim(gw);
}


void mkBoardWidget(struct gridWidget *gw)
{
	NOT(gw);
	
	gw->width   = BOARD_X;
	gw->height  = BOARD_Y;
	gw->onPress = onPressBoard;
	mkGridWidgetByDim(gw);
}


void gridWidgetDraw(SDL_Surface *s, struct gridWidget *gw, struct coor pos, struct coor dim)
{
	int y, x;
	SDL_Rect clip;
	Uint32 red, green, color;

	NOT(gw);

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


