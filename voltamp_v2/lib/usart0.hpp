#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include <avr/sfr_defs.h>

class Usart0 {
    public:
        enum InterruptFlags {
            NoInterrupt = 0,
            RxNotEmptyInt = _BV(RXCIE0),
            TxCompleteInt = _BV(TXCIE0),
            TxEmptyInt = _BV(UDRIE0),
            AllInterrupts = RxNotEmptyInt | TxCompleteInt | TxEmptyInt
        };

        enum UsartMode { /* 0x UCSRC UCSRB */
            DataBits5 = 0,
            DataBits6 = _BV(UCSZ00) << 8,
            DataBits7 = _BV(UCSZ01) << 8,
            DataBits8 = _BV(UCSZ01) << 8 | _BV(UCSZ00) << 8,
            DataBits9 = _BV(UCSZ02) | _BV(UCSZ01) << 8 | _BV(UCSZ00) << 8,

            NoneParity = 0,
            EvenParity = _BV(UPM01) << 8,
            OddParity = _BV(UPM00) << 8 | _BV(UPM01) << 8,

            Disabled = 0,
            RxEnable = _BV(RXEN0),
            TxEnable = _BV(TXEN0),
            RxTxEnable = RxEnable | TxEnable,

            OneStopBit = 0,
            HalfStopBit = 0,
            TwoStopBits = _BV(USBS0) << 8,
            OneAndHalfStopBits = _BV(USBS0) << 8,

            Default = RxTxEnable | DataBits8 | NoneParity | OneStopBit
        };

        enum Error {
            NoError = 0,
            OverrunError = _BV(DOR0),
            NoiseError = 0,
            FramingError = _BV(FE0),
            ParityError = _BV(UPE0),
            ErrorMask = OverrunError | FramingError | ParityError
        };

        static inline void SetBaudRate(uint32_t baund) {
            uint32_t clock = F_CPU;
            uint16_t ubrr = (clock / 16 / baund - 1);
            uint16_t ubrr2x = (clock / 8 / baund - 1);
            uint32_t rbaund = (clock / 16 / (ubrr + 1));
            uint32_t rbaund2x = (clock / 8 / (ubrr2x + 1));

            uint32_t err1;
            if (baund > rbaund) {
                err1 = (baund - rbaund) * 1000 / baund;
                } else {
                err1 = (rbaund - baund) * 1000 / rbaund;
            }

            uint32_t err2;
            if (baund > rbaund2x) {
                err2 = (baund - rbaund2x) * 1000 / baund;
                } else {
                err2 = (rbaund2x - baund) * 1000 / rbaund2x;
            }

            uint16_t ubrrToUse;
            if (err1 > err2) {
                UCSR0A |= _BV(U2X0);
                ubrrToUse = ubrr2x;
                } else {
                UCSR0A &= ~_BV(U2X0);
                ubrrToUse = ubrr;
            }
            UBRR0 = ubrrToUse;
        }

        static void DisableTx() {
            UCSR0B &= ~_BV(TXEN0);
        }

        static void EnableTx() {
            UCSR0B |= _BV(TXEN0);
        }

        static inline void Init(unsigned long baud, UsartMode usartMode = Default) {
            UCSR0C = ((uint8_t)(usartMode >> 8));
            UCSR0B = ((uint8_t)(usartMode));
            SetBaudRate(baud);
        }

        static void EnableInterrupt(InterruptFlags interruptSources = AllInterrupts) {
            UCSR0B |= (uint8_t) interruptSources;
        }

        static void DisableInterrupt(InterruptFlags interruptSources = AllInterrupts) {
            UCSR0B &= (uint8_t) ~interruptSources;
        }

        static void Write(uint8_t c) {
            while (!WriteReady())
            ;
            UDR0 = c;
        }

        static uint8_t Read() {
            while (!ReadReady())
            ;
            return UDR0;
        }

        static bool WriteReady() {
            return UCSR0A & _BV(UDRE0);
        }

        static bool ReadReady() {
            return UCSR0A & _BV(RXC0);
        }

        static InterruptFlags InterruptSource() {
            uint8_t result = NoInterrupt;
            if (UCSR0A & _BV(RXC0)) {
                result |= RxNotEmptyInt;
            }
            if (UCSR0A & _BV(UDRE0)) {
                result |= TxEmptyInt;
            }
            if (UCSR0A & _BV(TXC0)) {
                result |= TxCompleteInt;
            }
            return (InterruptFlags)result;
        }

        static Error GetError() {
            return (Error)(UCSR0A & ErrorMask);
        }
};

#endif /* USART_H_ */