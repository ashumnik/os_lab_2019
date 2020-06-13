#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "utils.h"
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include "sum.h"


void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  /*
   *  TODO:
   *  threads_num by command line arguments
   *  array_size by command line arguments
   *	seed by command line arguments
   */
  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;
  /*
   * TODO:
   * your code here
   * Generate array here
   */
  while (true)
  {
  int current_optind = optind ? optind : 1;

  static struct option options[] = { {"threads_num", required_argument, 0, 0},
				    {"array_size", required_argument, 0, 0},
				    {"seed", required_argument, 0, 0},
				    {0, 0, 0, 0} };

  int option_index = 0;
  int c = getopt_long(argc, argv, "", options, &option_index);

  if (c == -1) break;

  switch (c) {
    case 0:
      switch (option_index) {
        case 0:
          threads_num = atoi(optarg);
          if (threads_num < 0) {
            printf("threads_num=%d < 0!\n", threads_num);
            return 1;
          }
          break;
          case 1:
            array_size = atoi(optarg);
            if (array_size < 0) {
              printf("Array size=%d < 0!\n", array_size);
              return 1;
            }
            if (threads_num > array_size) {
              printf("threads_num>Array size!\n");
            }
            break;
          case 2:
            seed = atoi(optarg);
            if (seed <= 0) {
              printf("seed =%d <= 0!\n", seed);
              return 1;
            }
            break;
          defalut:
            printf("Index %d is out of options\n", option_index);
          }
          break;
        case '?':
          break;
        default:
          printf("getopt returned character code 0%o?\n", c);
      }
    }
    if (optind < argc) {
      printf("Has at least one no option argument\n");
      return 1;
    }
    if (threads_num == -1 || array_size == -1 || seed == -1) {
      printf("Usage: %s --threads_num \"num\" --array_size \"num\" --seed \"num\"  \n", argv[0]);
      return 1;
    }

    pthread_t threads[threads_num];

    int *array = malloc(sizeof(int) * array_size);

    GenerateArray(array, array_size, seed);

    struct SumArgs args[threads_num];
    int Part_Size = array_size / threads_num;

    // start time
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    for (size_t i = 0; i < threads_num; i++) {		
      if (i == threads_num - 1) {
        args[i].array = array;
        args[i].begin = i * Part_Size;
        args[i].end = array_size;
        }
      else {
        args[i].array = array;
        args[i].begin = i * Part_Size;
        args[i].end = (i + 1)*Part_Size;
      }
    }
  
  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args)) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += (int)sum;
  }

  free(array);
  printf("Total: %d\n", total_sum);
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);
  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
  printf("Total: %d\n", total_sum);
  printf("Elapsed time: %fms\n", elapsed_time);
  return 0;
}
