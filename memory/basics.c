#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int staticVar = 0;

int main(int argc, char *argv[]) {
  int *heapVar = malloc(sizeof(int));
  int localVar = 10;

  staticVar += 1;
  *heapVar = 0;

  sleep(0);
  printf("static address: %8x, value %d\n", &staticVar, staticVar);
  printf("local address:  %8x, value %d\n", &localVar, localVar);
  printf("heap address:   %8x, value %d\n", heapVar, *heapVar);
  return 0;
}
