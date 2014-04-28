![Testing PixelBone](https://lh3.googleusercontent.com/-de4gV0F2_Gk/U1vb6bDet1I/AAAAAAAACJg/mGFfGTMWo4c/w1084-h813-no/IMG_20140426_121532.jpg)

#DANGER!

This code works with the PRU units on the BeagleBone and can easily
cause *hard crashes*.  It is still being debugged and developed.
Be careful hot-plugging things into the headers -- it is possible to
damage the pin drivers and cause problems in the ARM, especially if
there are +5V signals involved.


#Overview

This is a modified version of the LEDscape library designed to control a single chain of WS2811-based LED modules from a BeagleBone (Black). The timing has been updated and verified to work with both WS2812 and WS2812b chips. This version of the library uses a single PRU on the BeagleBone. This allows sending at about 60fps to strings of 512 pixels or at ~120fps for 256 pixels.

The bit-unpacking is handled by the PRU, which allows PixelBone to take almost no cpu time to run, freeing up time for the actual generation of animations or dealing with network protocols.


#Installation and Usage

To use PixelBone, download it to your BeagleBone Black.

First, make sure that PixelBone compiles:

```sh
make
```

Before PixelBone will function, you will need to replace the device tree
file and reboot.

There are two different dtb files in the `dirtrees` folder, one for the original BeagleBone, and one for the BeagleBone Black. There is also an overlay file for other operating systems (such as Arch Linux). Whichever you choose, place it in the proper folder. `/boot/` for the dtb file, or `/usr/lib/firmware/` for the overlay. Please check your distro to make sure this is correct.

Reboot if you're using the dtb, or run the provided `dtbo_loader.sh` script if using the dtbo. 

Connect a WS2811-based LED chain to the Beagle Bone. The strip must be running at the same voltage as the data signal. If you are using an external 5v supply for the LEDs, you'll need to use a level shifter or other technique to bring the BBB's 3.3v signals up to 5v.

Once everything is connected, run the `rgb-test` program:

```sh
./examples/rgb-test
```

The LEDs should now be fading prettily. If not, go back and make
sure everything is setup correctly.


#Pin Mapping

The mapping from PixelBone channel to BeagleBone GPIO pin:

```
		 PixelBone Channel Index
	 Row  Pin#       P9        Pin#
	  1    1                    2  
	  2    3                    4  
	  3    5                    6  
	  4    7                    8 
	  5    9                    10
	  6    11                   12
	  7    13                   14
	  8    15                   16
	  9    17                   18
	  10   19                   20
	  11   21    [1]     [0]    22
	  12   23                   24
	  13   25                   26
	  14   27                   28
	  15   29                   30
	  16   31                   32
	  17   33                   34
	  18   35                   36
	  19   37                   38
	  20   39                   40
	  21   41                   42
	  22   43                   44
	  23   45                   46

```

The numbers on the inside of each block indicate the PixelBone channel.


#Implementation Notes

The WS281x LED chips are built like shift registers and make for very easy LED strip construction.  The signals are duty-cycle modulated, with a 0 measuring 250 ns long and a 1 being 600 ns long, and 1250 ns between bits.  Since this doesn't map to normal SPI hardware and requires an 800 KHz bit clock, it is typically handled with a dedicated microcontroller or DMA hardware on something like the Teensy 3.

However, the TI AM335x ARM Cortex-A8 in the BeagleBone has two programmable "microcontrollers" built into the CPU that can handle realtime tasks and also access the ARM's memory.  This allows things that might have been delegated to external devices to be handled without any additional hardware, and without the overhead of clocking data out the USB port.

The frames are stored in memory as a series of 4-byte pixels in the order GRBA.  This means that it looks like this in RAM:

`S0P0 S0P1 S0P2 ... etc`

4 * 32 * length bytes are required per frame buffer.  The maximum frame rate also depends on the length.


API
===

`pixel.hpp` and `matrix.hpp` defines the API. The key components are:

```cpp
class PixelBone_Pixel {
public:
  PixelBone_Pixel(uint16_t pixel_count);
  void show(void);
  void clear(void);
  void setPixelColor(uint32_t n, uint8_t r, uint8_t g, uint8_t b);
  void setPixelColor(uint32_t n, uint32_t c);
  void moveToNextBuffer();
  uint32_t wait();
  uint32_t numPixels() const;
  uint32_t getPixelColor(uint32_t n) const;
  static uint32_t Color(uint8_t red, uint8_t green, uint8_t blue);
  static uint32_t HSB(uint16_t hue, uint8_t saturation, uint8_t brightness);
};

class PixelBone_Matrix{
public:
  // Constructor for single matrix:
  PixelBone_Matrix(int w, int h,
                   uint8_t matrixType = MATRIX_TOP + MATRIX_LEFT + MATRIX_ROWS);

  // Constructor for tiled matrices:
  PixelBone_Matrix(uint8_t matrixW, uint8_t matrixH, uint8_t tX, uint8_t tY,
                   uint8_t matrixType = MATRIX_TOP + MATRIX_LEFT + MATRIX_ROWS +
                                        TILE_TOP + TILE_LEFT + TILE_ROWS);

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillScreen(uint16_t color);
  static uint16_t Color(uint8_t r, uint8_t g, uint8_t b);
};
```

You can double buffer like this:

```cpp
const int num_pixels = 256;
PixelBone_Pixel strip(num_pixels);

while (1) {
	render(strip); //modify the pixels here

	// wait for the previous frame to finish;
	strip.wait();
	strip.show()

	// Alternate frame buffers on each draw command
	strip.moveToNextBuffer();
}
```

The 24-bit RGB data to be displayed is laid out with BRGA format,
since that is how it will be translated during the clock out from the PRU.

```cpp
struct PixelBone_pixel_t{
	uint8_t b;
	uint8_t r;
	uint8_t g;
	uint8_t a;
} __attribute__((__packed__));
```

#Low level API

If you want to poke at the PRU directly, there is a command structure
shared in PRU DRAM that holds a pointer to the current frame buffer,
the length in pixels, a command byte and a response byte.
Once the PRU has cleared the command byte you are free to re-write the
dma address or number of pixels.

```cpp
struct ws281x_command_t {
	// in the DDR shared with the PRU
	const uintptr_t pixels_dma;

	// Length in pixels of the longest LED strip.
	unsigned num_pixels;

	// write 1 to start, 0xFF to abort. will be cleared when started
	volatile unsigned command;

	// will have a non-zero response written when done
	volatile unsigned response;
} __attribute__((__packed__));
```

Reference
==========
* http://www.adafruit.com/products/1138
* http://www.adafruit.com/datasheets/WS2811.pdf
