#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "common.h"

void initFrameBuffer(void);

void cleanupFrameBuffer(void);

void show();

void draw(size_t index, FloatType escapeTime);

void erase(size_t index);

#endif
