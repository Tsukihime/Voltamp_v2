#ifndef SPI_H
#define SPI_H

#include <string.h>

class Spi {        
    public:
        void initialize(void);
        void sendData(uint8_t cData);
};

extern Spi spi;

#endif /* SPI_H */
