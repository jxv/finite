include Makefile.inc

CFLAGS += $(shell sdl-config --cflags)
LIB += $(shell sdl-config --libs)


%.o:	%.c
	$(CC) $(CFLAGS) $(INC) $(LIB) -o $@ -c $<

all:	$(OBJ)
	$(CC) $(OBJ) -o $(OUT) $(CFLAG) $(INC) $(LIB)  
clean:
	$(RM) $(OUT) $(OBJ)
