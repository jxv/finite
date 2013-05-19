all:
	cc -ansi -Werror -Wall -pedantic -O2 -lc `sdl-config --cflags` `sdl-config --libs` -lSDL_image -o scabs main.c core.c ai.c dict.c mem.c term.c init.c gui.c sdl_util.c
clean:
	rm scabs
