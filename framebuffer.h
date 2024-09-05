#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "common.h"

void initFrameBuffers(void);

void cleanupFrameBuffers(void);

void show();

void draw(size_t index, FloatType escapeTime);

void erase(size_t index);

#endif
