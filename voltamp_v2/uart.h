#ifndef UART_H
#define UART_H

#include <string.h>

#include "config.h"

extern "C" void USART_TX_vect(void) __attribute__((signal));

class Uart {        
    friend void USART_TX_vect();

    private:    
        char sendBuffer[UART_SEND_BUFFER_SIZE];        
        volatile uint8_t currentByteIndex;
        volatile uint8_t remainingDataSize;

        void startDataTransmit();
        void ISRTransmitComplete();

    public:
        void initialize(void);
        void sendData(const void *data, size_t length);
        void sendChar(const char ch);
        void sendString(const char* str);
};

extern Uart uart;

#endif /* UART_H */
