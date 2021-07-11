#include <avr/io.h>

#include "VoltageSelector.h"
#include "lib/hc595reg.hpp"

typedef HC595Reg<Pd4, Pd5, Pd6> SelectorReg;

static uint8_t port = 0;

void updatePort() {
    SelectorReg::WriteMSBFirst(port);
    SelectorReg::Latch();
}

void setPortBit(uint8_t bit, bool state) {
    if(state) {
        port |= (1 << bit);
        } else {
        port &= ~(1 << bit);
    }
}

void VoltageSelector::initialize() {
    SelectorReg::initialize();
}

void VoltageSelector::setBluetoothState(bool state){
    setPortBit(6, state);
    updatePort();
}

void VoltageSelector::setCVLedState(bool state) {
    setPortBit(4, state);
    updatePort();
}

void VoltageSelector::setCCLedState(bool state) {
    setPortBit(5, state);
    updatePort();
}

void VoltageSelector::setVoltageLevel(uint8_t level) {
    port &= ~(1 << 1 | 1 << 2 | 1 << 3);
    switch(level) {
        case 1:
            setPortBit(3, true);
            break;
        case 2:
            setPortBit(2, true);
            break;
        case 3:
            setPortBit(1, true);
            break;
        case 0: ;
        default: ;
    }
    updatePort();
}
