#include <avr/io.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "utils.h"

Spi spi;

const uint8_t SPI_NO_INTERRUPT = (0 << SPIE);
const uint8_t SPI_ENABLE = (1 << SPE);
const uint8_t SPI_LSB_FIRST = (1 << DORD);
const uint8_t SPI_MSB_FIRST = (1 << DORD);
const uint8_t SPI_MASTER = (1 << MSTR);
const uint8_t SPI_PRESCALER_DIV4 = ((0 << SPR1) | (0 << SPR0));
const uint8_t SPI_DOUBLE_SPEED = (1 << SPI2X);
const uint8_t SPI_OUTPUTS = ((1 << 5) | (1 << 3)); // MOSI & SCK
const uint8_t SPI_INPUTS = (1 << 4); // MISO

void Spi::initialize(void) {
    DDRB |= SPI_OUTPUTS;
    DDRB &= ~SPI_INPUTS;
    PORTB &= ~(SPI_OUTPUTS | SPI_INPUTS);

    SPSR = SPI_DOUBLE_SPEED;
    SPCR = SPI_NO_INTERRUPT | SPI_ENABLE | SPI_LSB_FIRST | SPI_MASTER | (1 << CPOL) | (1 << CPHA) | SPI_PRESCALER_DIV4;
}

void Spi::sendData(uint8_t cData) {
    /* Start transmission */
    SPDR = cData;
    /* Wait for transmission complete */
    while(!(SPSR & (1 << SPIF)));
}
