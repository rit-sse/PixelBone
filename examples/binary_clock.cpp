#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "../matrix.hpp"

#define HEIGHT 8
#define WIDTH 16

#define DIGIT_WIDTH 2
#define DIGIT_HEIGHT 2

const uint16_t color_hour = PixelBone_Matrix::Color(0x00, 0xff, 0xff);
const uint16_t color_min = PixelBone_Matrix::Color( 0x00, 0xf0, 0x00);
const uint16_t color_sec = PixelBone_Matrix::Color( 0xff, 0xff, 0x00);
const uint16_t color_empty = PixelBone_Matrix::Color( 0x00, 0x00, 0x00);


void draw_clock(PixelBone_Matrix *matrix,  tm *t) {
  int tmp;

  uint16_t x = DIGIT_WIDTH;
  uint16_t y = HEIGHT - DIGIT_HEIGHT;

  /* Clear everything. */
  matrix->clear();
  tmp = t->tm_hour / 10;

  // printf("Drawing %d, %d\n", x, y);

  /* Draw hour (first digit). */
  while (tmp != 0) {
    if (tmp % 2 == 1) {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_hour);
    } else {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_empty);
    }
    y -= DIGIT_HEIGHT;
    tmp >>= 1;
  }

  x += DIGIT_WIDTH;
  y = HEIGHT - DIGIT_HEIGHT;
  tmp = t->tm_hour % 10;

  /* Draw hour (second digit). */
  while (tmp != 0) {
    if (tmp % 2 == 1) {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_hour);
    } else {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_empty);
    }
    y -= DIGIT_HEIGHT;
    tmp >>= 1;
  }

  x += DIGIT_WIDTH;
  y = HEIGHT - DIGIT_HEIGHT;
  tmp = t->tm_min / 10;

  /* Draw minute (first digit). */
  while (tmp != 0) {
    if (tmp % 2 == 1) {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_min);
    } else {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_empty);
    }
    y -= DIGIT_HEIGHT;
    tmp >>= 1;
  }

  x += DIGIT_WIDTH;
  y = HEIGHT - DIGIT_HEIGHT;
  tmp = t->tm_min % 10;

  /* Draw minute (second digit). */
  while (tmp != 0) {
    if (tmp % 2 == 1) {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_min);
    } else {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_empty);
    }
    y -= DIGIT_HEIGHT;
    tmp >>= 1;
  }

  x += DIGIT_WIDTH;
  y = HEIGHT - DIGIT_HEIGHT;
  tmp = t->tm_sec / 10;

  /* Draw second (first digit). */
  while (tmp != 0) {
    if (tmp % 2 == 1) {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_sec);
    } else {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_empty);
    }
    y -= DIGIT_HEIGHT;
    tmp >>= 1;
  }

  x += DIGIT_WIDTH;
  y = HEIGHT - DIGIT_HEIGHT;
  tmp = t->tm_sec % 10;
  /* Draw second (second digit). */
  while (tmp != 0) {
    if (tmp % 2 == 1) {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_sec);
    } else {
      matrix->drawRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, color_empty);
    }
    y -= DIGIT_HEIGHT;
    tmp >>= 1;
  }
}

int main(void) {
  PixelBone_Matrix* matrix = new PixelBone_Matrix(WIDTH,HEIGHT,
                          MATRIX_TOP + MATRIX_LEFT +
                          MATRIX_ROWS + MATRIX_ZIGZAG);

  struct tm *now;
  time_t t_curr;

  while (true) {
    /* Update time and draw clock here. */
    t_curr = time(NULL);
    now = localtime(&(t_curr));
    draw_clock(matrix, now);
    matrix->wait();
    matrix->show();
    matrix->moveToNextBuffer();
  }
  return 0;
}
