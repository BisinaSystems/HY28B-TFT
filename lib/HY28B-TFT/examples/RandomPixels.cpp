/**
 * Draws random colored pixels on an HY28B-TFT display
 */

#include <Arduino.h>
#include <SPI.h>

#include "Display.h"

#define MISO   19
#define MOSI   23
#define SCLK   18
#define CS 5
#define RESET 13

SPIClass * spi;
Display display;

void setup() {
  Serial.begin(115200);

  pinMode(CS, OUTPUT);
  pinMode(RESET, OUTPUT);

  spi = new SPIClass(VSPI);
  spi->begin(SCLK, MISO, MOSI, CS);

  display.init(spi, CS, RESET, Orientation::HORIZONTAL, 78000000U);
  display.clear(0x0000);
}

void loop() {
  auto x = (unsigned short) random(0, 319);
  auto y = (unsigned short) random(0, 239);
  auto c = (unsigned short) 
    random(0, 1) == 1 
      ? 0x0000
      : random(0, 65535);

  display.setPoint(x, y, c);
}