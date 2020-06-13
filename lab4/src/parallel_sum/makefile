CC=gcc
CFLAGS=-I.


parallel_sum: utils.o utils.h sum.o sum.h
	$(CC) -o parallel_sum utils.o sum.o parallel_sum.c -lpthread

sum.o : sum.h
	$(CC) -o sum.o -c sum.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

clean :
	rm utils.o parallel_sum sum.o
