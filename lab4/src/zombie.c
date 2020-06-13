#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	pid_t child_pid;
	child_pid = fork();
	if (child_pid > 0) { //родительский процесс
		sleep(60);
	}
	else { //дочерний процесс
		exit(0);
	}
	return 0;
}
