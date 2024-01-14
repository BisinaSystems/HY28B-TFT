/**
 * Draws rectangle with some 16x16 text (32px height on font) on the HY28B-TFT display
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
  display.clear(Color::black);
}

void loop() {
  display.drawRectangle(10, 10, 309, 229, Color::white);
  display.drawText(20, 20, "Hello 16x16 Font!", Color::orchid, Color::black);
  delay(2000);
}