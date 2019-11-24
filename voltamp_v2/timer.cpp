#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>
#include "timer.h"
#include "utils.h"

Timer timer;

const uint16_t period = F_CPU / 256 / 100 - 1; // every 10 ms
uint16_t calls = period;

void __vector_updateTaskStatusFromNakedISR() {
    timer.updateTaskStatus();
    calls = period;
}

ISR(TIMER2_OVF_vect, ISR_NAKED) {
    asm volatile(
        "PUSH r31                                     \n\t"
        "IN r31, __SREG__                             \n\t"
        "PUSH r24                                     \n\t"
        "PUSH r25                                     \n\t" // prologue
        
        "LDS R24, calls                               \n\t" // calls--
        "LDS R25, calls+1                             \n\t"
        "SBIW R24, 1                                  \n\t"
        "STS calls, R24                               \n\t"
        "STS calls+1, R25                             \n\t"

        "POP r25                                      \n\t"
        "POP r24                                      \n\t"
        "BRNE return                                  \n\t" // if(calls != 0) { return; }

        "OUT __SREG__, R31                            \n\t" // else { __vector_updateTaskStatusFromNakedISR (); }
        "POP R31                                      \n\t"
        "JMP __vector_updateTaskStatusFromNakedISR    \n\t" // RETI from __vector_updateTaskStatusFromNakedISR

    "return:"
        "OUT __SREG__, R31                            \n\t"  // return;
        "POP R31                                      \n\t"
        "RETI                                         \n\t"
        :
        :
        : "r24", "r25", "r31"
        );
}

void Timer::initialize() {
    taskCount = 0;
    // init on combinePWM.cpp
}

void Timer::processTasks() {
    for(uint8_t i = 0; i < taskCount; i++){
        if(tasks[i].isReady) {
            tasks[i].isReady = false;
            tasks[i].callback();
        }
    }
}

void Timer::run() {
    while(true) {
        processTasks();
        //set_sleep_mode(SLEEP_MODE_IDLE); // Enter Sleep Mode To Save Power
        //sleep_mode();                    // CPU Will Wake Up From Timer2 Interrupt
    }
}

bool Timer::addTask(uint16_t period_ms, TProcedurePointer callback)
{
    period_ms /= 10;
    if (taskCount >= MAX_TIMER_TASK_COUNT) {
        return false;
    }

    tasks[taskCount].callback = callback;
    tasks[taskCount].period = period_ms;
    tasks[taskCount].counter = 0;
    tasks[taskCount].isReady = false;

    _MemoryBarrier();
    taskCount++;

    return true;
}
