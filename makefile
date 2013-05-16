all:
	cc -ansi -Werror -Wall -pedantic -O2 `sdl-config --cflags` `sdl-config --libs` -lSDL_image -o scabs main.c core.c ai.c
clean:
	rm scabs
