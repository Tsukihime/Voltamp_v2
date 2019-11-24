#ifndef TIMER_H_
#define TIMER_H_

#define MAX_TIMER_TASK_COUNT 4

typedef void(*TProcedurePointer)();

// Define this function to have the "extra" prologue/etc needed by an ISR
extern "C" void __vector_updateTaskStatusFromNakedISR(void) __attribute__((signal));

class Timer {
    friend void __vector_updateTaskStatusFromNakedISR();

    public:
        void initialize();
        void processTasks();
        void run();

        bool addTask(uint16_t period_ms, TProcedurePointer callback);

    private:
        typedef struct {
            uint16_t period;
            uint16_t counter;
            bool isReady;
            TProcedurePointer callback;
        } TimerTask;

        TimerTask tasks[MAX_TIMER_TASK_COUNT];
        uint8_t taskCount;

        inline void updateTaskStatus() {
            for (uint8_t i = 0; i < taskCount; i++) {
                tasks[i].counter++;
                if(tasks[i].counter >= tasks[i].period) {
                    tasks[i].counter = 0;
                    tasks[i].isReady = true;
                }
            }
        }
};

extern Timer timer;

#endif /* TIMER_H_ */
