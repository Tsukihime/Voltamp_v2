#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>
#include "timer2.h"

#include "Buttons.h"
#include "lib/Scheduler.h"

uint32_t systemTicks = 0;
uint32_t cycleCount = 0;
uint8_t MCULoad = 0;

ISR(TIMER2_COMPA_vect) {
    systemTicks++;
    Scheduler::TimerISR();
    Buttons::poolISR();
}

uint8_t Timer2::getMCULoad() {
    return MCULoad;
}

void calcMCULoad() {
    MCULoad = ((F_CPU - cycleCount) * 100 / F_CPU);
    cycleCount = 0;
}

#ifdef DEBUG
    const uint8_t IDLE_TASK_TIME = 41;
#else
    const uint8_t IDLE_TASK_TIME = 38;
#endif

void Timer2::poolTasksRun() {
    while(true) {
        Scheduler::processTasks();
        cycleCount += IDLE_TASK_TIME;
    }
}

uint32_t Timer2::getSystemTime() {
    return systemTicks / 1024;
}

const uint8_t TIMER2_CTC_MODE_A = ((1 << WGM21) | (0 << WGM20));
const uint8_t TIMER2_CTC_MODE_B = (0 << WGM22);
const uint8_t TIMER2_PRESCALER_DIV1 = ((0 << CS22) | (0 << CS21) | (1 << CS20));
const uint8_t TIMER2_OUTPUT_COMPARE_MATCH_A_INTERRUPT_ENABLE = (1 << OCIE2A);
const uint8_t TIMER2_ALL_INTERRUPT_DISABLE = 0;
const uint8_t TIMER2_ALL_BUSY_BITS = ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB));
const uint8_t TIMER2_EXTERNAL_OSCILLATOR_CLOCK = (1 << AS2);
const uint8_t TIMER2_NORMAL_PORT_OPERATION_OC0A_DISCONNECTED = ((0 << COM2A1) | (0 << COM2A0));
const uint8_t TIMER2_NORMAL_PORT_OPERATION_OC0B_DISCONNECTED = ((0 << COM2B1) | (0 << COM2B0));

void Timer2::initialize() {
    TIMSK2 = TIMER2_ALL_INTERRUPT_DISABLE;
    ASSR = TIMER2_EXTERNAL_OSCILLATOR_CLOCK;
    TCNT2 = 0;
    OCR2A = 0x1f; // 32768 / (32 - 1) = 1024 ticks per second >> period = 1000/1024 = 125/128 = 0.9765625 ms
    OCR2B = 0x00;
    TCCR2A = TIMER2_NORMAL_PORT_OPERATION_OC0A_DISCONNECTED | TIMER2_NORMAL_PORT_OPERATION_OC0B_DISCONNECTED | TIMER2_CTC_MODE_A;
    TCCR2B = TIMER2_CTC_MODE_B | TIMER2_PRESCALER_DIV1;

    while (ASSR & TIMER2_ALL_BUSY_BITS); // waiting for register update

    TIFR2 = (1 << OCF2A) | (1 << OCF2B) | (1 << TOV2); // reset interrupt flags
    TIMSK2 = TIMER2_OUTPUT_COMPARE_MATCH_A_INTERRUPT_ENABLE;

    Scheduler::setTimer(&calcMCULoad, 1024, true);
}
