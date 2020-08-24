#include <avr/io.h>
#include <avr/interrupt.h>

#include "Pwm.h"
#include "utils.h"

const uint32_t TIMER1_OUT = 1; // OC1A/PB1

const uint8_t TIMER1_COMPARE_OUTPUT_FAST_PWM_INV_CH_A = ((1 <<  COM1A1) | (1 <<  COM1A0));
const uint8_t TIMER1_COMPARE_OUTPUT_FAST_PWM_INV_CH_B = ((1 <<  COM1B1) | (1 <<  COM1B0));
const uint8_t TIMER1_FAST_PWM_8BIT_A = ((0 << WGM11) | (1 << WGM10));
const uint8_t TIMER1_FAST_PWM_8BIT_B = ((0 << WGM13) | (1 << WGM12));
const uint8_t TIMER1_FAST_PWM_ICR_TOP_A = (1 << WGM11) | (0 << WGM10);
const uint8_t TIMER1_FAST_PWM_ICR_TOP_B = (1 << WGM13) | (1 << WGM12);
const uint8_t TIMER1_COMPARE_OUTPUT_FAST_PWM_CH_A = ((1 <<  COM1A1) | (0 <<  COM1A0));
const uint8_t TIMER1_COMPARE_OUTPUT_FAST_PWM_CH_B = ((1 <<  COM1B1) | (0 <<  COM1B0));
const uint8_t TIMER1_PWM_PHASE_CORRECT_8BIT_A = ((0 << WGM11) | (1 << WGM10));
const uint8_t TIMER1_PWM_PHASE_CORRECT_8BIT_B = ((0 << WGM13) | (0 << WGM12));
const uint8_t TIMER1_PRESCALER_DIV1 = ((0 << CS12) | (0 << CS11) | (1 << CS10));
const uint8_t TIMER1_NO_INTERRUPT = ((0 << ICIE1) | (0 << OCIE1B) | (0 << OCIE1A) | (0 << TOIE1));
const uint8_t TIMER1_COUNTER_TOP = 0x003f;

void Pwm::initialize() {
    bit::clear(PORTB, TIMER1_OUT);
    bit::set(DDRB, TIMER1_OUT);

    TIMSK1 = TIMER1_NO_INTERRUPT;
    OCR1A = 0x0000;
    OCR1B = 0x0000;
    TCNT1 = 0;
    ICR1 = TIMER1_COUNTER_TOP;
    TCCR1A = TIMER1_COMPARE_OUTPUT_FAST_PWM_INV_CH_A | TIMER1_FAST_PWM_ICR_TOP_A;
    TCCR1B = TIMER1_FAST_PWM_ICR_TOP_B | TIMER1_PRESCALER_DIV1;
    SetDitherValue(0);
}

void Pwm::SetDitherValue(uint8_t value) {
    OCR1AL = ~value & TIMER1_COUNTER_TOP;
}
