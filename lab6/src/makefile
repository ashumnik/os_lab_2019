CC=gcc
CFLAGS=-I.

all:
	$(CC) -o server server.c lib.c -lpthread
	$(CC) -o client client.c lib.c -lpthread

clean : 
	rm client server
