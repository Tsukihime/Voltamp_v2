#ifndef UTILS_H_
#define UTILS_H_

#define SET_PORT_BIT(p,b)       ((p) |= _BV(b))
#define CLR_PORT_BIT(p,b)       ((p) &= ~_BV(b))
#define TGL_PORT_BIT(p,b)       ((p) ^= _BV(b))
#define GET_PORT_BIT(p,b)       (((p) & _BV(b)) != 0)

extern void bin2bcd(uint16_t value, uint8_t buffer[4]);
extern void bin2bcd5(uint16_t value, uint8_t buffer[5]);
extern void bcd2text(uint8_t ints[4]);

#endif /* UTILS_H_ */
