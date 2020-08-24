#ifndef TIMER_H_
#define TIMER_H_

#define MAX_TASK_COUNT 4

typedef void(*TProcedurePointer)();

extern "C" void TIMER2_COMPA_vect(void) __attribute__((interrupt)); 

class Scheduler {
    friend void TIMER2_COMPA_vect(void);

    public:
        static void initialize();
        static void processTasks();
        static void run();
        static uint32_t getSystemTime();

        static bool addTask(uint16_t period_ms, TProcedurePointer callback);
        static uint32_t cycleCount;

    private:
        typedef struct {
            uint16_t period;
            uint16_t counter;
            bool isReady;
            TProcedurePointer callback;
        } Task;

        static Task tasks[MAX_TASK_COUNT];
        static uint8_t taskCount;
        static uint32_t systemTicks;

        static inline void updateTaskStatus();
};

#endif /* TIMER_H_ */
