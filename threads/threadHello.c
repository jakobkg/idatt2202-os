#include <stdio.h>
#include <pthread.h>

#define NTHREADS 10

pthread_t threads[NTHREADS];

void *go (void *n) {
  printf("hello from thread %ld\n", (long)n);
  pthread_exit(100 + n);
}

int main (int argc, char *argv[])
{
  long i;
  for (i = 0; i < NTHREADS; i++) {
    pthread_create(threads[i], NULL, go, (void*)i);
  }

  for (i = 0; i < NTHREADS; i++) {
    long exitVal;
    pthread_join(threads[i], (void*)&exitVal);
    printf("Thread %ld returned tirh %ld\n", i, exitVal);
  }
  return 0;
}
