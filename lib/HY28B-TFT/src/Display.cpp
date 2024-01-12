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

    digitalWrite(_cs, HIGH);
    digitalWrite(_rst, HIGH);

    // Reset controller
    digitalWrite(_rst, LOW);
    delay(2);
    digitalWrite(_rst, HIGH);
    delay(2);

    digitalWrite(_cs, LOW);                  // Enable LCD
    writeReg(0xE5, 0x78F0); /* set SRAM internal timing */
    writeReg(0x02, 0x0700); /* set 1 line inversion OLD 0700*/
    
    // if (_orientation == VERTICAL)
    // {
    //     writeReg(0x01, 0x0000); /* set Driver Output Control OLD 0100*/
    //     writeReg(0x03, 0x1038); // Entry mode OLD 1030
    //     writeReg(0x60, 0x2700); /* Gate Scan Line */
    //     MAX_X = 240;
    //     MAX_Y = 320;
    // }
    // else
    // {
    //     writeReg(0x01, 0x0000); /* set Driver Output Control OLD 0100*/
    //     writeReg(0x03, 0x1030); // Entry mode
    //     writeReg(0x60, 0x2700); /* Gate Scan Line */
    //     MAX_X = 320;
    //     MAX_Y = 240;
    // }
    
    writeReg(0x04, 0x0000); /* Resize register */
    writeReg(0x08, 0x0207); /* set the back porch and front porch */
    writeReg(0x09, 0x0000); /* set non-display area refresh cycle ISC[3:0] */
    writeReg(0x0A, 0x0000); /* FMARK function */
    writeReg(0x0C, 0x0000); /* RGB interface setting */
    writeReg(0x0D, 0x0000); /* Frame marker Position */
    writeReg(0x0F, 0x0000); /* RGB interface polarity */

    /*************Power On sequence ****************/
    writeReg(0x10, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    writeReg(0x11, 0x0007); /* DC1[2:0], DC0[2:0], VC[2:0] */
    writeReg(0x12, 0x0000); /* VREG1OUT voltage */
    writeReg(0x13, 0x0000); /* VDV[4:0] for VCOM amplitude */
    writeReg(0x07, 0x0001);
    delay(200);

    /* Dis-charge capacitor power voltage */
    writeReg(0x10, 0x1090); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    writeReg(0x11, 0x0227); /* Set DC1[2:0], DC0[2:0], VC[2:0] */
    delay(50);                  /* Delay 50ms */
    writeReg(0x12, 0x001F);
    delay(50);                  /* Delay 50ms */
    writeReg(0x13, 0x1500); /* VDV[4:0] for VCOM amplitude */
    writeReg(0x29, 0x0027); /* 04 VCM[5:0] for VCOMH */
    writeReg(0x2B, 0x000D); /* Set Frame Rate */
    delay(50);                  /* Delay 50ms */
    writeReg(0x20, 0x0000); /* GRAM horizontal Address */
    writeReg(0x21, 0x0000); /* GRAM Vertical Address */

    /* ----------- Adjust the Gamma Curve ---------- */
    writeReg(0x30, 0x0000);
    writeReg(0x31, 0x0707);
    writeReg(0x32, 0x0307);
    writeReg(0x35, 0x0200);
    writeReg(0x36, 0x0008);
    writeReg(0x37, 0x0004);
    writeReg(0x38, 0x0000);
    writeReg(0x39, 0x0707);
    writeReg(0x3C, 0x0002);
    writeReg(0x3D, 0x1D04);

    /* ------------------ Set GRAM area --------------- */
    writeReg(0x50, 0);   /* Set X Start Horizontal GRAM Start Address */
    writeReg(0x51, 239); /* Set X End Horizontal GRAM End Address */
    writeReg(0x52, 0);   /* Set Y Start Vertical GRAM Start Address */
    writeReg(0x53, 319); /* Set Y End Vertical GRAM Start Address */

    writeReg(0x61, 0x0001); /* NDL, VLE, REV */
    writeReg(0x6A, 0x0000); /* set scrolling line */

    /* -------------- Partial Display Control --------- */
    writeReg(0x80, 0x0000);
    writeReg(0x81, 0x0000);
    writeReg(0x82, 0x0000);
    writeReg(0x83, 0x0000);
    writeReg(0x84, 0x0000);
    writeReg(0x85, 0x0000);
    
    /* -------------- Panel Control ------------------- */
    writeReg(0x90, 0x0010);
    writeReg(0x92, 0x0600);
    writeReg(0x07, 0x0133);
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
    if (_orientation == HORIZONTAL)
    {
        setCursor(319, 0);
    }
    else
    {
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