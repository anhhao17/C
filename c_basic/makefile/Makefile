
CC=gcc
CFLAGS= -I../
INC1= hellomake.h
OBJ= hellomake.o hellofunc.o

%.o : %.c $(INC1)
	$(CC) -c -o $@ $< 
hellomake: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm $(OBJ) hellomake