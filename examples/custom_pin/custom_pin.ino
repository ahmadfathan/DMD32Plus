/*--------------------------------------------------------------------------------------
Author: ahmadfathan1021@gmail.com
2 Jul 2025

This example demonstrates how to use custom pin for P10 Led Matrix. By customizing the 
pin configuration, you are free to use I2C interface (which is preseve by default pin 
configuration). 
--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/
#include <DMD32Plus.h>
#include "fonts/Arial_black_16.h"
#include <DMDContainer.h>

// Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1

#define OE_PIN 15
#define A_PIN 16
#define B_PIN 17
#define CLK_PIN 18 // default VSPI CLK
#define LAT_PIN 4
#define R_DATA_PIN 23 // default VSPI MOSI

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN, OE_PIN, A_PIN, B_PIN, CLK_PIN, LAT_PIN, R_DATA_PIN);

// Timer setup
// create a hardware timer  of ESP32
hw_timer_t *timer = NULL;

char text[] = "Hello World!";

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

  dmd.selectFont(Arial_Black_16);
  dmd.drawString(0, 0, text, strlen(text), GRAPHICS_NORMAL);
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void) {
  // Do nothing
}
