#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "mand.h"
#include "common.h"
#include "framebuffer.h"

static size_t histogram[MAX_ITERATIONS];
static size_t histogramMass;

size_t* escapeTimes = NULL;

size_t getEscapeMargin(size_t index) {
  size_t i = index / w;
  size_t j = index % w;

  FloatType x0 = viewport_l + ((FloatType)j / w) * (viewport_r - viewport_l);
  FloatType y0 = viewport_t - ((FloatType)i / h) * (viewport_t - viewport_b);

  FloatType x = 0;
  FloatType y = 0;

  size_t remainingIterations = MAX_ITERATIONS;
  while (x * x + y * y <= 4 && --remainingIterations > 0) {
    FloatType t = x * x - y * y + x0;
    y = 2 * x * y + y0;
    x = t;
  }

  return remainingIterations;
}

void initMandelbrot(void) {
  escapeTimes = (size_t*) mallocAligned(w * h * sizeof(size_t));

  histogramMass = 0;
  for (size_t i = 0; i < MAX_ITERATIONS; ++i) {
    histogram[i] = 0;
  }
  
  //dont bother with multithreading as we only do this once at startup
  for (size_t index = 0; index < w * h; ++index) {
      size_t remainingIterations = getEscapeMargin(index);
      if (remainingIterations != 0) {
        ++histogram[remainingIterations];
        ++histogramMass;
        escapeTimes[index] = remainingIterations;
      }
  }

  for (size_t i = 1; i < MAX_ITERATIONS; ++i) {
    histogram[i] = histogram[i - 1] + histogram[i];
  }

  for (size_t index = 0; index < w * h; ++index) {
    FloatType myPixelValue = (FloatType)histogram[escapeTimes[index]] / histogramMass;
    if (myPixelValue == 0) erase(index);
    else draw(index, myPixelValue);
  }
}

void resize(void) {
  free(escapeTimes);
  escapeTimes = (size_t*) mallocAligned(w * h * sizeof(size_t));
  redraw();
}

void* redrawWorker(void* ptr) {
  size_t threadid = (size_t) ptr;
  size_t area = w * h;
  size_t workload = area / 64 * NUM_THREADS + (area % (64 * NUM_THREADS) != 0);
  size_t myFirstIndex = workload * threadid; 
  size_t top = threadid == NUM_THREADS - 1? area : myFirstIndex + workload;

  for (size_t index = myFirstIndex; index < top; ++index) {
    FloatType myPixelValue = (FloatType)histogram[getEscapeMargin(index)] / histogramMass;
    if (myPixelValue == 0) erase(index);
    else draw(index, myPixelValue);
  }

  return NULL;
}

void redraw(void) {
  pthread_t threads[NUM_THREADS];

  for (size_t i = 0; i < NUM_THREADS; ++i) {
    pthread_create(&threads[i], NULL, &redrawWorker, (void*)i);
  }

  for (size_t i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }
}

void shift(Direction direction) {
  double m = 1.3;
  int shift = 8;
  FloatType xgap = (viewport_r - viewport_l) / shift;
  FloatType ygap = (viewport_b - viewport_t) / shift;
  FloatType midx = (viewport_l + viewport_r) / 2;
  FloatType midy = (viewport_t + viewport_b) / 2;

  switch(direction) {
    case IN:
      viewport_l = (viewport_l + (m - 1) * midx) / m;
      viewport_r = (viewport_r + (m - 1) * midx) / m;
      viewport_b = (viewport_b + (m - 1) * midy) / m;
      viewport_t = (viewport_t + (m - 1) * midy) / m;
      break;
    case OUT:
      viewport_l = (1 - m) * midx + m * viewport_l;
      viewport_r = (1 - m) * midx + m * viewport_r;
      viewport_b = (1 - m) * midy + m * viewport_b;
      viewport_t = (1 - m) * midy + m * viewport_t;
      break;
    case LEFT:
      viewport_l += xgap;
      viewport_r += xgap;
      break;
    case RIGHT:
      viewport_l -= xgap;
      viewport_r -= xgap;
      break;
    case UP:
      viewport_t -= ygap;
      viewport_b -= ygap;
      break;
    case DOWN:
      viewport_t += ygap;
      viewport_b += ygap;
      break;
    default:
    __builtin_unreachable();
  }
  redraw();
  show();
}

void cleanupMandelbrot(void) {
  free(escapeTimes);
}

