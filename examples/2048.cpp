/*
 This program is based on the 2048 implementation by Maurits van der Schee.
 This program is licensed under the MIT License.
 */

#define _XOPEN_SOURCE 500
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctime>
#include <signal.h>
#include "../matrix.hpp"

#define SIZE 4

uint32_t score = 0;

uint16_t getColor(uint16_t value) {
  if (value == 2) {
    return PixelBone_Matrix::Color(238, 228, 218);
  } else if (value == 4) {
    return PixelBone_Matrix::Color(237, 224, 200);
  } else if (value == 8) {
    return PixelBone_Matrix::Color(242, 177, 121);
  } else if (value == 16) {
    return PixelBone_Matrix::Color(245, 149, 99);
  } else if (value == 32) {
    return PixelBone_Matrix::Color(246, 124, 95);
  } else if (value == 64) {
    return PixelBone_Matrix::Color(246, 94, 59);
  } else if (value == 128) {
    return PixelBone_Matrix::Color(237, 207, 114);
  } else if (value == 256) {
    return PixelBone_Matrix::Color(237, 204, 97);
  } else if (value == 512) {
    return PixelBone_Matrix::Color(237, 200, 80);
  } else if (value == 1024) {
    return PixelBone_Matrix::Color(237, 197, 63);
  } else if (value == 2048) {
    return PixelBone_Matrix::Color(237, 194, 46);
  } else {
    return PixelBone_Matrix::Color(0, 0, 0);
  }
}

void drawBoard(PixelBone_Matrix &matrix, uint16_t board[SIZE][SIZE]) {
  // matrix.drawPixel(2,2, PixelBone_Matrix::Color(255,255,255));
  // matrix.show();
  matrix.clear();
  for (int16_t y = 0; y < SIZE; y++) {
    for (int16_t x = 0; x < SIZE; x++) {
      uint16_t color = getColor(board[x][y]);
      matrix.fillRect(x * 4, y * 4, 4, 4, color);
      // printf("(%d, %d): %d ->  %x\n", x, y, board[x][y],  color);
    }
  }
  matrix.show();
}

int8_t findTarget(uint16_t array[SIZE], int8_t x, int8_t stop) {
  int8_t t;
  // if the position is already on the first, don't evaluate
  if (x == 0) {
    return x;
  }
  for (t = x - 1; t >= 0; t--) {
    if (array[t] != 0) {
      if (array[t] != array[x]) {
        // merge is not possible, take next position
        return t + 1;
      }
      return t;
    } else {
      // we should not slide further, return this one
      if (t == stop) {
        return t;
      }
    }
  }
  // we did not find a
  return x;
}

bool slideArray(uint16_t array[SIZE]) {
  bool success = false;
  int8_t x, t, stop = 0;

  for (x = 0; x < SIZE; x++) {
    if (array[x] != 0) {
      t = findTarget(array, x, stop);
      // if target is not original position, then move or merge
      if (t != x) {
        // if target is not zero, set stop to avoid double merge
        if (array[t] != 0) {
          score += array[t] + array[x];
          stop = t + 1;
        }
        array[t] += array[x];
        array[x] = 0;
        success = true;
      }
    }
  }
  return success;
}

void rotateBoard(uint16_t board[SIZE][SIZE]) {
  int8_t i, j, n = SIZE;
  uint16_t tmp;
  for (i = 0; i < n / 2; i++) {
    for (j = i; j < n - i - 1; j++) {
      tmp = board[i][j];
      board[i][j] = board[j][n - i - 1];
      board[j][n - i - 1] = board[n - i - 1][n - j - 1];
      board[n - i - 1][n - j - 1] = board[n - j - 1][i];
      board[n - j - 1][i] = tmp;
    }
  }
}

bool moveUp(uint16_t board[SIZE][SIZE]) {
  bool success = false;
  int8_t x;
  for (x = 0; x < SIZE; x++) {
    success |= slideArray(board[x]);
  }
  return success;
}

bool moveLeft(uint16_t board[SIZE][SIZE]) {
  bool success;
  rotateBoard(board);
  success = moveUp(board);
  rotateBoard(board);
  rotateBoard(board);
  rotateBoard(board);
  return success;
}

bool moveDown(uint16_t board[SIZE][SIZE]) {
  bool success;
  rotateBoard(board);
  rotateBoard(board);
  success = moveUp(board);
  rotateBoard(board);
  rotateBoard(board);
  return success;
}

bool moveRight(uint16_t board[SIZE][SIZE]) {
  bool success;
  rotateBoard(board);
  rotateBoard(board);
  rotateBoard(board);
  success = moveUp(board);
  rotateBoard(board);
  return success;
}

bool findPairDown(uint16_t board[SIZE][SIZE]) {
  bool success = false;
  int8_t x, y;
  for (x = 0; x < SIZE; x++) {
    for (y = 0; y < SIZE - 1; y++) {
      if (board[x][y] == board[x][y + 1])
        return true;
    }
  }
  return success;
}

int16_t countEmpty(uint16_t board[SIZE][SIZE]) {
  int8_t x, y;
  int16_t count = 0;
  for (x = 0; x < SIZE; x++) {
    for (y = 0; y < SIZE; y++) {
      if (board[x][y] == 0) {
        count++;
      }
    }
  }
  return count;
}

bool gameEnded(uint16_t board[SIZE][SIZE]) {
  bool ended = true;
  if (countEmpty(board) > 0)
    return false;
  if (findPairDown(board))
    return false;
  rotateBoard(board);
  if (findPairDown(board))
    ended = false;
  rotateBoard(board);
  rotateBoard(board);
  rotateBoard(board);
  return ended;
}

