#ifndef PWM_H_
#define PWM_H_

class Pwm {
    public:
        static void initialize();
        static void SetDitherValue6Bit(uint8_t value);
};

#endif /* PWM_H_ */