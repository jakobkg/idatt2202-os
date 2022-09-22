#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>

int main(int argc, char *argv[]) {
  char input[256];

  while (1) {
    printf("$ ");
    fflush(stdout);

    if (fgets(input, 256, stdin) != NULL) {

      if (input[strlen(input) - 1])
        input[strlen(input) - 1] = '\0';

      printf("You entered \"%s\"\n", input);

      char *newargv[] = {input, NULL};

      int pid = fork();

      if (pid == -1) {
        error(1, ECHILD, "fork() failed");
      }
      if (pid == 0) {
        execve(input, newargv, NULL);
      } else {
        printf("Child spawned %d\n", pid);
        wait(NULL);
      }
    }
    else {
      exit(0);
    }
  }
  return 0;
}
