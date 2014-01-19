#include "pixelbone.hpp"
#include <iostream>

/* GPIO pins used */
static const uint8_t gpios0 = 2;

PixelBone::PixelBone(uint16_t pixel_count)
    : pru0(pru_init(0)), num_pixels(pixel_count),
      buffer_size(pixel_count * sizeof(pixel_t)) {
  if (2 * buffer_size > pru0->ddr_size)
    die("Pixel data needs at least 2 * %zu, only %zu in DDR\n", buffer_size,
        pru0->ddr_size);

  ws281x = (ws281x_command_t *)pru0->data_ram;
  *(ws281x) =
      ws281x_command_t((unsigned) pixel_count);

  // Configure all of our output pins.
  pru_gpio(0, gpios0, 1, 0);

  // Initiate the PRU0 program
  pru_exec(pru0, "./ws281x.bin");

  // Watch for a done response that indicates a proper startup
  // TODO: timeout if it fails
  std::cout << "waiting for response from pru0... ";
  while (!ws281x->response)
    ;
  std::cout << "OK" << std::endl;
};

PixelBone::~PixelBone() {
  ws281x->command = 0xFF;
  pru_close(pru0);
}

void PixelBone::show(void) {
  ws281x->pixels_dma = pru0->ddr_addr + buffer_size * current_buffer_num;

  // Wait for any current command to have been acknowledged
  while (ws281x->command)
    ;

  // Send the start command
  ws281x->command = 1;
}

void PixelBone::moveToNextBuffer() {
  ++current_buffer_num %= 2;
};

uint16_t PixelBone::numPixels() const { return num_pixels; }

/** Retrieve one of the two frame buffers. */
pixel_t *PixelBone::getCurrentBuffer() const {
  return (pixel_t *)((uint8_t *)pru0->ddr + buffer_size * current_buffer_num);
}

pixel_t *PixelBone::getPixel(uint16_t n) const {
  return &getCurrentBuffer()[n];
}

void PixelBone::setPixelColor(uint8_t n, uint8_t r, uint8_t g, uint8_t b) {
  pixel_t *const p = getPixel(n);
  p->r = r;
  p->g = g;
  p->b = b;
}

void PixelBone::setPixelColor(uint8_t n, pixel_t c) {
  setPixelColor(n, c.r, c.g, c.b);
}

pixel_t PixelBone::Color(uint8_t r, uint8_t g, uint8_t b) {
  pixel_t p;
  p.r = r;
  p.g = g;
  p.b = b;
  return p;
}

uint32_t PixelBone::wait() {
  while (1) {
    uint32_t response = ws281x->response;

    //printf("pru0: (%d,%d)\n", ws281x->command, ws281x->response);

    if (response) {
      ws281x->response = 0;
      return response;
    }
  }
}