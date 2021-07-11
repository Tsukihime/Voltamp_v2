#include "MainScreen.h"

#include "../fonts/Font16x24.h"
#include "../fonts/Font11x16.h"
#include "../fonts/Font4x14.h"
#include "../fonts/Font6x8.h"
#include "../ST7735Lcd.h"
#include "../utils.h"

uint8_t m_voltage[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t m_voltageLimit[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t m_current[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t m_currentLimit[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t m_power[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t m_energy[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

bool voltageCoarseAdjustment = false;
bool currentCoarseAdjustment = false;

const uint16_t SCREEN_BACKGROUND = ST7735::Colors::BLACK;
const uint16_t BORDER_GREEN = ST7735Lcd::rgbTo16BitColor(99, 186, 99);
const uint16_t BORDER_YELLOW = ST7735Lcd::rgbTo16BitColor(230, 235, 189);
const uint16_t VOLTAGE_COLOR = ST7735Lcd::rgbTo16BitColor(147, 233, 140);
const uint16_t VOLTAGE_LIMIT_COLOR = ST7735Lcd::rgbTo16BitColor(178, 212, 241);
const uint16_t CURRENT_COLOR = ST7735Lcd::rgbTo16BitColor(212, 236, 142);
const uint16_t CURRENT_LIMIT_COLOR = ST7735Lcd::rgbTo16BitColor(178, 212, 241);
const uint16_t TEXT_COLOR = ST7735Lcd::rgbTo16BitColor(178, 212, 241);
const uint16_t WATT_COLOR = ST7735Lcd::rgbTo16BitColor(234, 157, 140);
const uint16_t ENERGY_COLOR = ST7735Lcd::rgbTo16BitColor(232, 232, 232);
const uint16_t AJUSTMENT_COLOR = VOLTAGE_LIMIT_COLOR;

void MainScreen::DrawBorder() {
    ST7735Lcd::FillRectangle(2, 0, 156, 2, BORDER_GREEN); // ----
    ST7735Lcd::FillRectangle(2, 39, 156, 2, BORDER_GREEN); // ----
    ST7735Lcd::FillRectangle(2, 77, 156, 1, BORDER_GREEN); // ----

    ST7735Lcd::FillRectangle(0, 0, 2, 78, BORDER_GREEN); // |
    ST7735Lcd::FillRectangle(158, 0, 2, 78, BORDER_GREEN); // |

    ST7735Lcd::FillRectangle(2, 78, 156, 1, BORDER_YELLOW); // ----
    ST7735Lcd::FillRectangle(2, 101, 156, 2, BORDER_YELLOW); // ----
    ST7735Lcd::FillRectangle(2, 126, 156, 2, BORDER_YELLOW); // ----

    ST7735Lcd::FillRectangle(0, 78, 2, 75, BORDER_YELLOW); // |
    ST7735Lcd::FillRectangle(158, 78, 2, 75, BORDER_YELLOW); // |
}

void MainScreen::DrawDots() {
    ST7735Lcd::DrawChar(2, 7 + 22, 47 + 10,  CURRENT_COLOR, SCREEN_BACKGROUND, Font4x14);
    ST7735Lcd::DrawChar(2, 86 + 22, 47 + 10,  CURRENT_LIMIT_COLOR, SCREEN_BACKGROUND, Font4x14);
    ST7735Lcd::DrawChar(2, 7 + 36, 9 + 10, VOLTAGE_COLOR, SCREEN_BACKGROUND, Font4x14);
    ST7735Lcd::DrawChar(2, 86 + 36, 9 + 10, VOLTAGE_LIMIT_COLOR, SCREEN_BACKGROUND, Font4x14);
    ST7735Lcd::DrawChar(0, 40 + 28, 106 + 2, WATT_COLOR, SCREEN_BACKGROUND, Font4x14);
    ST7735Lcd::DrawChar(0, 40 + 28, 81 + 2, ENERGY_COLOR, SCREEN_BACKGROUND, Font4x14);
}

void drawString(const char* string, uint8_t x, uint8_t y, uint16_t FgColor, uint16_t BgColor) {
    uint8_t offset = 0;
    while(*string) {
        ST7735Lcd::DrawChar(*string - 0x20 /* skip escape chars */, x + offset, y, FgColor, BgColor, Font6x8);
        string++;
        offset += Font6x8.getWidth();
    }
}

void MainScreen::drawBackground() {
    ST7735Lcd::FillRectangle(0, 0, 160, 128, SCREEN_BACKGROUND);
    DrawBorder();
    DrawDots();
    drawString("VOLTS", 46, 6, TEXT_COLOR, SCREEN_BACKGROUND);
    drawString("AMPERES", 34, 45, TEXT_COLOR, SCREEN_BACKGROUND);
    drawString("LIMIT", 125, 6, TEXT_COLOR, SCREEN_BACKGROUND);
    drawString("LIMIT", 125, 45, TEXT_COLOR, SCREEN_BACKGROUND);
    drawString("WATT", 125, 111, TEXT_COLOR, SCREEN_BACKGROUND);
    drawString("W*H", 131, 86, TEXT_COLOR, SCREEN_BACKGROUND);
}

void drawCurrentLabel(uint8_t chars[5], uint8_t old_chars[5], uint8_t x, uint8_t y, uint16_t FgColor, uint16_t BgColor) {
    if(chars[1] != old_chars[1]) {
        ST7735Lcd::DrawChar(chars[1], x + 4, y, FgColor, BgColor, Font16x24);
    }
    if(chars[2] != old_chars[2]) {
        ST7735Lcd::DrawChar(chars[2], x + 29, y + 8, FgColor, BgColor, Font11x16);
    }
    if(chars[3] != old_chars[3]) {
        ST7735Lcd::DrawChar(chars[3], x + 43, y + 8, FgColor, BgColor, Font11x16);
    }
    if(chars[4] != old_chars[4]) {
        ST7735Lcd::DrawChar(chars[4], x + 57, y + 8, FgColor, BgColor, Font11x16);
    }
}

void drawVoltageLabel(uint8_t chars[5], uint8_t old_chars[5], uint8_t x, uint8_t y, uint16_t FgColor, uint16_t BgColor) {
    if(chars[0] != old_chars[0]) {
        if(chars[0] == 0) {
            ST7735Lcd::FillRectangle(x, y, 16, 24, BgColor);
            } else {
            ST7735Lcd::DrawChar(chars[0], x, y, FgColor, BgColor, Font16x24);
        }
    }
    if(chars[1] != old_chars[1]) {
        ST7735Lcd::DrawChar(chars[1], x + 18, y, FgColor, BgColor, Font16x24);
    }
    if(chars[2] != old_chars[2]) {
        ST7735Lcd::DrawChar(chars[2], x + 43, y + 8, FgColor, BgColor, Font11x16);
    }
    if(chars[3] != old_chars[3]) {
        ST7735Lcd::DrawChar(chars[3], x + 57, y + 8, FgColor, BgColor, Font11x16);
    }
}

void drawPowerLabel(uint8_t chars[5], uint8_t old_chars[5], uint8_t x, uint8_t y, uint16_t FgColor, uint16_t BgColor) {
    if(chars[0] != old_chars[0]) {
        if(chars[0] == 0) {
            ST7735Lcd::FillRectangle(x, y, 11, 16, BgColor);
            } else {
            ST7735Lcd::DrawChar(chars[0], x, y, FgColor, BgColor, Font11x16);
        }
    }
    if(chars[1] != old_chars[1]) {
        ST7735Lcd::DrawChar(chars[1], x + 14, y, FgColor, BgColor, Font11x16);
    }
    if(chars[2] != old_chars[2]) {
        ST7735Lcd::DrawChar(chars[2], x + 35, y, FgColor, BgColor, Font11x16);
    }
    if(chars[3] != old_chars[3]) {
        ST7735Lcd::DrawChar(chars[3], x + 49, y, FgColor, BgColor, Font11x16);
    }
    if(chars[4] != old_chars[4]) {
        ST7735Lcd::DrawChar(chars[4], x + 63, y, FgColor, BgColor, Font11x16);
    }
}

void MainScreen::setVoltage(uint16_t voltage) {
    uint8_t chars[5];
    bin2bcd5(voltage, chars);
    drawVoltageLabel(chars, m_voltage, 7, 9, VOLTAGE_COLOR, SCREEN_BACKGROUND);
    memcpy(m_voltage, chars, 5);
}

void MainScreen::setCurrent(uint16_t current) {
    uint8_t chars[5];
    bin2bcd5(current, chars);
    drawCurrentLabel(chars, m_current, 7, 47, CURRENT_COLOR, SCREEN_BACKGROUND);
    memcpy(m_current, chars, 5);
}

void MainScreen::setPower(uint32_t power) {
    uint8_t chars[10];
    bin2bcd10(power, chars);
    drawPowerLabel(&chars[5], m_power, 40, 106, WATT_COLOR, SCREEN_BACKGROUND);
    memcpy(m_power, &chars[5], 5);
}

void MainScreen::setEnergy(uint32_t energy) {
    uint8_t chars[10];
    bin2bcd10(energy, chars);
    drawPowerLabel(&chars[5], m_energy, 40, 81, ENERGY_COLOR, SCREEN_BACKGROUND);
    memcpy(m_energy, &chars[5], 5);
}

void MainScreen::setVoltageLimit(uint16_t voltageLimit) {
    uint8_t chars[5];
    bin2bcd5(voltageLimit, chars);
    drawVoltageLabel(chars, m_voltageLimit, 86, 9, VOLTAGE_LIMIT_COLOR, SCREEN_BACKGROUND);
    memcpy(m_voltageLimit, chars, 5);
}

void MainScreen::setCurrentLimit(uint16_t currentLimit) {
    uint8_t chars[5];
    bin2bcd5(currentLimit, chars);
    drawCurrentLabel(chars, m_currentLimit, 86, 47, CURRENT_LIMIT_COLOR, SCREEN_BACKGROUND);
    memcpy(m_currentLimit, chars, 5);
}

void MainScreen::setAdjustmentLevel(bool voltageCoarse, bool currentCoarse) {
    if(voltageCoarseAdjustment != voltageCoarse) {
        if(voltageCoarse) {
            ST7735Lcd::FillRectangle(143, 35, 11, 2, SCREEN_BACKGROUND);
            ST7735Lcd::FillRectangle(129, 35, 11, 2, AJUSTMENT_COLOR);
        } else {
            ST7735Lcd::FillRectangle(143, 35, 11, 2, AJUSTMENT_COLOR);
            ST7735Lcd::FillRectangle(129, 35, 11, 2, SCREEN_BACKGROUND);
        }
    }

    if(currentCoarseAdjustment != currentCoarse) {
        if(currentCoarse) {
            ST7735Lcd::FillRectangle(143, 73, 11, 2, SCREEN_BACKGROUND);
            ST7735Lcd::FillRectangle(115, 73, 25, 2, AJUSTMENT_COLOR);
        } else {
            ST7735Lcd::FillRectangle(143, 73, 11, 2, AJUSTMENT_COLOR);
            ST7735Lcd::FillRectangle(115, 73, 25, 2, SCREEN_BACKGROUND);
        }
    }

    voltageCoarseAdjustment = voltageCoarse;
    currentCoarseAdjustment = currentCoarse;
}