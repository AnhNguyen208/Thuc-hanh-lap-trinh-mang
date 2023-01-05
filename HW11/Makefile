CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: server client

server: select_server.o
	${CC} select_server.o -o server 

client: tcp_client.o
	${CC} tcp_client.o -o client

select_server.o: select_server.c
	${CC} ${CFLAGS} select_server.c 

tcp_client.o: tcp_client.c
	${CC} ${CFLAGS} tcp_client.c

clean:
	rm -f *.o client server