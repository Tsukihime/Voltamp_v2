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
#include "Calibrator.h"

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

static bool voltageCoarse = true;
static bool currentCoarse = true;

void processMeasure() {
    uint16_t voltage = Multimeter::getVoltage();
    uint16_t current = Multimeter::getCurrent();
    uint32_t power = static_cast<uint32_t>(voltage) * current / 1000;

    MainScreen::setAdjustmentLevel(voltageCoarse, currentCoarse);
    MainScreen::setVoltage(voltage);
    MainScreen::setCurrent(current);
    MainScreen::setPower(power);
    MainScreen::setEnergy(energyMicroWattHour / 1000); // milliWattHour
    MainScreen::setVoltageLimit(PowerSupplier::getVoltage());
    MainScreen::setCurrentLimit(PowerSupplier::getCurrent());

//    printValue(voltage);
//    Bluetooth::sendString(" mV  ");
//    printValue(current);
//    Bluetooth::sendString(" mA  ");

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
        voltageCoarse = !voltageCoarse;
    }

    if(currentDown & !Buttons::isCurrentEncoderPressed()) { // current click
        currentCoarse = !currentCoarse;
    }

    voltageDown = Buttons::isVoltageEncoderPressed();
    currentDown = Buttons::isCurrentEncoderPressed();
}

void updateEncodersRotation() {
    int32_t voltage = PowerSupplier::getVoltage();
    int32_t current = PowerSupplier::getCurrent();
    int8_t voltage_clicks = Buttons::voltageEncoderPopDelta();
    int8_t current_clicks = Buttons::currentEncoderPopDelta();

    int32_t voltage_delta = voltage_clicks * (voltageCoarse ? 200 : 10); // mv per click
    int32_t current_delta = current_clicks * (currentCoarse ? 100 : 1); // ma per click
    
    int32_t new_voltage = voltage + voltage_delta;
    int32_t new_current = current + current_delta;

    new_voltage = clamp(new_voltage, 0L, 30000L);
    new_current = clamp(new_current, 0L, 3000L);

    PowerSupplier::setCurrent(new_current);
    PowerSupplier::setVoltage(new_voltage);
}

void updateEncoders() {
    updateEncoderButtons();
    updateEncodersRotation();
}

int main(void) {
    // clock_prescale_set(clock_div_1); // CLKDIV8 Fuse disabled
    Buttons::initialize();
    VoltageSelector::initialize();
    PowerSupplier::initialize();
    Bluetooth::initialize();
    Multimeter::initialize();    
    Timer2::initialize();
    Calibrator::initialize();
    ST7735Lcd::initialize();
    sei();

    VoltageSelector::setBluetoothATState(false);

    Scheduler::setTimer(printMCULoad, 100, true);
    Scheduler::setTimer(updateEnergyCounter, 100, true);
    Scheduler::setTimer(updateEncoders, 10, true);
    Scheduler::setTask(firstDraw);

    PowerSupplier::setCurrent(3000);
    PowerSupplier::setVoltage(2000);

    Timer2::poolTasksRun();
}
