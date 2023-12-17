#ifndef CALIBRATOR_H_
#define CALIBRATOR_H_

#include "screen/MainScreen.h"

class Settings {
    public:
    static void initialize();
    static void save();
    static uint16_t normalizeMultimeterVoltage(uint16_t voltage);
    static uint16_t normalizeMultimeterCurrent(uint16_t current);
    static uint16_t normalizePowerSupplierVoltage(uint16_t voltage);
    static uint16_t normalizePowerSupplierCurrent(uint16_t current);

    static uint16_t getCurrent();
    static void setCurrent(uint16_t current);
    static uint16_t getVoltage();
    static void setVoltage(uint16_t voltage);
    static AdjustmentLevel getCurrentAdjustment();
    static void setCurrentAdjustment(AdjustmentLevel currentAdjustment);
    static AdjustmentLevel getVoltageAdjustment();
    static void setVoltageAdjustment(AdjustmentLevel voltageAdjustment);
};

#endif /* CALIBRATOR_H_ */
