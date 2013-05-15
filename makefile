all:
	cc -ansi -Werror -Wall -pedantic `sdl-config --cflags` `sdl-config --libs` -lSDL_image -o scabs main.c
clean:
	rm scabs
