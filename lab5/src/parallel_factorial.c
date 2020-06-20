#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>
#include <getopt.h>


pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

struct FaktArgs {

  uint32_t faktorial;
  uint32_t mod;
  uint32_t current;
  uint32_t next;
  uint32_t part;

};

int ModFakt(struct FaktArgs *args) {
  
  uint32_t m = (*args).mod;
  uint32_t f = (*args).faktorial;
  uint32_t c = (*args).current;
  uint32_t n = (*args).next;
  uint32_t p = (*args).part;

  uint32_t i;
  for (i = 0; i < p; i++){
      pthread_mutex_lock(&mut);
      c = (c * n) % m;
      //printf("c= %i\n", c);
      n++;
      //printf("n= %i\n", n);
      
      (*args).next = n;
      (*args).current = c;
      if (n > f) {
          break;
      }
      pthread_mutex_unlock(&mut);
  }
  return 0;
}

void *ThreadFakt(void *args) {
  struct FaktArgs *fakt_args = (struct FaktArgs *)args;
  return (void *)(size_t)ModFakt(fakt_args);
}

int main(int argc, char **argv) {

  uint32_t faktorial = 0;
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
            faktorial = atoi(optarg);
             if (faktorial <= 0) {
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

  if (mod == 0 || faktorial == 0 || threads_num == 0) {
    printf("Usage: %s --mod \"num\" --k \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  struct FaktArgs args;
  args.current = 1;
  args.next = 2;
  args.mod = mod;
  args.faktorial = faktorial;
  args.part = faktorial/threads_num;

  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadFakt, (void *)&(args))) {
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


  printf("mod faktorial %i\n", args.current);
  exit(0);
}
