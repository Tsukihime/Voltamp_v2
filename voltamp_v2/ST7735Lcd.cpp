#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/cpufunc.h>

#include "lib/AVRPin.hpp"
#include "ST7735Lcd.h"

using namespace ST7735;

using ResetPin = Pd7;
using ChipSelectPin = Pb2;
using DataCommandPin = Pb0;

const uint8_t SPI_NO_INTERRUPT = (0 << SPIE);
const uint8_t SPI_ENABLE = (1 << SPE);
const uint8_t SPI_LSB_FIRST = (1 << DORD);
const uint8_t SPI_MSB_FIRST = (0 << DORD);
const uint8_t SPI_MASTER = (1 << MSTR);
const uint8_t SPI_PRESCALER_DIV4 = ((0 << SPR1) | (0 << SPR0));
const uint8_t SPI_PRESCALER_DIV16 = ((0 << SPR1) | (1 << SPR0));
const uint8_t SPI_DOUBLE_SPEED = (1 << SPI2X);

using MISO = Pb4;
using MOSI = Pb3;
using SCK = Pb5;

void ST7735Lcd::initialize(void) {
    MISO::SetDirRead();
    MOSI::SetDirWrite();
    SCK::SetDirWrite();

    SPSR = SPI_DOUBLE_SPEED;
    SPCR = SPI_NO_INTERRUPT | SPI_ENABLE | SPI_MSB_FIRST | SPI_MASTER | (0 << CPOL) | (0 << CPHA) | SPI_PRESCALER_DIV4;

    ChipSelectPin::SetDirWrite();
    ResetPin::SetDirWrite();
    DataCommandPin::SetDirWrite();

    Init();
}

void ST7735Lcd::lcd_delay_ms(uint16_t ms) {
    while (ms--) {
        _delay_ms(1);
    }
}

void ST7735Lcd::Select() {
    ChipSelectPin::Clear();
}

void ST7735Lcd::Unselect() {
    ChipSelectPin::Set();
}

void ST7735Lcd::Reset() {
    ResetPin::Clear();
    lcd_delay_ms(5);
    ResetPin::Set();
}

void ST7735Lcd::WriteCommand(uint8_t cmd) {
    DataCommandPin::Clear();
    SendData(&cmd, 1);
}

void ST7735Lcd::WriteData(uint8_t* buff, size_t buff_size) {
    DataCommandPin::Set();
    SendData(buff, buff_size);
}

void ST7735Lcd::SendData(uint8_t* data, size_t size) {
    if (size == 0) {
        return;
    }

    SPDR = *data;
    while (--size > 0) {
        data++;
        while(!(SPSR & (1 << SPIF)));
        SPDR = *data;
    }
    while(!(SPSR & (1 << SPIF)));
}

void ST7735Lcd::SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // column address set
    WriteCommand(CASET);
    uint8_t data[] = { 0x00, (uint8_t)(x0 + XSTART), 0x00, (uint8_t)(x1 + XSTART) };
    WriteData(data, sizeof(data));

    // row address set
    WriteCommand(RASET);
    data[1] = y0 + YSTART;
    data[3] = y1 + YSTART;
    WriteData(data, sizeof(data));

    // write to RAM
    WriteCommand(RAMWR);
}

void ST7735Lcd::ExecuteCommandList(const uint8_t *addr) {
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        WriteCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            lcd_delay_ms(ms);
        }
    }
}

void ST7735Lcd::Init() {
    Select();
    Reset();
    ExecuteCommandList(init_cmds1);
    ExecuteCommandList(init_cmds2);
    ExecuteCommandList(init_cmds3);
    Unselect();
}

void ST7735Lcd::FillScreen(uint16_t color) {
    FillRectangle(0, 0, WIDTH, HEIGHT, color);
}

void ST7735Lcd::FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= WIDTH) || (y >= HEIGHT)) return;
    if((x + w - 1) >= WIDTH)  w = WIDTH - x;
    if((y + h - 1) >= HEIGHT) h = HEIGHT - y;

    Select();
    SetAddressWindow(x, y, x+w-1, y+h-1);

    size_t size = w * h;
    uint8_t data[] = { (uint8_t) (color >> 8), (uint8_t) (color & 0xFF) };
    while (--size > 0) {
        WriteData(data, sizeof(data));
    }

    Unselect();
}

void ST7735Lcd::DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= WIDTH) || (y >= HEIGHT))
        return;

    Select();

    SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { (uint8_t) (color >> 8), (uint8_t) (color & 0xFF) };
    WriteData(data, sizeof(data));

    Unselect();
}

void ST7735Lcd::DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= WIDTH) || (y >= HEIGHT)) return;
    if((x + w - 1) >= WIDTH) return;
    if((y + h - 1) >= HEIGHT) return;

    Select();
    SetAddressWindow(x, y, x+w-1, y+h-1);
    WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    Unselect();
}

void ST7735Lcd::InvertColors(bool invert) {
    Select();
    WriteCommand(invert ? INVON : INVOFF);
    Unselect();
}