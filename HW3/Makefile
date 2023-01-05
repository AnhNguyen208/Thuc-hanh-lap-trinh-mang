CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

run: main.o
	${CC} main.o -o resolver

main.o: main.c
	${CC} ${CFLAGS} main.c

clean:
	rm -f *.o *~