void addRandom(uint16_t board[SIZE][SIZE]) {
  static bool initialized = false;
  int8_t x, y;
  int16_t r, len = 0;
  uint16_t n, list[SIZE * SIZE][2];

  if (!initialized) {
    srand(time(NULL));
    initialized = true;
  }

  for (x = 0; x < SIZE; x++) {
    for (y = 0; y < SIZE; y++) {
      if (board[x][y] == 0) {
        list[len][0] = x;
        list[len][1] = y;
        len++;
      }
    }
  }

  if (len > 0) {
    r = rand() % len;
    x = list[r][0];
    y = list[r][1];
    n = ((rand() % 10) / 9 + 1) * 2;
    board[x][y] = n;
  }
}

void setBufferedInput(bool enable) {
  static bool enabled = true;
  static struct termios old;
  struct termios newt;

  if (enable && !enabled) {
    // restore the former settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    // set the new state
    enabled = true;
  } else if (!enable && enabled) {
    // get the terminal settings for standard input
    tcgetattr(STDIN_FILENO, &newt);
    // we want to keep the old setting to restore them at the end
    old = newt;
    // disable canonical mode (buffered i/o) and local echo
    newt.c_lflag &= (~ICANON & ~ECHO);
    // set the new settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    // set the new state
    enabled = false;
  }
}

int test() {
  uint16_t array[SIZE];
  uint16_t data[] = { 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 4, 0, 0, 0, 0, 2,
                      0, 2, 4, 0, 0, 0, 2, 0, 0, 2, 4, 0, 0, 0, 2, 0, 2, 0,
                      4, 0, 0, 0, 2, 2, 2, 0, 4, 2, 0, 0, 2, 0, 2, 2, 4, 2,
                      0, 0, 2, 2, 0, 2, 4, 2, 0, 0, 2, 2, 2, 2, 4, 4, 0, 0,
                      4, 4, 2, 2, 8, 4, 0, 0, 2, 2, 4, 4, 4, 8, 0, 0, 8, 0,
                      2, 2, 8, 4, 0, 0, 4, 0, 2, 2, 4, 4, 0, 0 };
  uint16_t *in, *out;
  uint16_t t, tests;
  uint8_t i;
  bool success = true;

  tests = (sizeof(data) / sizeof(data[0])) / (2 * SIZE);
  for (t = 0; t < tests; t++) {
    in = data + t * 2 * SIZE;
    out = in + SIZE;
    for (i = 0; i < SIZE; i++) {
      array[i] = in[i];
    }
    slideArray(array);
    for (i = 0; i < SIZE; i++) {
      if (array[i] != out[i]) {
        success = false;
      }
    }
    if (success == false) {
      for (i = 0; i < SIZE; i++) {
        printf("%d ", in[i]);
      }
      printf("=> ");
      for (i = 0; i < SIZE; i++) {
        printf("%d ", array[i]);
      }
      printf("expected ");
      for (i = 0; i < SIZE; i++) {
        printf("%d ", in[i]);
      }
      printf("=> ");
      for (i = 0; i < SIZE; i++) {
        printf("%d ", out[i]);
      }
      printf("\n");
      break;
    }
  }
  if (success) {
    printf("All %u tests executed successfully\n", tests);
  }
  return !success;
}

void signal_callback_handler(int signum) {
  printf("         TERMINATED         \n");
  setBufferedInput(true);
  exit(signum);
}

int main(int argc, char *argv[]) {
  uint16_t board[SIZE][SIZE];
  char c;
  bool success;
  PixelBone_Matrix* matrix = new PixelBone_Matrix(16,8,2,2, 
                          MATRIX_TOP + MATRIX_LEFT + 
                          MATRIX_ROWS + MATRIX_ZIGZAG + 
                          TILE_TOP + TILE_LEFT + TILE_ROWS);


  if (argc == 2 && strcmp(argv[1], "test") == 0) {
    return test();
  }


  // register signal handler for when ctrl-c is pressed
  signal(SIGINT, signal_callback_handler);

  memset(board, 0, sizeof(board));
  addRandom(board);
  addRandom(board);
  drawBoard(*matrix, board);
  setBufferedInput(false);
  while (true) {
    c = getchar();
    switch (c) {
    case 97:  // 'a' key
    case 104: // 'h' key
    case 68:  // left arrow
      success = moveLeft(board);
      break;
    case 100: // 'd' key
    case 108: // 'l' key
    case 67:  // right arrow
      success = moveRight(board);
      break;
    case 119: // 'w' key
    case 107: // 'k' key
    case 65:  // up arrow
      success = moveUp(board);
      break;
    case 115: // 's' key
    case 106: // 'j' key
    case 66:  // down arrow
      success = moveDown(board);
      break;
    default:
      success = false;
    }
    if (success) {
      drawBoard(*matrix, board);
      usleep(150000);
      addRandom(board);
      drawBoard(*matrix, board);
      if (gameEnded(board)) {
        printf("         GAME OVER          \n");
        break;
      }
    }
    if (c == 'q') {
      printf("        QUIT? (y/n)         \n");
      while (true) {
        c = getchar();
        if (c == 'y') {
          setBufferedInput(true);
          exit(0);
        } else {
          drawBoard(*matrix, board);
          break;
        }
      }
    }
    if (c == 'r') {
      printf("       RESTART? (y/n)       \n");
      while (true) {
        c = getchar();
        if (c == 'y') {
          memset(board, 0, sizeof(board));
          addRandom(board);
          addRandom(board);
          drawBoard(*matrix, board);
          break;
        } else {
          drawBoard(*matrix, board);
          break;
        }
      }
    }
  }
  setBufferedInput(true);
  matrix.clear();
  delete matrix;

  return EXIT_SUCCESS;
}
