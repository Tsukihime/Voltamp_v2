#ifndef FONT11X16_H_
#define FONT11X16_H_

#include "Font.h"

static constexpr uint8_t PROGMEM Font11x16Data[ 220 ] = {
    0x00, 0x00, 0x00, 0x7f, 0xfc, 0x67, 0xb0, 0x81, 0x0f, 0x7a, 0xc8, 0x23, 0x9e, 0xf0, 0x82, 0x0f,
    0x3c, 0x60, 0x83, 0xf9, 0x8f, 0x3f, 0x00, 0x00, 0x00, 0x30, 0xc0, 0x01, 0x0f, 0x6c, 0x20, 0x03,
    0x18, 0xc0, 0x00, 0x06, 0x30, 0x80, 0x01, 0x0c, 0x60, 0x00, 0x03, 0x18, 0x00, 0x00, 0x00, 0x7f,
    0xfc, 0x77, 0xf0, 0x01, 0x03, 0x18, 0x60, 0xc0, 0x83, 0x07, 0x0f, 0x1c, 0x70, 0x80, 0x01, 0xfc,
    0xff, 0xff, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x73, 0xb8, 0x81, 0x01, 0x0c, 0x3e, 0xf0, 0x01, 0x1c,
    0x80, 0x01, 0x3c, 0xe0, 0x83, 0xfb, 0x8f, 0x3f, 0x00, 0x00, 0x00, 0x60, 0x80, 0x03, 0x1e, 0xd8,
    0x60, 0x86, 0x31, 0x86, 0x19, 0xcc, 0xff, 0xff, 0x0f, 0x18, 0xc0, 0x00, 0x06, 0x30, 0x00, 0x00,
    0x80, 0xff, 0xfc, 0x67, 0x80, 0x03, 0x0c, 0x60, 0x3f, 0xff, 0x3b, 0x38, 0x80, 0x01, 0x0c, 0xe0,
    0x83, 0xfb, 0x8f, 0x3f, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x77, 0xf0, 0x01, 0x0c, 0x60, 0x3f, 0xff,
    0x3b, 0xf8, 0x80, 0x07, 0x3c, 0xe0, 0x83, 0xfb, 0x8f, 0x3f, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x0f,
    0x70, 0xc0, 0x01, 0x07, 0x18, 0x60, 0x00, 0x03, 0x0c, 0x60, 0x80, 0x01, 0x0c, 0x30, 0x80, 0x01,
    0x00, 0x00, 0x00, 0x7f, 0xfc, 0x67, 0x30, 0x83, 0x31, 0x86, 0x3f, 0xfe, 0x3b, 0xf8, 0x80, 0x07,
    0x3c, 0xe0, 0x83, 0xfb, 0x8f, 0x3f, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x77, 0xf0, 0x01, 0x0f, 0x78,
    0xc0, 0x07, 0xf7, 0x3f, 0xbf, 0x01, 0x0c, 0xe0, 0x83, 0xfb, 0x8f, 0x3f, };

Font Font11x16(11, 16, Font11x16Data);

#endif /* FONT11X16_H_ */
