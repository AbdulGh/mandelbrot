#ifndef MAND_H
#define MAND_H

void initMandelbrot(void);
void redraw(void);
void resize(void);
void cleanupMandelbrot(void);

typedef enum {
  LEFT, RIGHT, UP, DOWN, IN, OUT
} Direction ;
void shift(Direction direction);

#endif
