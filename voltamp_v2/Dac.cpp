#include <avr/io.h>
#include <avr/interrupt.h>

#include "Dac.h"
#include "lib/hc595reg.hpp"

typedef HC595Reg<Pc3, Pc2, Pc4> DacReg;
using DacOEpin = Pc5;

uint16_t Dac::voltage = 0;
uint16_t Dac::current = 0;

void Dac::initialize(void) {
    DacReg::initialize();
    DacOEpin::SetDirWrite();
    disable();
}

void Dac::enable() {
    DacOEpin::Clear();
}

void Dac::disable() {
    DacOEpin::Set();
}

void Dac::setVoltage(uint16_t voltage) {
     setValues(voltage, Dac::current);
}

void Dac::setCurrent(uint16_t current) {
    setValues(Dac::voltage, current);
}

void Dac::setValues(uint16_t voltage, uint16_t current){
    Dac::voltage = voltage;
    Dac::current = current;
    DacReg::WriteLSBFirst(current & 0xFF);

    uint8_t data = (voltage << 4) | (current & 0x0F);
    DacReg::WriteLSBFirst(data);
    DacReg::WriteLSBFirst(voltage >> 4);
    DacReg::Latch();
}
