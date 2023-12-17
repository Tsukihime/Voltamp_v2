#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <stdlib.h>

#include "utils.h"
#include "Bluetooth.h"
#include "Buttons.h"
#include "VoltageSelector.h"
#include "PowerSupplier.h"
#include "Multimeter.h"
#include "lib/Scheduler.h"
#include "Timer2.h"
#include "ST7735Lcd.h"
#include "screen/MainScreen.h"
#include "Settings.h"

void printValue(uint16_t value) {
    uint8_t digits[5];
    bin2bcd5(value, digits);
    bcd2ascii(digits);

    for(int i = 0; i < 4; i++) {
        if (digits[i] != '0') {
            break;
        }
        digits[i] = ' ';
    }

    Bluetooth::sendData(digits, 5);
}

void updateLeds() {
    bool cc = Buttons::isConstantCurrentMode();
    VoltageSelector::setCCLedState(cc);
    VoltageSelector::setCVLedState(!cc);
}

void processMeasure();
void startMeasure();

void firstDraw() {
    MainScreen::drawBackground();
    Scheduler::setTask(startMeasure);
}

static uint8_t frameCount = 0;
void startMeasure() {
    frameCount++;
    updateLeds();
    Multimeter::updateValues(processMeasure);
}

uint32_t energyMicroWattHour = 0;

void updateEnergyCounter() {
    uint16_t voltage = Multimeter::getVoltage();
    uint16_t current = Multimeter::getCurrent();
    energyMicroWattHour += static_cast<uint32_t>(voltage) * current / 3600;
}

static AdjustmentLevel voltageAdjustment = AdjustmentLevel::COARSE;
static AdjustmentLevel currentAdjustment = AdjustmentLevel::COARSE;

void processMeasure() {
    uint16_t voltage = Multimeter::getVoltage();
    uint16_t current = Multimeter::getCurrent();
    uint32_t power = static_cast<uint32_t>(voltage) * current / 1000;

    MainScreen::setAdjustmentLevel(voltageAdjustment, currentAdjustment);
    MainScreen::setVoltage(voltage);
    MainScreen::setCurrent(current);
    MainScreen::setPower(power);
    MainScreen::setEnergy(energyMicroWattHour / 1000); // milliWattHour
    MainScreen::setVoltageLimit(PowerSupplier::getVoltage());
    MainScreen::setCurrentLimit(PowerSupplier::getCurrent());

    Bluetooth::sendString("aA:");
    printValue(Multimeter::getRawCurrent());
    Bluetooth::sendString(" aV:");
    printValue(Multimeter::getRawVoltage());

    Bluetooth::sendString(" dA:");
    printValue(PowerSupplier::getRawCurrent());
    Bluetooth::sendString(" dV:");
    printValue(PowerSupplier::getRawVoltage());

    Bluetooth::sendString("\r\n");
    Scheduler::setTimer(startMeasure, 1);
}

void printMCULoad() {
    uint8_t load = Timer2::getMCULoad();
    printValue(load);
    Bluetooth::sendString(" % Load ");
    printValue(frameCount);
    Bluetooth::sendString(" FPS\r\n");
    frameCount = 0;
}

void updateEncoderButtons() {
    static bool voltageDown = false;
    static bool currentDown = false;

    if(voltageDown & !Buttons::isVoltageEncoderPressed()) { // voltage click
        voltageAdjustment = (AdjustmentLevel)(voltageAdjustment + 1);
        if(voltageAdjustment >= AdjustmentLevel::END) {
            voltageAdjustment = AdjustmentLevel::COARSE;
        }
        Settings::setVoltageAdjustment(voltageAdjustment);
    }

    if(currentDown & !Buttons::isCurrentEncoderPressed()) { // current click
        currentAdjustment = (AdjustmentLevel)(currentAdjustment + 1);
        if(currentAdjustment >= AdjustmentLevel::END) {
            currentAdjustment = AdjustmentLevel::COARSE;
        }
        Settings::setCurrentAdjustment(currentAdjustment);
    }

    voltageDown = Buttons::isVoltageEncoderPressed();
    currentDown = Buttons::isCurrentEncoderPressed();
}

void updateEncodersRotation(int8_t voltage_clicks, int8_t current_clicks) {
    if(voltage_clicks == 0 && current_clicks == 0) {
        return;
    }

    const uint8_t DV[3] = {200, 10, 0};
    const uint8_t DA[3] = {100, 1, 0};

    int32_t voltage_delta = voltage_clicks * DV[voltageAdjustment]; // mv per click
    int32_t current_delta = current_clicks * DA[currentAdjustment]; // ma per click
    
    int32_t new_voltage = PowerSupplier::getVoltage() + voltage_delta;
    int32_t new_current = PowerSupplier::getCurrent() + current_delta;

    new_voltage = clamp(new_voltage, 0L, 30000L);
    new_current = clamp(new_current, 0L, 3000L);

    PowerSupplier::setCurrent(new_current);
    PowerSupplier::setVoltage(new_voltage);
    Settings::setCurrent(new_current);
    Settings::setVoltage(new_voltage);
}

void updateEncoders() {
    updateEncoderButtons();

    int8_t voltage_clicks = Buttons::voltageEncoderPopDelta();
    int8_t current_clicks = Buttons::currentEncoderPopDelta();
    updateEncodersRotation(voltage_clicks, current_clicks);
}

void updateSettings() {
    Settings::save();
}

int main(void) {
    // clock_prescale_set(clock_div_1); // CLKDIV8 Fuse disabled
    Buttons::initialize();
    VoltageSelector::initialize();
    PowerSupplier::initialize();
    Bluetooth::initialize();
    Multimeter::initialize();
    Timer2::initialize();
    Settings::initialize();
    ST7735Lcd::initialize();
    sei();

    VoltageSelector::setBluetoothATState(false);

    Scheduler::setTimer(printMCULoad, 100, true);
    Scheduler::setTimer(updateEnergyCounter, 100, true);
    Scheduler::setTimer(updateEncoders, 10, true);
    Scheduler::setTask(firstDraw);
    Scheduler::setTimer(updateSettings, 1000, true);

    PowerSupplier::setCurrent(Settings::getCurrent());
    PowerSupplier::setVoltage(Settings::getVoltage());
    currentAdjustment = Settings::getCurrentAdjustment();
    voltageAdjustment = Settings::getVoltageAdjustment();

    Timer2::poolTasksRun();
}
