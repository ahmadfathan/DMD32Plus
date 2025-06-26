/*--------------------------------------------------------------------------------------
Author: ahmadfathan1021@gmail.com
26 Jun 2025

This example illustrates how to create a running (scrolling) text display within a 
custom-defined area using the DMDContainer class.

DMDContainer is designed to manage pixel-level rendering within a specified region of 
the display. By leveraging this class, you can effectively partition a P10 LED matrix 
into multiple independent display zones, each capable of rendering text or graphics 
separately. 
--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/
#include <DMD32Plus.h>  //
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_black_16.h"
#include <DMDContainer.h>

// Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

// A container to draw the running text
DMDContainer container(0, 0, 11, 16);

// Horizontal position (x) of the running text
int16_t posX;

// Last update time of the running text position 
long last;

char scrollingText[] = "Welcome To Indonesia";

// Timer setup
// create a hardware timer  of ESP32
hw_timer_t *timer = NULL;

/*--------------------------------------------------------------------------------------
  Interrupt handler for timer driven DMD refresh scanning, this gets
  called at the period set in timerAlarm;
--------------------------------------------------------------------------------------*/
void IRAM_ATTR triggerScan() {
  dmd.scanDisplayBySPI();
}

/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void) {
  Serial.begin(115200);

  // Create timer with 1MHz frequency
  timer = timerBegin(1000000L);  // 1MHz

  // Attach interrupt to the timer
  timerAttachInterrupt(timer, &triggerScan);

  // Set alarm to the timer every 1000 ticks (1ms), then repeat 
  timerAlarm(timer, 1000, true, 0);

  // clear/init the DMD pixels held in RAM
  dmd.clearScreen(true);  // true is normal (all pixels off), false is negative (all pixels on)

  // Set font for the container
  container.setFont(Arial_Black_16);

  // Set initial horizontal position of the running text
  posX = container.getX1();
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void) {
  // Check if running text horizontal position should be updated
  if (millis() - last > 50) {
    last = millis();

    // Clear pixel in the container
    container.clear();

    // Append text to the container
    // Get the width to check if the running text is completely out of the container area
    uint16_t w = container.appendText(posX, 0, scrollingText, strlen(scrollingText));

    // Draw the container to DMD 
    dmd.drawContainer(&container);

    // Move horizontal position left
    posX--;

    if (posX + w == 0) {
      // Reset horizontal position when running text is out of the container area
      posX = container.getX1();
    }
  }
}
