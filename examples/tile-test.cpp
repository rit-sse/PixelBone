#include "../gfx.hpp"
#include "../matrix.hpp"

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = number of matrices arranged horizontally
// Parameter 4 = number of matrices arranged vertically
// Parameter 5 = matrix layout flags, add together as needed:
//   MATRIX_TOP, MATRIX_BOTTOM, MATRIX_LEFT, MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     MATRIX_TOP + MATRIX_LEFT for the top-left corner.
//   MATRIX_ROWS, MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   MATRIX_PROGRESSIVE, MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   TILE_TOP, TILE_BOTTOM, TILE_LEFT, TILE_RIGHT:
//     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
//     two, e.g. TILE_TOP + TILE_LEFT for the top-left corner.
//   TILE_ROWS, TILE_COLUMNS: the matrices in the OVERALL DISPLAY
//     are arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   TILE_PROGRESSIVE, TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
//     (tiles) in the OVERALL DISPLAY proceed in the same order for every
//     line, or alternate lines reverse direction; pick one.  When using
//     zig-zag order, the orientation of the matrices in alternate rows
//     will be rotated 180 degrees (this is normal -- simplifies wiring).
//   See example below for these values in action.

int main() {
  PixelBone_Matrix matrix(16,8,2,1,
  TILE_TOP   + TILE_LEFT   + TILE_ROWS   + TILE_PROGRESSIVE +
  MATRIX_TOP + MATRIX_LEFT + MATRIX_ROWS + MATRIX_ZIGZAG);

  matrix.setTextWrap(false);
  matrix.setTextColor(PixelBone_Pixel::Color(128, 128, 128));

  int x = 0;
  while (1) {
    matrix.fillScreen(0);
    matrix.setCursor(x, 0);
    matrix.print("Howdy");

    if (--x < -36)
      x = matrix.width();

    matrix.show();
    usleep(100000);
  }
  return 0;
}
