CC=gcc
CFLAGS=-I.

all:parallel_min_maxm zombie


zombie: zombie.c
	$(CC) -o zombie -c zombie.c $(CFLAGS)

parallel_min_maxm:
	make -C ./parallel_min_max -f makefile

clean :
	rm zombie
