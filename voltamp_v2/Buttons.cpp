#include <avr/io.h>
#include <util/atomic.h>

#include "Buttons.h"
#include "PowerSupplier.h"
#include "Bluetooth.h"

#include "lib/hc165reg.hpp"

typedef HC165Reg<Pc0, Pd3, Pc1> ButtonReg;

static uint8_t port;
static int16_t encoder1Delta = 0;
static int16_t encoder2Delta = 0;

void Buttons::initialize() {
    ButtonReg::initialize();
}

bool getState(uint8_t pin) {
    uint8_t p;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        p = port;
    }
    return p & (1 << pin);
}

bool Buttons::isVoltageEncoderPressed() {
    return getState(1);
}

bool Buttons::isCurrentEncoderPressed() {
    return getState(6);
}

bool Buttons::isOutputSwitchOn() {
    return getState(0);
}

bool Buttons::isConstantCurrentMode() {
    return !getState(7);
}

int8_t Buttons::voltageEncoderPopDelta() {
    int8_t result = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        result = encoder1Delta;
        encoder1Delta = 0;
    }
    return result;
}

int8_t Buttons::currentEncoderPopDelta() {
    int8_t result = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        result = encoder2Delta;
        encoder2Delta = 0;
    }
    return result;
}

static constexpr int8_t states[16] = {
     0, 0, 0, 0,
     0, 0, 0,+1,
     0, 0, 0, 0,
     0,-1, 0, 0
};

void Buttons::poolISR() {
    static uint8_t old1State = 0;
    static uint8_t old2State = 0;

    port = ~ButtonReg::Read();
    if(!isOutputSwitchOn()) {
        PowerSupplier::disable();
    } else {
        PowerSupplier::enable();
    }

    uint8_t enc1State = (port >> 2) & 0x03; // 2+3 => 0+1
    uint8_t index1 = (old1State << 2) | enc1State;
    encoder1Delta += states[index1];
    old1State = enc1State;

    uint8_t enc2State = (port >> 4) & 0x03; // 4+5 => 0+1
    uint8_t index2 = (old2State << 2) | enc2State;
    encoder2Delta += states[index2];
    old2State = enc2State;
}
