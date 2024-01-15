#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <functional>

#include "Fonts/Fonts.h"

enum Orientation
{
    VERTICAL,
    HORIZONTAL
} typedef Orientation;

/**
 * @brief Display driver for the HY28B-TFT LCD
 *
 */
class Display
{
public:
    /**
     * @brief Initializes the display driver.
     */
    void init(
        SPIClass *spi,
        uint8_t cs,
        uint8_t rst,
        Orientation orientation = Orientation::HORIZONTAL,
        uint32_t spiClockSpeed = 78000000U);

    void setCursor(unsigned int x, unsigned int y);

    void setPoint(
        unsigned short x,
        unsigned short y,
        unsigned short color);

    void clear(unsigned short color = 0x0000);

    void off()
    {
        writeReg(0x07, 0x0000);
    };

    void on()
    {
        writeReg(0x07, 0x0173);
    };

    void drawLine(
        unsigned short x0,
        unsigned short y0,
        unsigned short x1,
        unsigned short y1,
        unsigned short color);

    void drawRectangle(
        unsigned short x1,
        unsigned short y1,
        unsigned short x2,
        unsigned short y2,
        unsigned short color);

    void drawText(uint16_t Xpos, uint16_t Ypos, const char *str, uint16_t Color, uint16_t bkColor)
    {
        int l = 0;
        uint16_t v;
        unsigned int Chnum = 0;
        do
        {
            v = *(str + l);
            l++;
            Chnum = (v - 32) * 32;
            putChar32(Xpos, Ypos, Chnum, Color, bkColor);
            if (Xpos < 320 - 32)
            {
                Xpos += 16;
            }
            else if (Ypos < 240 - 16)
            {
                Xpos = 10;
                Ypos += 32;
            }
            else
            {
                Xpos = 0;
                Ypos = 0;
            }
        } while (l < strlen(str));
    }

    void drawTextSmall(uint16_t Xpos, uint16_t Ypos, const char *str, uint16_t Color, uint16_t bkColor)
    {
        int l = 0;
        uint16_t v;
        unsigned int Chnum = 0;

        do
        {
            v = *(str + l);
            l++;
            Chnum = (v - 32) * 12;
            putChar(Xpos, Ypos, Chnum, Color, bkColor);
            if (Xpos < 320 - 16)
            {
                Xpos += 8;
            }
            else if (Ypos < 240 - 12)
            {
                Xpos = 10;
                Ypos += 24;
            }
            else
            {
                Xpos = 0;
                Ypos = 0;
            }
        } while (l < strlen(str));
    }

private:
    SPIClass *_spi;
    uint8_t _cs;
    uint8_t _rst;
    uint32_t _spiClockSpeed;
    Orientation _orientation;
    SPISettings _spiSettingsMsb;
    SPISettings _spiSettingsLsb;

    static const int _spiStart = 0x70;
    static const int _spiRD = 0x01;
    static const int _spiWR = 0x00;
    static const int _spiData = 0x02;
    static const int _spiIndex = 0x00;

    /**
     * @brief Executes a SPI transaction with the provided operation.
     *
     * This method begins a SPI transaction, sets the chip select pin to LOW,
     * executes the provided operation, sets the chip select pin to HIGH,
     * and ends the SPI transaction.
     *
     * @param operation The operation to be executed within the transaction.
     */
    void transactWrite(std::function<void()> operation)
    {
        _spi->beginTransaction(_spiSettingsMsb);
        digitalWrite(_cs, LOW);

        operation();

        digitalWrite(_cs, HIGH);
        _spi->endTransaction();
    }

    unsigned short transactRead(std::function<unsigned short()> operation)
    {
        auto result = operation();
        return result;
    }

    void writeIndex(unsigned char index)
    {
        transactWrite([=]()
                      {
            _spi->transfer(0x70 | 0x00 | 0x00);
            _spi->transfer(0);
            _spi->transfer(index); });
    }

    void writeData(unsigned short data)
    {
        transactWrite([=]()
                      {
            _spi->transfer(0x70 | 0x00 | 0x02);
            _spi->transfer((data >> 0x08));
            _spi->transfer((data & 0xFF)); });
    }

    void writeDataStart(void)
    {
        transactWrite([=]()
                      { _spi->transfer(_spiStart | _spiWR | _spiData); });
    }

    void writeDataOnly(unsigned short data)
    {
        transactWrite([=]()
                      {
            _spi->transfer((data >> 0x08));
            _spi->transfer((data & 0xFF)); });
    }

    unsigned short readData(void)
    {
        // return transactRead([=](){
        //     unsigned short value;

        //     _spi->transfer(_spiStart | _spiRD | _spiData);
        //     _spi->transfer(0);

        //     value = _spi->transfer(0);
        //     value <<= 8;
        //     value |= _spi->transfer(0);
        // });

        return 0; // TODO: not sure why the above is having issue accessing transactRead method, works in another project fine
    }

    void writeReg(unsigned short index, unsigned short data)
    {
        writeIndex(index);
        writeData(data);
    }

    unsigned short readReg(unsigned short LCD_Reg)
    {
        unsigned short lcdRam;
        writeIndex(LCD_Reg);
        lcdRam = readData();
        return lcdRam;
    }

