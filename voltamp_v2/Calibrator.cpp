#include <avr/eeprom.h>

#include "Calibrator.h"

struct CalibrationData {
    struct {
        uint16_t V0;
        uint16_t A0;
        uint16_t V30;
        uint16_t A3;
    } PowerSupplier;
    struct {
        uint16_t V0;
        uint16_t A0;
        uint16_t V30;
        uint16_t A3;
    } Multimeter;
} calibrationData;

void Calibrator::initialize() {
    eeprom_read_block(&calibrationData, nullptr, sizeof(CalibrationData));
    calibrationData.Multimeter.A0 = 861;
    calibrationData.Multimeter.A3 = 52684;
    calibrationData.Multimeter.V0 = 0;
    calibrationData.Multimeter.V30 = 59480;
    calibrationData.PowerSupplier.A0 = 67;
    calibrationData.PowerSupplier.A3 = 4095;
    calibrationData.PowerSupplier.V0 = 0;
    calibrationData.PowerSupplier.V30 = 3830;
}

void Calibrator::save() {
    eeprom_write_block(&calibrationData, nullptr, sizeof(CalibrationData));
}

uint16_t remap(uint16_t value, uint16_t oldMin, uint16_t oldMax, uint16_t newMin, uint16_t newMax) {
    if(value < oldMin) {
        return 0;
    }
    uint16_t oldRange = oldMax - oldMin;
    uint16_t newRange = newMax - newMin;
    return (static_cast<uint32_t>(value - oldMin) * newRange) / oldRange + newMin;
}

uint16_t Calibrator::normalizeMultimeterVoltage(uint16_t voltage) {
    return remap(voltage, calibrationData.Multimeter.V0, calibrationData.Multimeter.V30, 0, 30000);
}

uint16_t Calibrator::normalizeMultimeterCurrent(uint16_t current) {
    return remap(current, calibrationData.Multimeter.A0, calibrationData.Multimeter.A3, 0, 3000);
}

uint16_t Calibrator::normalizePowerSupplierVoltage(uint16_t voltage) {
    return remap(voltage, 0, 30000, calibrationData.PowerSupplier.V0, calibrationData.PowerSupplier.V30);
}

uint16_t Calibrator::normalizePowerSupplierCurrent(uint16_t current) {
    return remap(current, 0, 3000, calibrationData.PowerSupplier.A0, calibrationData.PowerSupplier.A3);
}
