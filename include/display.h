#pragma once

#include <Arduino.h>

/**
 * @brief Display driver for the HY28B 
 * 
 */
class Display {
public:
    void print(String value);
};