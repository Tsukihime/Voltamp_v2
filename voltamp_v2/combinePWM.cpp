#include <avr/io.h>
#include <avr/interrupt.h>

#include "combinePWM.h"
#include "utils.h"
#include "uart.h"

const uint8_t TIMER0_COMPARE_OUTPUT_FAST_PWM_INV_CH_A = ((1 <<  COM0A1) | (1 <<  COM0A0));
const uint8_t TIMER0_COMPARE_OUTPUT_FAST_PWM_INV_CH_B = ((1 <<  COM0B1) | (1 <<  COM0B0));
const uint8_t TIMER0_COMPARE_OUTPUT_FAST_PWM_CH_A = ((1 <<  COM0A1) | (0 <<  COM0A0));
const uint8_t TIMER0_COMPARE_OUTPUT_FAST_PWM_CH_B = ((1 <<  COM0B1) | (0 <<  COM0B0));
const uint8_t TIMER0_FAST_PWM_8BIT_A = ((1 << WGM01) | (1 << WGM00));
const uint8_t TIMER0_FAST_PWM_8BIT_B = (0 << WGM02);
const uint8_t TIMER0_PWM_PHASE_CORRECT_8BIT_A = ((0 << WGM01) | (1 << WGM00));
const uint8_t TIMER0_PWM_PHASE_CORRECT_8BIT_B = (0 << WGM02);
const uint8_t TIMER0_PRESCALER_DIV1 = ((0 << CS02) | (0 << CS01) | (1 << CS00));
const uint8_t TIMER0_NO_INTERRUPT = ((0 << OCIE0A) | (0 << OCIE0B) | (0 << TOIE0));

const uint8_t TIMER1_COMPARE_OUTPUT_FAST_PWM_INV_CH_A = ((1 <<  COM1A1) | (1 <<  COM1A0));
const uint8_t TIMER1_COMPARE_OUTPUT_FAST_PWM_INV_CH_B = ((1 <<  COM1B1) | (1 <<  COM1B0));
const uint8_t TIMER1_FAST_PWM_8BIT_A = ((0 << WGM11) | (1 << WGM10));
const uint8_t TIMER1_FAST_PWM_8BIT_B = ((0 << WGM13) | (1 << WGM12));
const uint8_t TIMER1_COMPARE_OUTPUT_FAST_PWM_CH_A = ((1 <<  COM1A1) | (0 <<  COM1A0));
const uint8_t TIMER1_COMPARE_OUTPUT_FAST_PWM_CH_B = ((1 <<  COM1B1) | (0 <<  COM1B0));
const uint8_t TIMER1_PWM_PHASE_CORRECT_8BIT_A = ((0 << WGM11) | (1 << WGM10));
const uint8_t TIMER1_PWM_PHASE_CORRECT_8BIT_B = ((0 << WGM13) | (0 << WGM12));
const uint8_t TIMER1_PRESCALER_DIV1 = ((0 << CS12) | (0 << CS11) | (1 << CS10));
const uint8_t TIMER1_NO_INTERRUPT = ((0 << ICIE1) | (0 << OCIE1B) | (0 << OCIE1A) | (0 << TOIE1));
const uint8_t TIMER1_COUNTER_TOP = 0x00FF;

const uint8_t TIMER2_FAST_PWM_8BIT_A = ((1 << WGM21) | (1 << WGM20));
const uint8_t TIMER2_FAST_PWM_8BIT_B = (0 << WGM22);
const uint8_t TIMER2_PRESCALER_DIV1 = ((0 << CS22) | (0 << CS21) | (1 << CS20));
const uint8_t TIMER2_PRESCALER_DIV8 = ((0 << CS22) | (1 << CS21) | (0 << CS20));
const uint8_t TIMER2_OUTPUT_COMPARE_MATCH_A_INTERRUPT_ENABLE = (1 << OCIE2A);
const uint8_t TIMER2_OUTPUT_COMPARE_MATCH_B_INTERRUPT_ENABLE = (1 << OCIE2B);
const uint8_t TIMER2_ALL_INTERRUPT_DISABLE = 0;
const uint8_t TIMER2_ALL_BUSY_BITS = ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB));
const uint8_t TIMER2_EXTERNAL_CLOCK = (1 << AS2);
const uint8_t TIMER2_NORMAL_PORT_OPERATION_OC0A_DISCONNECTED = ((0 << COM2A1) | (0 << COM2A0));
const uint8_t TIMER2_NORMAL_PORT_OPERATION_OC0B_DISCONNECTED = ((0 << COM2B1) | (0 << COM2B0));
const uint8_t TIMER2_TOGGLE_OC2B_ON_COMPARE_MATCH = ((0 << COM2B1) | (1 << COM2B0));
const uint8_t TIMER2_NORMAL_MODE_A = ((0 << WGM21) | (0 << WGM20));
const uint8_t TIMER2_NORMAL_MODE_B = (0 << WGM22);
const uint8_t TIMER2_COMPARE_OUTPUT_FAST_PWM_CH_B = ((1 <<  COM2B1) | (0 <<  COM2B0));
const uint8_t TIMER2_OVERFLOW_INTERRUPT = (1 <<  TOIE2);

CombinePWM pwm;

void CombinePWM::initialize() {
    DDRB |= TIMER1_OUTPUTS;
    DDRD |= TIMER0_OUTPUTS;
    PORTB &= ~TIMER1_OUTPUTS;
    PORTD &= ~TIMER0_OUTPUTS;

    OCR0A = 0xFF;
    OCR0B = 0xFF;
    TCNT0 = 0;
    TCCR0A = TIMER0_COMPARE_OUTPUT_FAST_PWM_INV_CH_A | TIMER0_COMPARE_OUTPUT_FAST_PWM_INV_CH_B | TIMER0_FAST_PWM_8BIT_A;
    TCCR0B = TIMER0_FAST_PWM_8BIT_B | TIMER0_PRESCALER_DIV1;
    TIMSK0 = TIMER0_NO_INTERRUPT;

    OCR1A = 0x00FF;
    OCR1B = 0x00FF;
    TCNT1 = 0;
    ICR1 = TIMER1_COUNTER_TOP;
    TCCR1A = TIMER1_COMPARE_OUTPUT_FAST_PWM_INV_CH_A | TIMER1_COMPARE_OUTPUT_FAST_PWM_INV_CH_B | TIMER1_FAST_PWM_8BIT_A;
    TCCR1B = TIMER1_FAST_PWM_8BIT_B | TIMER1_PRESCALER_DIV1;
    TIMSK1 = TIMER1_NO_INTERRUPT;

    OCR2A = 0x00;
    OCR2B = 0x00;
    TCNT2 = 0;
    TCCR2A = TIMER2_NORMAL_PORT_OPERATION_OC0A_DISCONNECTED | TIMER2_COMPARE_OUTPUT_FAST_PWM_CH_B | TIMER2_FAST_PWM_8BIT_A;
    TCCR2B = TIMER2_FAST_PWM_8BIT_B | TIMER2_PRESCALER_DIV1;
    TIMSK2 = TIMER2_OVERFLOW_INTERRUPT; // for scheduler
}

void CombinePWM::setPWMch0(uint16_t value) {
    value = ~value;
    OCR0A = value & 0xFF;
    OCR0B = (value >> 8) & 0xFF;
}

void CombinePWM::setPWMch1(uint16_t value) {
    value = ~value;
    OCR1AL = value & 0xFF;
    OCR1BL = (value >> 8) & 0xFF;
}

void CombinePWM::setPWMch2(uint8_t value) {
    OCR2B = value;
}
