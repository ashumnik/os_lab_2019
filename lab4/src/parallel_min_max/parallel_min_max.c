#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>  

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

#define STDIN 0

//struct timeval { 
//    long    tv_sec;         /* seconds */
//    long    tv_usec;        /* microseconds */
//};

pid_t *child_pids = NULL;
int pids_num = 0;

void KillChild(int signal)
{
	//printf("pids_num %d\n", pids_num);
	for(int i = 0; i < pids_num; i++){
		printf("killing %d\n", child_pids[i]);
		//fflush(stdout);
		kill(child_pids[i], SIGTERM);
	}
}

int main(int argc, char **argv) {
	int seed = -1;
	int array_size = -1;
	int pnum = -1;
	int time = 0;
	bool with_files = false;
	while (true)
	{
		int current_optind = optind ? optind : 1;
		static struct option options[] = { {"seed", required_argument, 0, 0},
						  {"array_size", required_argument, 0, 0}, 
						  {"pnum", required_argument, 0, 0},
						  {"by_files", no_argument, 0, 'f'},
						  {"timeout", required_argument, 0,0},
						  {0, 0, 0, 0} }; 
		int option_index = 0;
		int c = getopt_long(argc, argv, "f", options, &option_index); 

		if (c == -1) break; 

		switch (c) {
		case 0:
			switch (option_index) {
			case 0:
				seed = atoi(optarg);
				if (!seed) {
              				printf("Error: seed is a positive number\n");
           			        return -1;
				}
				break;
			case 1:
				array_size = atoi(optarg);

				if (array_size < 0) {
					printf("Error: array size is a positive number\n");
					return -1;
				}
				break;
			case 2:
				pnum = atoi(optarg);

				if (pnum <= 0) {
					printf("Error: pnum is a positive number\n");
					return -1;
				}
				break;
			case 3:
				with_files = true;
				break;
			case 4:
				time = atoi(optarg);
				if (time <= 0) {
					printf("Error: time of alarm is a positive number\n");
					return -1;
				}
				break;

			defalut:
				printf("Index %d is out of options\n", option_index);
			}
			break;
		case 'f':
			with_files = true;
			break;

		case '?':
			break;

		default:
			printf("getopt returned character code 0%o?\n", c);
		}
	}
	if (optind < argc)
	{
		printf("Has at least one no option argument\n");
		return 1;
	}

	if (seed == -1 || array_size == -1 || pnum == -1) {
		printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\"  \n",
			argv[0]);
		return 1;
	}

	int* array = malloc(sizeof(int) * array_size);
	GenerateArray(array, array_size, seed);

	int active_child_processes = 0;

	struct timeval start_time;
	gettimeofday(&start_time, NULL);

	int file_pipes[2];
	pipe(file_pipes);

	child_pids = (pid_t*)malloc(pnum * sizeof(pid_t));    
	pids_num = pnum;
	for (int i = 0; i < pnum; i++) {
		pid_t child_pid = fork(); 
		child_pids[i] = child_pid; 
		if (child_pid >= 0) {
			active_child_processes += 1;
			if (child_pid == 0) {
				sleep(10); //остановка дочернего роцесса на 10 сек
				struct MinMax min_max;
				int Part_Size = array_size / pnum;
				if (i == pnum - 1) {
					min_max = GetMinMax(array, i*Part_Size, array_size);
				}
				else {
					min_max = GetMinMax(array, i*Part_Size, (i + 1)*Part_Size);
				}

				if (with_files) {
					FILE *f;
					if (i == 0)f = fopen("data.txt", "w"); 
					else f = fopen("data.txt", "a+"); 

					fprintf(f, "%d\n", min_max.min);
					fprintf(f, "%d\n", min_max.max);
					fclose(f);
				}
				else {
					write(file_pipes[1], &min_max, sizeof(struct MinMax));
				}
				return 0;
			}
		}
		else {
			printf("Fork failed!\n");
			return 1;
		}
	}

	//printf("aaa\n");
	//printf("time %d\n", time);
	signal(SIGALRM, KillChild);
	alarm(time);

	
	while (active_child_processes > 0) {
		int status;
		wait(&status);

		active_child_processes -= 1;
	}
	
	//printf("ddddd\n");
	struct MinMax min_max;
	min_max.min = INT_MAX;
	min_max.max = INT_MIN;

	for (int i = 0; i < pnum; i++) {
		int min = INT_MAX;
		int max = INT_MIN;

		if (with_files) {
			struct MinMax NewMinMax;
			FILE *f;
			f = fopen("data.txt", "r");
			for (int j = 0; j < i; j++) {
				fscanf(f, "%d", &NewMinMax.min);
				fscanf(f, "%d", &NewMinMax.min);
			}
			fscanf(f, "%d", &NewMinMax.min);
			fscanf(f, "%d", &NewMinMax.max);
			fclose(f);
			min = NewMinMax.min;
			max = NewMinMax.max;

		}
		else {
/////////////////////////////////////////////////////////////////////////////////////////////
			struct MinMax NewMinMax;
			struct timeval timeout; 
			fd_set set; 
			timeout.tv_sec = 5; 
			timeout.tv_usec = 0; 
			
			FD_ZERO(&set);
			FD_SET(file_pipes[0], &set);

			int ret = select(FD_SETSIZE+1, &set, NULL, NULL, &timeout);

			if (ret == -1) {
			  printf("ERROR!\n");
			  return -1;
			}
			else if (ret == 0) {
 			  printf("TIMEOUT\n");
			  return -1;
			}
			else {
 			  if (FD_ISSET(file_pipes[0], &set))
			    read(file_pipes[0], &NewMinMax, sizeof(struct MinMax));
			}
			
			min = NewMinMax.min;
			max = NewMinMax.max;

		}
/////////////////////////////////////////////////////////////////////////////////////////////
		if (min < min_max.min) min_max.min = min;
		if (max > min_max.max) min_max.max = max;
	}
	struct timeval finish_time;
	gettimeofday(&finish_time, NULL);

	double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
	elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

	free(array);

	printf("Min: %d\n", min_max.min);
	printf("Max: %d\n", min_max.max);
	printf("Elapsed time: %fms\n", elapsed_time);
	free(child_pids);
	fflush(NULL);
	return 0;
}
