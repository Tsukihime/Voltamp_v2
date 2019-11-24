#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "adc.h"
#include "utils.h"
#include "combinePWM.h"

Adc adc;

uint16_t adc0value = 0;
uint16_t adc1value = 0;

const uint8_t ADC_EXTERNAL_AREF = ((0 << REFS1) | (0 << REFS0));
const uint8_t ADC_RESULT_RIGHT_ADJUSTED = (0 << ADLAR);
const uint8_t ADC_ENABLE = (1 << ADEN);
const uint8_t ADC_INTERRUPT_ENABLE = (1 << ADIE);
const uint8_t ADC_PRESCALER_DIV64 = ((1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0));
const uint8_t ADC_PRESCALER_DIV128 = ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
const uint8_t ADC_FREE_RUNNING_MODE = ((0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0));
const uint8_t ADC_AUTO_TRIGGER_ENABLE = (1 << ADATE);

const uint8_t ADCMUX_MASK = 0x0F;
static void selectAdcChannel(uint8_t channel) {
    channel &= ADCMUX_MASK;
    ADMUX = (ADMUX & ~ADCMUX_MASK) | channel;
}

inline void adcEnable() {
    SET_PORT_BIT(ADCSRA, ADEN);
}

inline void adcDisable() {
    CLR_PORT_BIT(ADCSRA, ADEN);
}

inline void startConversion() {
    SET_PORT_BIT(ADCSRA, ADSC);
}

void Adc::initialize() {
    ADMUX = ADC_EXTERNAL_AREF | ADC_RESULT_RIGHT_ADJUSTED;
    ADCSRA = ADC_INTERRUPT_ENABLE | ADC_PRESCALER_DIV128 | ADC_AUTO_TRIGGER_ENABLE;
    ADCSRB = ADC_FREE_RUNNING_MODE;

    adcEnable();
    selectAdcChannel(0);
    startConversion();
}

uint16_t Adc::getVoltage() {
    return adc0value;
}

uint16_t Adc::getAmperage() {
    return adc1value;
}

inline void setDitherValue(uint16_t step) {
    uint8_t value;
    step--;
    if(step > 127) {
        value = 255 - step;
    } else {
        value = step;
    }

    pwm.setPWMch2(value * 2);
}

uint8_t channel = 0;

inline void newADCValue(uint32_t value) {
    value >>= 2;

    switch (channel) {
        case 0:
            adc0value = value;
            channel = 1;
            break;
        case 1:
            adc1value = value;
            channel = 0;
    }

    selectAdcChannel(channel);
}

uint16_t adc_counter = 0;
uint32_t adc_accumulator = 0;

ISR(ADC_vect) {
    adc_accumulator += ADC;
    setDitherValue(adc_counter);
    
    if(adc_counter--) {
        return;
    }

    adc_counter = 256;

    newADCValue(adc_accumulator);
    adc_accumulator = 0;
}
