#ifndef CALIBRATOR_H_
#define CALIBRATOR_H_

class Calibrator {
    public:
    static void initialize();
    static void save();
    static uint16_t normalizeMultimeterVoltage(uint16_t voltage);
    static uint16_t normalizeMultimeterCurrent(uint16_t current);
    static uint16_t normalizePowerSupplierVoltage(uint16_t voltage);
    static uint16_t normalizePowerSupplierCurrent(uint16_t current);
};

#endif /* CALIBRATOR_H_ */