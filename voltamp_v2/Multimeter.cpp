#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "lib/adc.hpp"
#include "Multimeter.h"
#include "utils.h"
#include "Pwm.h"
#include "Calibrator.h"

uint32_t adc_accumulator;
uint16_t adc_counter;
uint16_t adc_result;

TaskPointer onCompleteCallback = nullptr;

uint8_t step;

uint16_t voltage;
uint16_t current;

const uint8_t VOLTAGE_CHANNEL = 6;
const uint8_t CURRENT_CHANNEL = 7;
const uint8_t ADC_INPUTS = (1 << VOLTAGE_CHANNEL | 1 << CURRENT_CHANNEL);
const uint16_t STEPS = 128;

void Multimeter::initialize() {
    Pwm::initialize();
    bitmask::clear(DDRC, ADC_INPUTS);
    bitmask::clear(PORTC, ADC_INPUTS);

    Adc::Init(0, Adc::Div128, Adc::External);
    Adc::EnableInterrupt();
    Adc::Enable();
}

void startMeasure(uint8_t channel) {
    adc_accumulator = 0;
    adc_counter = STEPS;
    Adc::SetChannel(channel);
    Adc::StartContinuousConversions();
}

void measureAll() {
    switch(step) {
        case 0:
            startMeasure(VOLTAGE_CHANNEL);
            break;
        case 1:
            voltage = adc_result;
            startMeasure(CURRENT_CHANNEL);
            break;
        case 2:
            current = adc_result;
            if(onCompleteCallback != nullptr) {
                Scheduler::setTask(onCompleteCallback);
            }
            break;
    }
    step++;
}

void onCompleteMeasure() {
    Scheduler::setTimer(measureAll, 1); // small delay for RC filter do relax
}

void Multimeter::updateValues(TaskPointer onComplete) {
    onCompleteCallback = onComplete;
    step = 0;
    measureAll();
}

uint16_t Multimeter::getRawVoltage() {
    return voltage;
}

uint16_t Multimeter::getVoltage() {
   return Calibrator::normalizeMultimeterVoltage(voltage);
}

uint16_t Multimeter::getRawCurrent() {
    return current;
}

uint16_t Multimeter::getCurrent() {
    return Calibrator::normalizeMultimeterCurrent(current);
}

void setDitherValue(uint16_t step) {
    uint8_t value = step;

    if(step >= STEPS / 2) {      // 128..64 > 0..63
        value = ~step;
    }

    Pwm::SetDitherValue6Bit(value);
}

ISR(ADC_vect) {
    adc_counter--;
    adc_accumulator += ADC;
    setDitherValue(adc_counter);

    if(adc_counter != 0) {
        return;
    }

    Adc::Disable();
    adc_result = adc_accumulator >> 1;
    onCompleteMeasure();
}
