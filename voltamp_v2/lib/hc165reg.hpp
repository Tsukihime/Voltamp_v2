#ifndef HC165REG_H_
#define HC165REG_H_

#include "AVRPin.hpp"

template<class ClockPin, class DataPin, class LatchPin, class T = uint8_t>
class HC165Reg {
    public:
        static void initialize() {
            ClockPin::SetDirWrite();
            LatchPin::SetDirWrite();
            DataPin::SetDirRead();
            LatchPin::Set();
        }

        static void Latch() {
            LatchPin::Clear();
            LatchPin::Set();
        }

        static void Clock() {
            ClockPin::Set();
            ClockPin::Clear();
        }

        static uint8_t Read() {
            Latch();
            return ReadNext();
        }

        static uint8_t ReadNext() {
            uint8_t result = 0;
            for (uint8_t i = 0; i < 8; ++i) {
                result <<= 1;
                if(DataPin::IsSet()) {
                    result |= 1;
                }
                Clock();
            }
            return result;
        }
};

#endif /* HC165REG_H_ */
