#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "lib/adc.hpp"
#include "Multimeter.h"
#include "utils.h"
#include "Pwm.h"

Multimeter multimeter;

volatile bool measure_complete;
volatile uint32_t adc_accumulator;
volatile uint16_t adc_counter;

const uint8_t VOLTAGE_CHANNEL = 6;
const uint8_t CURENT_CHANNEL = 7;
const uint8_t ADC_INPUTS = (1 << VOLTAGE_CHANNEL | 1 << CURENT_CHANNEL);
const uint16_t STEPS = 128;

void Multimeter::initialize() {
    bitmask::clear(DDRC, ADC_INPUTS);
    bitmask::clear(PORTC, ADC_INPUTS);

    Adc::Init(0, Adc::Div64, Adc::External);
    Adc::EnableInterrupt();
    Adc::Enable();
}

uint16_t getOversampledValue(uint8_t channel) {
    measure_complete = false;
    adc_accumulator = 0;
    adc_counter = STEPS;
    Adc::SetChannel(channel);
    Adc::StartContinuousConversions();
    while(!measure_complete)
    ;
    return adc_accumulator >> 1;
}

uint16_t Multimeter::getVoltage() {
    return getOversampledValue(VOLTAGE_CHANNEL);
}

uint16_t Multimeter::getCurrent() {
    return getOversampledValue(CURENT_CHANNEL);
}


void setDitherValue(uint16_t step) {
    uint8_t value = step; // 255..127 > 127..0

    if(step >= STEPS / 2) {      // 255..128
        value = STEPS - 1 - step;  // 0..127
    }

    Pwm::SetDitherValue(value);
}

ISR(ADC_vect) {
    adc_counter--;
    adc_accumulator += ADC;
    setDitherValue(adc_counter);

    if(adc_counter != 0) {
        return;
    }

    Adc::Disable();
    measure_complete = true;
}
