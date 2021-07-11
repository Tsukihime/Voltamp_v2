#include <avr/io.h>
#include <avr/interrupt.h>

#include "Dac.h"
#include "lib/hc595reg.hpp"

typedef HC595Reg<Pc4, Pc5, Pc3> DacReg;
using DacOEpin = Pc2;

void Dac::initialize(void) {
    DacOEpin::SetDirWrite();
    disable();
    DacReg::initialize();
    setValues(0, 0);
}

void Dac::enable() {
    DacOEpin::Clear();
}

void Dac::disable() {
    DacOEpin::Set();
}

void Dac::setValues(uint16_t voltage_ch, uint16_t current_ch) {
    DacReg::WriteLSBFirst(current_ch & 0xFF);
    uint8_t data = (voltage_ch << 4) | ((current_ch >> 8) & 0x0F);
    DacReg::WriteLSBFirst(data);
    DacReg::WriteLSBFirst(voltage_ch >> 4);
    DacReg::Latch();
}
