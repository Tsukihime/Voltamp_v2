#ifndef _ADC_H_
#define _ADC_H_

class Multimeter {
    public:
        static void initialize();
        static uint16_t getVoltage();
        static uint16_t getCurrent();
};

#endif /* _ADC_H_ */
