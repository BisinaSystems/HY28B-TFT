#include <Arduino.h>
#include <SPI.h>

#include "Display.h"
#include "Colors.h"

#define MISO 19
#define MOSI 23
#define SCLK 18
#define CS 5
#define RESET 13

SPIClass *spi;
Display display;

#define WIDTH 80
#define HEIGHT 60

bool grid[HEIGHT][WIDTH];
bool newGrid[HEIGHT][WIDTH];

void initGame()
{
  for (int i = 0; i < HEIGHT; ++i)
  {
    for (int j = 0; j < WIDTH; ++j)
    {
      grid[i][j] = random(2);
    }
  }
}

int countAliveNeighbors(int x, int y)
{
  int count = 0;
  for (int i = -1; i <= 1; i++)
  {
    for (int j = -1; j <= 1; j++)
    {
      if (i == 0 && j == 0)
        continue; // Skip the cell itself
      int nx = x + i;
      int ny = y + j;

      // Check boundaries
      if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT)
      {
        count += grid[ny][nx];
      }
    }
  }
  return count;
}

void setup()
{
  Serial.begin(115200);

  pinMode(CS, OUTPUT);
  pinMode(RESET, OUTPUT);

  spi = new SPIClass(VSPI);
  spi->begin(SCLK, MISO, MOSI, CS);

  display.init(spi, CS, RESET, Orientation::HORIZONTAL, 78000000U);
  display.clear(Color::black);

  initGame();
}

void loop() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int aliveNeighbors = countAliveNeighbors(x, y);

            if (grid[y][x]) {
                newGrid[y][x] = (aliveNeighbors == 2 || aliveNeighbors == 3);
            } else {
                newGrid[y][x] = (aliveNeighbors == 3);
            }
        }
    }

    for (uint8_t y = 0; y < HEIGHT; ++y) {
        for (uint8_t x = 0; x < WIDTH; ++x) {

            bool noChange = (grid[y][x] == newGrid[y][x]);
            grid[y][x] = newGrid[y][x];
            newGrid[y][x] = false;

            if (noChange) continue;

            unsigned short color = grid[y][x] ? Color::bisque : Color::black;

            for (uint8_t dy = 0; dy < 4; ++dy) {
                for (uint8_t dx = 0; dx < 4; ++dx) {
                    display.setPoint(4 * x + dx, 4 * y + dy, color);
                }
            }
        }
    }
}