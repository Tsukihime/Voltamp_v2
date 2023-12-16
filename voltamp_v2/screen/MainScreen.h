#ifndef MAINSCREEN_H_
#define MAINSCREEN_H_

#include <stdint.h>

enum AdjustmentLevel {COARSE, FINE, LOCK, END};

class MainScreen {
    private:
        static void DrawBorders();
        static void DrawDots();
    public:
        static void drawBackground();
        static void setVoltage(uint16_t voltage);
        static void setCurrent(uint16_t current);
        static void setPower(uint32_t power);
        static void setEnergy(uint32_t energy);
        static void setVoltageLimit(uint16_t voltageLimit);
        static void setCurrentLimit(uint16_t currentLimit);
        static void setAdjustmentLevel(AdjustmentLevel voltage, AdjustmentLevel current);
};

#endif /* MAINSCREEN_H_ */
