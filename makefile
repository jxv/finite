

all:
	cc -ansi -Werror -Wall -pedantic -O2 -lc `sdl-config --cflags` `sdl-config --libs` -lSDL_image -o finite main.c core/core.c core/ai.c core/init.c ui/term.c ui/gui.c util/mem.c util/sdl.c -I core -I ui -I util
clean:
	rm finite
