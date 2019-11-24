#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "utils.h"

Uart uart;
volatile bool sendBufferEmpty = true;

#ifdef UART_DOUBLESPEED
    #define UART_CALC_BAUDRATE(baud) ((uint32_t)F_CPU / ((uint32_t)baud * 8) - 1)
#else
    #define UART_CALC_BAUDRATE(baud) ((uint32_t)F_CPU / ((uint32_t)baud * 16) - 1)
#endif

ISR(USART_TX_vect) {
    uart.ISRTransmitComplete();
}

void Uart::ISRTransmitComplete() {
    if(remainingDataSize == 0) {
        currentByteIndex = 0;
        sendBufferEmpty = true;
        CLR_PORT_BIT(UCSR0B, TXCIE0); // Disable the USART Transmit Complete interrupt (USART_TXC)
        return;
    }

    UDR0 = sendBuffer[currentByteIndex++];
    remainingDataSize--;
}

void Uart::startDataTransmit() {
    SET_PORT_BIT(UCSR0B, TXCIE0); // Enable the USART Transmit Complete interrupt (USART_TXC)
    ISRTransmitComplete();
}

void Uart::initialize(void) {
    // Set baud rate
    UBRR0H = (UART_CALC_BAUDRATE(UART_BAUDRATE) >> 8) & 0xFF;
    UBRR0L = (UART_CALC_BAUDRATE(UART_BAUDRATE) & 0xFF);

    UCSR0B = (1 << TXEN0) | (0 << UCSZ02); // Turn on the transmission circuitry

    UCSR0C = (0 << UMSEL01) | (0 << UMSEL00) | // Asynchronous Operation
             (0 << UPM01) | (0 << UPM00) |     // no parity
             (0 << USBS0) |                    // 1stop bit
             (1 << UCSZ01) | (1 << UCSZ00) |   // 8 bit data
             (0 << UCPOL0);

#ifdef UART_DOUBLESPEED
    UCSR0A = (1 << U2X);
#else
    UCSR0A = 0;
#endif
}

void Uart::sendChar(const char ch) {
    sendData(&ch, 1);
}

void Uart::sendData(const void *data, size_t length) {
    while(!sendBufferEmpty);    // wait while last transmit completes
    sendBufferEmpty = false;
    memcpy(sendBuffer, data, length);
    remainingDataSize = length;
    currentByteIndex = 0;
    uart.startDataTransmit();
}

void Uart::sendString(const char* str) {
    uint8_t len = strlen(str);
    if(len > UART_SEND_BUFFER_SIZE) {
        len = UART_SEND_BUFFER_SIZE;
    }
    sendData(str, len);
}

