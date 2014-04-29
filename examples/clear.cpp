/** \file
 * Test the ledscape library by pulsing RGB on the first three LEDS.
 */
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <cinttypes>
#include <cerrno>
//#include <unistd.h>
#include "../pixel.hpp"

int main(void) {
  const int num_pixels = 512;
  PixelBone_Pixel strip(num_pixels);
  strip.clear();
  strip.show();
  return 0;
}
