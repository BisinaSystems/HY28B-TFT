/**
 * Draws random colored rectangles on an HY28B-TFT display
 */

#include <Arduino.h>
#include <SPI.h>

#include "Display.h"
#include "Colors.h"

#define MISO    19
#define MOSI    23
#define SCLK    18
#define CS      5
#define RESET   13

SPIClass * spi;
Display display;

void setup() {
  Serial.begin(115200);

  pinMode(CS, OUTPUT);
  pinMode(RESET, OUTPUT);

  spi = new SPIClass(VSPI);
  spi->begin(SCLK, MISO, MOSI, CS);

  display.init(spi, CS, RESET, Orientation::HORIZONTAL, 78000000U);
  display.clear(Color::royalblue);
}

void loop() {
  display.clear((unsigned short) random(0, 65535));

  auto x1 = (unsigned short) random(0, 319);
  auto x2 = (unsigned short) random(0, 319);
  auto y1 = (unsigned short) random(0, 239);
  auto y2 = (unsigned short) random(0, 239);
  auto c = (unsigned short) random(0, 65535);

  display.drawLine(x1, y1, x2, y1, c);
  display.drawLine(x2, y1, x2, y2, c);
  display.drawLine(x2, y2, x1, y2, c);
  display.drawLine(x1, y2, x1, y1, c);

  display.drawRectangle(x1, y1, x2, y2, c);
}