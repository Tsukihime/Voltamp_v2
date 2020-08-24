#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "utils.h"
#include "Pwm.h"
#include "Bluetooth.h"
#include "Multimeter.h"
#include "Scheduler.h"
#include "ST7735Lcd.h"
#include "Dac.h"
#include "ST7735Lcd.h"
#include "lib/hc165reg.hpp"
#include "fonts/Font16x24.hpp"

uint16_t i = 0;

const uint16_t BORDER_GREEN = 0x65CC;
const uint16_t BORDER_YELLOW = 0xE757;

void draw_lcd() {
    ST7735Lcd::FillRectangle(2, 0, 156, 2, BORDER_GREEN);  
    ST7735Lcd::FillRectangle(2, 39, 156, 2, BORDER_GREEN);       
    ST7735Lcd::FillRectangle(2, 76, 156, 1, BORDER_GREEN);

    ST7735Lcd::FillRectangle(0, 0, 2, 77, BORDER_GREEN);
    ST7735Lcd::FillRectangle(158, 0, 2, 77, BORDER_GREEN);

    ST7735Lcd::FillRectangle(2, 77, 156, 1, BORDER_YELLOW);
    ST7735Lcd::FillRectangle(2, 101, 156, 2, BORDER_YELLOW);
    ST7735Lcd::FillRectangle(2, 126, 156, 2, BORDER_YELLOW);

    ST7735Lcd::FillRectangle(0, 77, 2, 76, BORDER_YELLOW);
    ST7735Lcd::FillRectangle(158, 77, 2, 76, BORDER_YELLOW);

    uint16_t bitmap[Font16x24::GetCharPixelCount()];
    Font16x24::RenderChar(3, 0, 0xF0FF, bitmap);
    ST7735Lcd::DrawImage(10,10,16,24, bitmap);
}

void dac_test(){

    static uint16_t i = 0;
    uint16_t out = i;
    if(bit::test(i, 4)) {
        //out++;
    }
    
    Dac::setVoltage(out);

    i+=1;
    

/*
     static uint16_t voltage = 0;
     dac.set_values(voltage, 65535 - voltage);

     uint8_t digits[6];
     bin2bcd5(voltage, digits);
     for(int i = 0; i < 5; i++) {
         digits[i] += '0';
     }
     digits[5] = 0;

     voltage++;

     uart.sendString((char*)digits);
     uart.sendChar(13);
    */
}

void print_value(uint16_t value) {
 uint8_t digits[6];
 bin2bcd5(value, digits);
 bcd2ascii(digits);

 for(int i = 0; i < 4; i++) {
     if (digits[i] != '0') {
        break;
     }
     digits[i] = ' ';
 }

 digits[5] = 0;
 Bluetooth::sendString((char*)digits);
}

void adc_test() {
    uint16_t voltage = (uint32_t)(Multimeter::getVoltage() - 29) * 2.5 * 1000 / 65536;
    uint16_t current = Multimeter::getCurrent();
    uint16_t time = Scheduler::getSystemTime();

    Bluetooth::sendString("----------");
    Bluetooth::sendChar(13);
    print_value(voltage);
    Bluetooth::sendString(" Volt");
    Bluetooth::sendChar(13);
    print_value(current);
    Bluetooth::sendString(" Amp");
    Bluetooth::sendChar(13);
    print_value(time);
    Bluetooth::sendString(" Sec");
    Bluetooth::sendChar(13);
}

void calcMCULoad() {
    uint8_t load = ((F_CPU - Scheduler::cycleCount) * 100 / F_CPU);
    Scheduler::cycleCount = 0;

    Bluetooth::sendString("**********");
    Bluetooth::sendChar(13);
    print_value(load);
    Bluetooth::sendString(" % Load");
    Bluetooth::sendChar(13);
}

typedef HC165Reg<Pd4, Pc1, Pc0> ButtonReg;

void latch_test() {
  uint8_t reg = ButtonReg::Read();
  Bluetooth::sendChar(reg);
}

int main(void) {
    Bluetooth::initialize();
    Dac::initialize(); 
    ButtonReg::initialize();
    Pwm::initialize();
    Multimeter::initialize();
    ST7735Lcd::initialize();
    Scheduler::initialize();
    
    sei();
    
    Dac::enable();
    Dac::setCurrent(0);
    Dac::setVoltage(4094);
        
    //Lcd::FillRectangle(0, 0, 160, 128, ST7735_COLOR565(0,0,0));

    //Scheduler::addTask(1000, &calcMCULoad);

    Scheduler::addTask(200, &draw_lcd);
    //Scheduler::addTask(100, &adc_test);
    //Scheduler::addTask(100, &latch_test);
    //Scheduler::addTask(10, &dac_test);
    
    Scheduler::run();
}
