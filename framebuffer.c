#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "framebuffer.h"

char* framebuffer;
static const char* stringTemplate = "\033[38;2;000;000;000mX";
const size_t ST_R = 7;
const size_t ST_C = 19;
const size_t ST_SIZE = 20;

void initFrameBuffers(void) {
  size_t rowSize = ST_SIZE * w;

  framebuffer = (char*) mallocAligned(h * (rowSize + 1) + 1);

  for (int row = 0; row < h; ++row) {
    char* currentRow = framebuffer + row * rowSize;
    for (size_t writeHead = 0; writeHead < rowSize; ++writeHead) {
      currentRow[writeHead] = stringTemplate[writeHead % ST_SIZE];
    }
    currentRow[rowSize] = '\n';
  }

  framebuffer[h * (rowSize + 1)] = '\0';
}

//todo rename
void cleanupFrameBuffers(void) {
  free(framebuffer);
}

void show(int frame) {
  fwrite(framebuffer, 1, w * h * ST_SIZE, stdout);
}

void write3Digits(int digits, char* location) {
  for (int i = 2; i >= 0; --i) {
    location[i] = '0' + digits % 10;
    digits /= 10;
  }
}

void draw(size_t index, FloatType escapeTime) {
  char* location = framebuffer + ST_SIZE * index; 

  char c = 33 + (126 - 33) * escapeTime;
  int r = 255 * escapeTime;
  int g = 255 * (1 - escapeTime);
  int b = 255 * (1 - escapeTime);

  write3Digits(r % 255, location + ST_R);
  write3Digits(g % 255, location + ST_R + 4);
  write3Digits(b % 255, location + ST_R + 8);
  location[ST_C] = c;
}

void erase(size_t index) {
  char* location = framebuffer + ST_SIZE * index; 
  location[ST_C] = ' ';
}
