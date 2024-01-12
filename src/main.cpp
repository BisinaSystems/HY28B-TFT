#include <Arduino.h>
#include "Display.h"

Display display;

void setup() {
  Serial.begin(115200);
  display.print("Hello World!");
}

void loop() {
  display.print(String(millis()));
  delay(5000);
}