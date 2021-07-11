#ifndef _ADC_H_
#define _ADC_H_

#include "lib/Scheduler.h"

class Multimeter {
    public:
        static void initialize();
        static void updateValues(TaskPointer onComplete);
        static uint16_t getVoltage();
        static uint16_t getRawVoltage();
        static uint16_t getCurrent();
        static uint16_t getRawCurrent();
};

#endif /* _ADC_H_ */
