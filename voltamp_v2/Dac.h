#ifndef DAC_H_
#define DAC_H_

class Dac {
    public:
        static void initialize(void);
        static void setValues(uint16_t voltage_ch, uint16_t current_ch);
        static void enable();
        static void disable();
};

#endif /* DAC_H_ */