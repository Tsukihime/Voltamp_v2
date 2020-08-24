#ifndef DAC_H_
#define DAC_H_

class Dac {
    private:
        static uint16_t voltage;
        static uint16_t current;
    public:
        static void initialize(void);
        static void setValues(uint16_t voltage, uint16_t current);
        static void setVoltage(uint16_t voltage);
        static void setCurrent(uint16_t current);
        static void enable();
        static void disable();
};

#endif /* DAC_H_ */