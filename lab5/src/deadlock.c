#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

void do_one_thing(int *);
void do_another_thing(int *);

int res[2];
int r1 = 0, r2 = 0, r3 = 0;

pthread_mutex_t mut1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;

int main() {
  pthread_t thread1, thread2;

  if (pthread_create(&thread1, NULL, (void *)do_one_thing,
			  (void *)&res) != 0) {
    perror("pthread_create");
    exit(1);
  }

  if (pthread_create(&thread2, NULL, (void *)do_another_thing,
                     (void *)&res) != 0) {
    perror("pthread_create");
    exit(1);
  }

  if (pthread_join(thread1, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }

  if (pthread_join(thread2, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }


  return 0;
}

void do_one_thing(int *pnum_times) {

    pthread_mutex_lock(&mut1);
        sleep(1);
	pthread_mutex_lock(&mut2);
    printf("do_one_thing не выполнится\n");
    pthread_mutex_unlock(&mut1);
    pthread_mutex_unlock(&mut2);

  
}

void do_another_thing(int *pnum_times) {
 
    pthread_mutex_lock(&mut2);
       sleep(1);
    pthread_mutex_lock(&mut1);
    printf("do_another_thing не выполнится\n");
    pthread_mutex_unlock(&mut2);
    pthread_mutex_unlock(&mut1);
  
}
