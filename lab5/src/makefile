CC=gcc
CFLAGS=-I.


all : mutex parallel_factorial

mutex : mutex
	$(CC) mutex.c -lpthread -o mutex $(CFLAGS)

parallel_factorial : parallel_factorial
	$(CC) parallel_factorial.c -lpthread -o parallel_factorial  $(CFLAGS)

clean : 
	rm  mutex parallel_factorial

