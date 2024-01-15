/**
 * Draws rectangle with some 8x12 text on the HY28B-TFT display
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
  display.drawRectangle(5, 5, 314, 234, Color::lightskyblue);
  display.drawRectangle(10, 10, 309, 229, Color::hotpink);
  display.drawRectangle(15, 15, 304, 224, Color::chartreuse);
  

  display.drawTextSmall(20, 20, "Hello 8x12 Red", Color::red, Color::black);
  display.drawTextSmall(20, 40, "Hello 8x12 Green", Color::green, Color::black);
  display.drawTextSmall(20, 60, "Hello 8x12 Blue", Color::blue, Color::black);
  display.drawTextSmall(20, 80, "Hello 8x12 Olive", Color::olive, Color::black);
  display.drawTextSmall(20, 100, "Hello 8x12 Olive (RGB)", RGB565CONVERT(128, 128, 0), Color::black);
  delay(2000);
}