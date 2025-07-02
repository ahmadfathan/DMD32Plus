/*--------------------------------------------------------------------------------------

 DMD32.h   - Function and support library for the Freetronics DMD, a 512 LED matrix display
           panel arranged in a 32 x 16 layout.

 Note that the DMD32 library uses the SPI port for the fastest, low overhead writing to the
 display. Keep an eye on conflicts if there are any other devices running from the same
 SPI port, and that the chip select on those devices is correctly set to be inactive
 when the DMD is being written to.


LED Panel Layout in RAM
                            32 pixels (4 bytes)
        top left  ----------------------------------------
                  |                                      |
         Screen 1 |        512 pixels (64 bytes)         | 16 pixels
                  |                                      |
                  ---------------------------------------- bottom right

 ---

 This program is free software: you can redistribute it and/or modify it under the terms
 of the version 3 GNU General Public License as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.

--------------------------------------------------------------------------------------*/
#ifndef DMD32_PLUS_H_
#define DMD32_PLUS_H_

// Arduino toolchain header, version dependent
#include "Arduino.h"

// SPI library must be included for the SPI scanning/connection method to the DMD
#include <SPI.h>

#include "DMDContainer.h"
#include "constants.h"

// ######################################################################################################################
// ######################################################################################################################
#warning CHANGE THESE TO SEMI-ADJUSTABLE PIN DEFS!
// ESP32 pins used for the display connection (Using VSPI)
#define PIN_DMD_nOE 22    // D22 active low Output Enable, setting this low lights all the LEDs in the selected rows. Can pwm it at very high frequency for brightness control.
#define PIN_DMD_A 19      // D19
#define PIN_DMD_B 21      // D21
#define PIN_DMD_CLK 18    // D18_SCK  is SPI Clock if SPI is used
#define PIN_DMD_LAT 2     // D02
#define PIN_DMD_R_DATA 23 // D23_MOSI is SPI Master Out if SPI is used
// Define this chip select pin that the Ethernet W5100 IC or other SPI device uses
// if it is in use during a DMD scan request then scanDisplayBySPI() will exit without conflict! (and skip that scan)
#define PIN_OTHER_SPI_nCS SS
// ######################################################################################################################
// ######################################################################################################################

// Pixel/graphics writing modes (bGraphicsMode)
#define GRAPHICS_NORMAL 0
#define GRAPHICS_INVERSE 1
#define GRAPHICS_TOGGLE 2
#define GRAPHICS_OR 3
#define GRAPHICS_NOR 4

// drawTestPattern Patterns
#define PATTERN_ALT_0 0
#define PATTERN_ALT_1 1
#define PATTERN_STRIPE_0 2
#define PATTERN_STRIPE_1 3

// display screen (and subscreen) sizing
#define DMD_PIXELS_ACROSS 32 // pixels across x axis (base 2 size expected)
#define DMD_PIXELS_DOWN 16   // pixels down y axis
#define DMD_BITSPERPIXEL 1   // 1 bit per pixel, use more bits to allow for pwm screen brightness control
#define DMD_RAM_SIZE_BYTES ((DMD_PIXELS_ACROSS * DMD_BITSPERPIXEL / 8) * DMD_PIXELS_DOWN)
// (32x * 1 / 8) = 4 bytes, * 16y = 64 bytes per screen here.
// lookup table for DMD::writePixel to make the pixel indexing routine faster
static byte bPixelLookupTable[8] =
    {
        0x80, // 0, bit 7
        0x40, // 1, bit 6
        0x20, // 2. bit 5
        0x10, // 3, bit 4
        0x08, // 4, bit 3
        0x04, // 5, bit 2
        0x02, // 6, bit 1
        0x01  // 7, bit 0
};

typedef uint8_t (*FontCallback)(const uint8_t *);

// The main class of DMD library functions
class DMD
{
public:
  // Instantiate the DMD
  DMD(byte panelsWide, byte panelsHigh);
  // virtual ~DMD();

