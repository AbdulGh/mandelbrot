#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>

#include "common.h"
#include "framebuffer.h"
#include "mand.h"

size_t w;
size_t h;
size_t cacheLineSize = 64; //todo

FloatType viewport_l = -1.5;
FloatType viewport_r = 0.5;
FloatType viewport_t = 1.2;
FloatType viewport_b = -1.2;

void* mallocAligned(size_t nbytes) {
  void* ptr;
  posix_memalign(&ptr, cacheLineSize, nbytes);
  return ptr;
}

void getWidthAndHeight(void) {
  struct winsize windowInf;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowInf);
  w = windowInf.ws_col;
  h = windowInf.ws_row;
}

void cleanup(void) {
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  tty.c_lflag |= (ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
  fputs("\033[0m", stdout);
  cleanupFrameBuffer();
  cleanupMandelbrot();
}

void init(void) {
  getWidthAndHeight();
  initFrameBuffer();
  initMandelbrot();

  //disable line buffering to grab chars
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  tty.c_lflag &= ~(ICANON | ECHO);
  tty.c_cc[VMIN] = 1;
  tty.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void handleWINCH(int sig) {
  getWidthAndHeight();
  
  // pickup new sizes
  cleanupFrameBuffer();
  initFrameBuffer();
  resize();
}

int main(void) {
  init();

  for (;;) {
    char c = getchar();
    switch (c) {
      case 'h':
        shift(RIGHT);
        break;
      case 'j':
        shift(DOWN);
        break;
      case 'k':
        shift(UP);
        break;
      case 'l':
        shift(LEFT);
        break;
      case 'i':
        shift(IN);
        break;
      case 'o':
        shift(OUT);
        break;
      default:
    }
    if (c == 'q') break;
  }

  cleanup();
  return 0;
}
