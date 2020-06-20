#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>
#include <getopt.h>

uint32_t result = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

struct FactArgs {

  uint32_t mod;
  uint32_t begin;
  uint32_t end;

};

int ModFact(struct FactArgs *args) {
  
  int factorial = 1;
  for(int i = args->begin; i <= args->end; i++){
      factorial *= i;
      factorial %= args->mod;
  }

  pthread_mutex_lock(&mut);      
  result *= factorial;
  result %= args->mod;
  pthread_mutex_unlock(&mut);
  return 0;
}

void *ThreadFact(void *args) {
  struct FactArgs *fact_args = (struct FactArgs *)args;
  return (void *)(size_t)ModFact(fact_args);
}

int main(int argc, char **argv) {

  uint32_t factorial = 0;
  uint32_t threads_num = 0;
  uint32_t mod = 0;

  pthread_t threads[threads_num];

   while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"mod", required_argument, 0, 0},
                                      {"k", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            mod = atoi(optarg);
              if (mod <= 0) {
                printf("mod is a positive number\n");
                 return 1;
                }
            break;
          case 1:
            factorial = atoi(optarg);
             if (factorial <= 0) {
                printf("k is a positive number\n");
                 return 1;
                }
            break;
          case 2:
            threads_num = atoi(optarg);
             if (threads_num <= 0) {
                printf("pnum is a positive number\n");
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

  if (mod == 0 || factorial == 0 || threads_num == 0) {
    printf("Usage: %s --mod \"num\" --k \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  struct FactArgs args[threads_num];
  int part = factorial/threads_num;

  for(int i = 0; i < threads_num; i++){
    if (i == threads_num - 1) {
      args[i].mod = mod;
      args[i].begin = (part * i) + 1;
      args[i].end = factorial;
    }
    else {
      args[i].mod = mod;
      args[i].begin = (part * i) + 1;
      args[i].end = part * (i + 1);
    }
  }

  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadFact, (void *)&(args[i]))) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

   for (uint32_t i = 0; i < threads_num; i++) {
     if (pthread_join(threads[i], NULL) != 0) {
       perror("pthread_join");
       exit(1);
     }
   }


  printf("mod factorial = %i\n", result);
  exit(0);
}