  DMD(
      byte panelsWide, byte panelsHigh,
      uint8_t nOEPin, uint8_t aPin, uint8_t bPin, uint8_t clkPin, uint8_t latPin, uint8_t rDataPin);

  // Set or clear a pixel at the x and y location (0,0 is the top left corner)
  void writePixel(unsigned int bX, unsigned int bY, byte bGraphicsMode, byte bPixel);

  // Draw a string
  void drawString(int bX, int bY, const char *bChars, byte length, byte bGraphicsMode);

  // Select a text font
  void selectFont(const uint8_t *font);

  // Draw a single character
  int drawChar(const int bX, const int bY, const unsigned char letter, byte bGraphicsMode);

  // Find the width of a character
  int charWidth(const unsigned char letter);

  // Draw a scrolling string
  void drawMarquee(const char *bChars, byte length, int left, int top);

  // Move the maquee accross by amount
  boolean stepMarquee(int amountX, int amountY);

  // Clear the screen in DMD RAM
  void clearScreen(byte bNormal);

  // Draw or clear a line from x1,y1 to x2,y2
  void drawLine(int x1, int y1, int x2, int y2, byte bGraphicsMode);

  // Draw or clear a circle of radius r at x,y centre
  void drawCircle(int xCenter, int yCenter, int radius, byte bGraphicsMode);

  // Draw or clear a box(rectangle) with a single pixel border
  void drawBox(int x1, int y1, int x2, int y2, byte bGraphicsMode);

  // Draw or clear a filled box(rectangle) with a single pixel border
  void drawFilledBox(int x1, int y1, int x2, int y2, byte bGraphicsMode);

  // Draw the selected test pattern
  void drawTestPattern(byte bPattern);

  // Scan the dot matrix LED panel display, from the RAM mirror out to the display hardware.
  // Call 4 times to scan the whole display which is made up of 4 interleaved rows within the 16 total rows.
  // Insert the calls to this function into the main loop for the highest call rate, or from a timer interrupt
  void scanDisplayBySPI();

  void drawContainer(DMDContainer *container);

private:
  // GPIOs
  uint8_t _nOEPin, _aPin, _bPin, _clkPin, _latPin, _rDataPin;

  void init(byte panelsWide, byte panelsHigh);

  void drawCircleSub(int cx, int cy, int x, int y, byte bGraphicsMode);

  // Mirror of DMD pixels in RAM, ready to be clocked out by the main loop or high speed timer calls
  byte *bDMDScreenRAM;

  // Marquee values
  char marqueeText[256];
  byte marqueeLength;
  int marqueeWidth;
  int marqueeHeight;
  int marqueeOffsetX;
  int marqueeOffsetY;

  // Pointer to current font
  const uint8_t *Font;

  // Display information
  byte DisplaysWide;
  byte DisplaysHigh;
  byte DisplaysTotal;
  int row1, row2, row3;

  // scanning pointer into bDMDScreenRAM, setup init @ 48 for the first valid scan
  volatile byte bDMDByte;

  // uninitalised pointer to SPI object
  SPIClass *vspi = NULL;
  static const int spiClk = 4000000; // 4 MHz SPI clock

  void lightRow_01_05_09_13()
  {
    digitalWrite(_aPin, LOW);
    digitalWrite(_bPin, LOW);
  }

  void lightRow_02_06_10_14()
  {
    digitalWrite(_aPin, HIGH);
    digitalWrite(_bPin, LOW);
  }

  void lightRow_03_07_11_15()
  {
    digitalWrite(_aPin, LOW);
    digitalWrite(_bPin, HIGH);
  }

  void lightRow_04_08_12_16()
  {
    digitalWrite(_aPin, HIGH);
    digitalWrite(_bPin, HIGH);
  }

  void latchShiftRegToOutput()
  {
    digitalWrite(_latPin, HIGH);
    digitalWrite(_latPin, LOW);
  }

  void oeRowsOff()
  {
    digitalWrite(_nOEPin, LOW);
  }

  void oeRowsOn()
  {
    digitalWrite(_nOEPin, HIGH);
  }
};

#endif /* DMD_H_ */
