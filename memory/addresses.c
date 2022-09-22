#include <stdio.h>
#include <stdlib.h>


int main (int argc, char *argv[])
{
  int* a = malloc(sizeof(int));
  int b;

  printf("Address a: %x\n", a);
  printf("Address b: %x\n", &b);
  return 0;
}
