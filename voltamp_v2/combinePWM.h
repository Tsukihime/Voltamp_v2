#ifndef INCFILE1_H_
#define INCFILE1_H_

const uint32_t TIMER1_OUTPUTS = ((1 << 2) | (1 << 1));
const uint32_t TIMER0_OUTPUTS = ((1 << 6) | (1 << 5));
const uint32_t TIMER2_OUTPUTS = (1 << 3);

class CombinePWM {    
    public:
    void initialize();
    void setPWMch0(uint16_t value);
    void setPWMch1(uint16_t value);
    void setPWMch2(uint8_t value);
};

extern CombinePWM pwm;

#endif /* INCFILE1_H_ */