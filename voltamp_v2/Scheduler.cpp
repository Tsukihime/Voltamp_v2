#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>
#include "Scheduler.h"
#include "utils.h"

uint32_t Scheduler::cycleCount = 0;
Scheduler::Task Scheduler::tasks[MAX_TASK_COUNT];
uint8_t Scheduler::taskCount;
uint32_t Scheduler::systemTicks;

const uint8_t TIMER2_CTC_MODE_A = ((1 << WGM21) | (0 << WGM20));
const uint8_t TIMER2_CTC_MODE_B = (0 << WGM22);
const uint8_t TIMER2_PRESCALER_DIV1 = ((0 << CS22) | (0 << CS21) | (1 << CS20));
const uint8_t TIMER2_OUTPUT_COMPARE_MATCH_A_INTERRUPT_ENABLE = (1 << OCIE2A);
const uint8_t TIMER2_ALL_INTERRUPT_DISABLE = 0;
const uint8_t TIMER2_ALL_BUSY_BITS = ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB));
const uint8_t TIMER2_EXTERNAL_OSCILLATOR_CLOCK = (1 << AS2);
const uint8_t TIMER2_NORMAL_PORT_OPERATION_OC0A_DISCONNECTED = ((0 << COM2A1) | (0 << COM2A0));
const uint8_t TIMER2_NORMAL_PORT_OPERATION_OC0B_DISCONNECTED = ((0 << COM2B1) | (0 << COM2B0));

ISR(TIMER2_COMPA_vect) {
    Scheduler::updateTaskStatus();
}

inline void Scheduler::updateTaskStatus() {
    systemTicks++;
    for (uint8_t i = 0; i < taskCount; i++) {
        tasks[i].counter++;
        if(tasks[i].counter >= tasks[i].period) {
            tasks[i].counter = 0;
            tasks[i].isReady = true;
        }
    }
}

void Scheduler::processTasks() {
    for(uint8_t i = 0; i < Scheduler::taskCount; i++){
        if(tasks[i].isReady) {
            tasks[i].isReady = false;
            tasks[i].callback();
        }
    }
}

void Scheduler::initialize() {
    taskCount = 0;
    systemTicks = 0;

    TIMSK2 = TIMER2_ALL_INTERRUPT_DISABLE;
    ASSR = TIMER2_EXTERNAL_OSCILLATOR_CLOCK;
    TCNT2 = 0;
    OCR2A = 0xff; // 128 ticks per second >> period = 1000/128 = 125/16 = 7.8125 ms
    OCR2B = 0x00;
    TCCR2A = TIMER2_NORMAL_PORT_OPERATION_OC0A_DISCONNECTED | TIMER2_NORMAL_PORT_OPERATION_OC0B_DISCONNECTED | TIMER2_CTC_MODE_A;
    TCCR2B = TIMER2_CTC_MODE_B | TIMER2_PRESCALER_DIV1;

    while (ASSR & TIMER2_ALL_BUSY_BITS); // waiting for register update

    TIFR2 = (1 << OCF2A) | (1 << OCF2B) | (1 << TOV2); // reset interrupt flags
    TIMSK2 = TIMER2_OUTPUT_COMPARE_MATCH_A_INTERRUPT_ENABLE;
}

const uint8_t SLEEP_TASK_TIME = 26;
const uint8_t WHILE_TIME = 64;

void Scheduler::run() {
    //set_sleep_mode(SLEEP_MODE_IDLE);
    while(true) {
        processTasks();
        //sleep_mode();                    // Enter Sleep Mode To Save Power CPU Will Wake Up From Timer2 Interrupt

        // for calc MCU load
        cycleCount += WHILE_TIME + SLEEP_TASK_TIME * taskCount;
    }
}

bool Scheduler::addTask(uint16_t period_ms, TProcedurePointer callback)
{
    if (taskCount >= MAX_TASK_COUNT) {
        return false;
    }

    uint32_t period = round_div(period_ms * 16, 125); // 7.8125 ms on period period_ms / 7.8125 = period_ms * 16 / 125;

    tasks[taskCount].callback = callback;
    tasks[taskCount].period = period;
    tasks[taskCount].counter = 0;
    tasks[taskCount].isReady = false;

    _MemoryBarrier();
    taskCount++;

    return true;
}

uint32_t Scheduler::getSystemTime() {
    return Scheduler::systemTicks / 128;
}
