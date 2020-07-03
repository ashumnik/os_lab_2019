CC=gcc
CFLAGS=-I.

all : server client

server : server
	$(CC) server.c -lpthread -L. -lmult_modulo -o server

client : client
	$(CC) client.c -lpthread -L. -lmult_modulo -o client

clean : 
	rm client server
