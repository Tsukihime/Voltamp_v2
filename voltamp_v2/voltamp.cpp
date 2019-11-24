#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "utils.h"
#include "combinePWM.h"
#include "uart.h"
#include "adc.h"
#include "timer.h"
#include "spi.h"

const uint8_t ALL_INPUT = (0x00);
const uint8_t ALL_PULLUP = (0xff);

uint16_t i = 0;

void ladder() {
    static uint16_t level = 253;
    static uint8_t state = 0;

    switch(state) {
        case 0: level = 253;
        break;
        case 1: level = 254;
        break;
        case 2: level = 255;
        break;
        case 3: level = 256;
        break;
        case 4: level = 257;
        break;
        case 5: level = 258;
        break;
        case 6: level = 257;
        break;
        case 7: level = 256;
        break;
        case 8: level = 255;
        break;
        case 9: level = 254;
        break;
    }

    pwm.setPWMch1(level + 0 * 256);
    state++;
    if(state >= 10) {
        state = 0;
    }
}

void chainsaw() {
    pwm.setPWMch1(i);
    pwm.setPWMch0(i);
    i++;
}

void doit() {
     chainsaw();
     // ladder();
     spi.sendData(i);
     spi.sendData(i+1);
     spi.sendData(i+2);
     i++;
     uint16_t voltage = adc.getVoltage();
     uint8_t digits[6];

     bin2bcd5(voltage, digits);
     for(int i = 0; i < 5; i++) {
        digits[i] += '0';
     }
     digits[5] = 0;


     uart.sendString((char*)digits);
     uart.sendChar(13);
}

int main(void) {
    DDRB = ALL_INPUT | TIMER1_OUTPUTS;
    DDRC = ALL_INPUT | (1 << 1) | (1 << 0);
    DDRD = ALL_INPUT | TIMER0_OUTPUTS | TIMER2_OUTPUTS;

    PORTB = ALL_PULLUP & ~TIMER1_OUTPUTS;
    PORTC = ALL_PULLUP & ~((1 << 1) | (1 << 0));
    PORTD = ALL_PULLUP & ~(TIMER0_OUTPUTS | TIMER2_OUTPUTS);
    
    uart.initialize();
    timer.initialize();
    pwm.initialize();
    adc.initialize();
    spi.initialize();

    sei();
    timer.addTask(100, &doit);
    timer.run();
}
