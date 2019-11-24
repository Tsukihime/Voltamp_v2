#ifndef _ADC_H_
#define _ADC_H_

class Adc {
    public:
        void initialize();
        uint16_t getVoltage();
        uint16_t getAmperage();        
};

extern Adc adc;

#endif /* _ADC_H_ */
