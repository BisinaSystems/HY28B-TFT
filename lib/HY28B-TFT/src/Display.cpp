    #include "Display.h"

    void Display::init(
        SPIClass *spi,
        uint8_t cs,
        uint8_t rst,
        Orientation orientation,
        uint32_t spiClockSpeed)
    {
        _spi = spi;
        _cs = cs;
        _rst = rst;
        _orientation = orientation;
        _spiClockSpeed = spiClockSpeed;

        _spiSettingsMsb = SPISettings(_spiClockSpeed, MSBFIRST, SPI_MODE3);
        _spiSettingsLsb = SPISettings(_spiClockSpeed, LSBFIRST, SPI_MODE3);
    }

    void Display::setCursor(unsigned int x, unsigned int y)
    {
        if (_orientation == VERTICAL)
        {
            writeReg(0x0020, x);
            writeReg(0x0021, y);
        }
        else
        {
            writeReg(0x0020, 240 - y);
            writeReg(0x0021, x);
        }
    }

    void Display::setPoint(unsigned short Xpos, unsigned short Ypos, unsigned short point)
    {
        if (Xpos >= 320 || Ypos >= 240)
        {
            return;
        }
        setCursor(Xpos, Ypos);
        writeReg(0x0022, point);
    }

    void Display::clear(unsigned short color)
    {
        if (_orientation == HORIZONTAL) {
            setCursor(319, 0);
        }
        else {
            setCursor(0, 0);
        }

        writeIndex(0x0022);
        digitalWrite(_cs, LOW);
        writeDataStart();
        
        for (long i = 0; i <= 320 * 240; i++) // replace the hard coded dimensions
        { 
            writeData(color);
        }

        digitalWrite(_cs, HIGH);
    }