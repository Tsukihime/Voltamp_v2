#ifndef HC595REG_H_
#define HC595REG_H_

#include "AVRPin.hpp"

template<class ClockPin, class DataPin, class LatchPin, class T = uint8_t>
class HC595Reg {
    public:
        static void initialize() {
            ClockPin::SetDirWrite();
            LatchPin::SetDirWrite();
            DataPin::SetDirWrite();
        }

        static void Latch() {
            LatchPin::Set();
            LatchPin::Clear();
        }

        static void Clock() {
            ClockPin::Set();
            ClockPin::Clear();
        }

        static void WriteLSBFirst(uint8_t byte) {
            for(int i = 0; i < 8; i++) {
                DataPin::Set(byte & 1);
                byte >>= 1;
                Clock();
            }
        }

        static void WriteMSBFirst(uint8_t byte) {
            for(int i = 0; i < 8; i++) {
                DataPin::Set(byte & 0x80);
                byte <<= 1;
                Clock();
            }
        }
};

#endif /* HC595REG_H_ */
