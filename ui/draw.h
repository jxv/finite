#ifndef FINITE_UI_DRAW_H
#define FINITE_UI_DRAW_H

void strDraw(SDL_Surface *s, Font *f, const char *str, int x, int y);
void drawProgressBar(SDL_Surface *s, float progress, int x, int y, int w, int h, int sp);
void draw(Env *e);

#endif

