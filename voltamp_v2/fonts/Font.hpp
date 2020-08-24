#ifndef FONT_H_
#define FONT_H_

#include <avr/pgmspace.h>

template<const uint8_t FontData[], uint8_t Width, uint8_t Height, class TPixel = uint16_t>
class Font {
    public:
    static constexpr uint16_t GetCharPixelCount() {
        return Width * Height;
    }

    static constexpr uint16_t GetWidth() {
        return Width;
    }

    static constexpr uint16_t GetHeight() {
        return Height;
    }

    static void RenderChar(uint8_t Charcode, TPixel ForegroundColor, TPixel BackgroundColor, TPixel* buffer) {
        uint16_t packedCharSize = Width * Height / 8;
        uint16_t offset = Charcode * packedCharSize;
        for(uint16_t i = offset; i < packedCharSize + offset; i++) {
            uint8_t byte = pgm_read_byte(&FontData[i]);
            for(int k = 0; k < 8; k++) {
                if(byte & 1) {
                    *buffer++ = ForegroundColor;
                } else {
                    *buffer++ = BackgroundColor;
                }
                byte >>= 1;
            }
        }
    }
};



#endif /* FONT_H_ */