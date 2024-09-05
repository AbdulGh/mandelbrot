#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

#define MAX_ITERATIONS 2000
#define NUM_THREADS 8

typedef long double FloatType;

extern FloatType viewport_l;
extern FloatType viewport_r;
extern FloatType viewport_t;
extern FloatType viewport_b;

extern size_t w;
extern size_t h;
extern size_t cacheLineSize;

extern void* mallocAligned(size_t nbytes);

#endif
