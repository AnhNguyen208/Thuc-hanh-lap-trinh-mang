CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: run

run: main.o
	${CC} main.o -o run

main.o: main.c
	${CC} ${CFLAGS} main.c

clean:
	rm -f *.o *~