    void init9320()
    {
        digitalWrite(_cs, HIGH);
        digitalWrite(_rst, HIGH);
        digitalWrite(_rst, LOW);
        delay(2);
        digitalWrite(_rst, HIGH);
        delay(2);
        digitalWrite(_cs, LOW);

        writeReg(0x00, 0x0000);
        writeReg(0x01, 0x0100);
        writeReg(0x02, 0x0700);

        writeReg(0x0003, (_orientation == 0)
                             ? 0x1030
                             : 0x1008);

        writeReg(0x04, 0x0000);   /* Scalling Contral */
        writeReg(0x08, 0x0202);   /* Display Contral 2 */
        writeReg(0x09, 0x0000);   /* Display Contral 3 */
        writeReg(0x0a, 0x0000);   /* Frame Cycle Contal */
        writeReg(0x0c, (1 << 0)); /* Extern Display Interface Contral 1 */
        writeReg(0x0d, 0x0000);   /* Frame Maker Position */
        writeReg(0x0f, 0x0000);   /* Extern Display Interface Contral 2 */
        delay(50);
        writeReg(0x07, 0x0101); /* Display Contral */
        delay(50);
        writeReg(0x10, (1 << 12) | (0 << 8) | (1 << 7) | (1 << 6) | (0 << 4)); /* Power Control 1 */
        writeReg(0x11, 0x0007);                                                /* Power Control 2 */
        writeReg(0x12, (1 << 8) | (1 << 4) | (0 << 0));                        /* Power Control 3 */
        writeReg(0x13, 0x0b00);                                                /* Power Control 4 */
        writeReg(0x29, 0x0000);                                                /* Power Control 7 */
        writeReg(0x2b, (1 << 14) | (1 << 4));

        writeReg(0x50, 0);   /* Set X Start */
        writeReg(0x51, 239); /* Set X End */
        writeReg(0x52, 0);   /* Set Y Start */
        writeReg(0x53, 319); /* Set Y End */

        delay(50);

        writeReg(0x60, 0x2700); /* Driver Output Control */
        writeReg(0x61, 0x0001); /* Driver Output Control */
        writeReg(0x6a, 0x0000); /* Vertical Scroll Control */

        writeReg(0x80, 0x0000); /* Display Position? Partial Display 1 */
        writeReg(0x81, 0x0000); /* RAM Address Start? Partial Display 1 */
        writeReg(0x82, 0x0000); /* RAM Address End-Partial Display 1 */
        writeReg(0x83, 0x0000); /* Display Position? Partial Display 2 */
        writeReg(0x84, 0x0000); /* RAM Address Start? Partial Display 2 */
        writeReg(0x85, 0x0000); /* RAM Address End? Partial Display 2 */

        writeReg(0x90, (0 << 7) | (16 << 0)); /* Frame Cycle Contral */
        writeReg(0x92, 0x0000);               /* Panel Interface Contral 2 */
        writeReg(0x93, 0x0001);               /* Panel Interface Contral 3 */
        writeReg(0x95, 0x0110);               /* Frame Cycle Contral */
        writeReg(0x97, (0 << 8));
        writeReg(0x98, 0x0000); /* Frame Cycle Contral */
        writeReg(0x07, 0x0133);
        delay(50);

        digitalWrite(_cs, HIGH);

        delay(30);
        digitalWrite(_cs, LOW);
    }

    void putChar32(uint16_t Xpos, uint16_t Ypos, int ASCI, uint16_t charColor, uint16_t bkColor)
    {
        uint16_t i, j;
        uint8_t tmp_char;
        unsigned int offset = ASCI;
        for (i = 0; i < 32; i++)
        {
            tmp_char = pgm_read_word(&chlib[offset + i]);
            for (j = 0; j < 8; j++)
            {
                if ((tmp_char >> 7 - j) & 0x01 == 0x01)
                {
                    setPoint(Xpos + j, Ypos + i, charColor); /* Character color */
                }
                else
                {
                    setPoint(Xpos + j, Ypos + i, bkColor); /* Background color */
                }
            }
            i++;
            tmp_char = pgm_read_word(&chlib[offset + i]);
            for (j = 0; j < 8; j++)
            {
                if ((tmp_char >> 7 - j) & 0x01 == 0x01)
                {
                    setPoint(Xpos + j + 8, Ypos + i - 1, charColor); /* Character color */
                }
                else
                {
                    setPoint(Xpos + j + 8, Ypos + i - 1, bkColor); /* Background color */
                }
            }
        }
    }

    void putChar(uint16_t Xpos, uint16_t Ypos, int ASCI, uint16_t charColor, uint16_t bkColor)
    {

        uint16_t i, j;
        unsigned int offset = ASCI;
        uint8_t tmp_char;
        for (i = 0; i < 12; i++)
        {
            tmp_char = pgm_read_word(&Smalls[offset + i]);
            for (j = 0; j < 8; j++)
            {
                if ((tmp_char >> 7 - j) & 0x01 == 0x01)
                {
                    setPoint(Xpos + j, Ypos + i, charColor);
                }
                else
                {
                    setPoint(Xpos + j, Ypos + i, bkColor);
                }
            }
        }
    }
};