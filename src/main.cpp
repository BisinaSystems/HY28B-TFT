#include <Arduino.h>
#include <SPI.h>

#include "../lib/HY28B-TFT/src/Display.h"

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
  spi->begin();
  
  display.init(spi, CS, RESET, Orientation::HORIZONTAL, 78000000U);

  display.clear(0x0000);
}

void loop() {
  auto x = (unsigned short) random(0, 319);
  auto y = (unsigned short) random(0, 239);
  auto c = (unsigned short) random(0, 65535);

  display.setPoint(x, y, c);
  
  delay(100);
}