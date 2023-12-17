#include <avr/eeprom.h>

#include "Settings.h"

const uint16_t MAGIC = 0xC0DE;

struct CalibrationData {
    uint16_t sign;
    struct {
        uint16_t V0;
        uint16_t A0;
        uint16_t V30;
        uint16_t A3;
    } PowerSupplier;
    struct {
        uint16_t V0;
        uint16_t V0Offset;
        uint16_t A0;
        uint16_t V30;
        uint16_t A3;
    } Multimeter;
    struct {
        uint16_t voltage;
        uint16_t current;
        AdjustmentLevel currentAdjustment;
        AdjustmentLevel voltageAdjustment;
    } State;
} calibrationData;

void Settings::initialize() {
    eeprom_read_block(&calibrationData, nullptr, sizeof(CalibrationData));
    if(calibrationData.sign == MAGIC) {
        return;
    }
    calibrationData.sign = MAGIC;
    calibrationData.Multimeter.A0 = 311;
    calibrationData.Multimeter.A3 = 60270;
    calibrationData.Multimeter.V0 = 0;
    calibrationData.Multimeter.V0Offset = 60;
    calibrationData.Multimeter.V30 = 59385;
    calibrationData.PowerSupplier.A0 = 57;
    calibrationData.PowerSupplier.A3 = 4008;
    calibrationData.PowerSupplier.V0 = 32;
    calibrationData.PowerSupplier.V30 = 3855;
    calibrationData.State.current = 3000;
    calibrationData.State.voltage = 2000;
    calibrationData.State.currentAdjustment = AdjustmentLevel::COARSE;
    calibrationData.State.voltageAdjustment = AdjustmentLevel::COARSE;
}

void Settings::save() {
    eeprom_update_block(&calibrationData, nullptr, sizeof(CalibrationData));
}

uint16_t remap(uint16_t value, uint16_t oldMin, uint16_t oldMax, uint16_t newMin, uint16_t newMax) {
    if(value < oldMin) {
        return 0;
    }
    uint16_t oldRange = oldMax - oldMin;
    uint16_t newRange = newMax - newMin;
    return (static_cast<uint32_t>(value - oldMin) * newRange) / oldRange + newMin;
}

uint16_t Settings::normalizeMultimeterVoltage(uint16_t voltage) {
    return remap(voltage, calibrationData.Multimeter.V0, calibrationData.Multimeter.V30, calibrationData.Multimeter.V0Offset, 30000);
}

uint16_t Settings::normalizeMultimeterCurrent(uint16_t current) {
    return remap(current, calibrationData.Multimeter.A0, calibrationData.Multimeter.A3, 0, 3000);
}

uint16_t Settings::normalizePowerSupplierVoltage(uint16_t voltage) {
    return remap(voltage, 0, 30000, calibrationData.PowerSupplier.V0, calibrationData.PowerSupplier.V30);
}

uint16_t Settings::normalizePowerSupplierCurrent(uint16_t current) {
    return remap(current, 0, 3000, calibrationData.PowerSupplier.A0, calibrationData.PowerSupplier.A3);
}

uint16_t Settings::getCurrent() {
    return calibrationData.State.current;
}

void Settings::setCurrent(uint16_t current) {
    calibrationData.State.current = current;
}

uint16_t Settings::getVoltage() {
    return calibrationData.State.voltage;
}

void Settings::setVoltage(uint16_t voltage) {
    calibrationData.State.voltage = voltage;
}

AdjustmentLevel Settings::getCurrentAdjustment() {
    return calibrationData.State.currentAdjustment;
}

void Settings::setCurrentAdjustment(AdjustmentLevel currentAdjustment) {
    calibrationData.State.currentAdjustment = currentAdjustment;
}

AdjustmentLevel Settings::getVoltageAdjustment() {
    return calibrationData.State.voltageAdjustment;
}

void Settings::setVoltageAdjustment(AdjustmentLevel voltageAdjustment) {
    calibrationData.State.voltageAdjustment = voltageAdjustment;
}
