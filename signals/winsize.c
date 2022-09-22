#include <asm-generic/ioctls.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define BUFFER_SIZE 64

struct winsize win_size;
char charbuf[BUFFER_SIZE];

void print_size() {
  // Unregister signal handler so the function does not interrupt itself
  signal(SIGWINCH, SIG_IGN);

  // Place screen size info in win_size
  ioctl(0, TIOCGWINSZ, &win_size);

  // Create string "width x height" and place in buffer
  snprintf(charbuf, BUFFER_SIZE, "%dx%d", win_size.ws_col, win_size.ws_row);

  // Erase screen and move cursor halfway down
  printf("\033[2J");
  printf("\033[H");
  printf("\033[%dB", win_size.ws_row / 2);

  // Move cursor slightly less than halfway across the screen
  printf("\033[%dC", (int)(win_size.ws_col - strlen(charbuf)) / 2);

  // Print string from buffer
  printf("%s", charbuf);

  fflush(0);

  // Re-register as signal handler
  signal(SIGWINCH, print_size);
}

void quit() {
  printf("\033[2J"); // Clear screen
  printf("\033[H");  // Move cursor to top left
  printf("Goodbye\n");
  printf("\033[?25h"); // Make cursor visible again before quitting
  exit(0);
}

int main(int argc, char *argv[]) {
  // Make cursor invisible (very cool)
  printf("\033[?25l");

  // Register window change signal handler
  signal(SIGWINCH, print_size);

  // Register interrupt/kill signal handler
  signal(SIGINT, quit);
  signal(SIGTERM, quit);

  // Print screen info
  print_size();

  // Wait for more signals
  while (1) {
    pause();
  };

  return 0;
}
