#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include "Arduino.h"

#include "constants.h"

inline int charWidthOfFont(const unsigned char letter, const uint8_t *font)
{
    unsigned char c = letter;
    // Space is often not included in font so use width of 'n'
    if (c == ' ')
        c = 'n';
    uint8_t width = 0;

    uint8_t firstChar = pgm_read_byte(font + FONT_FIRST_CHAR);
    uint8_t charCount = pgm_read_byte(font + FONT_CHAR_COUNT);

    uint16_t index = 0;

    if (c < firstChar || c >= (firstChar + charCount))
    {
        return 0;
    }
    c -= firstChar;

    if (pgm_read_byte(font + FONT_LENGTH) == 0 && pgm_read_byte(font + FONT_LENGTH + 1) == 0)
    {
        // zero length is flag indicating fixed width font (array does not contain width data entries)
        width = pgm_read_byte(font + FONT_FIXED_WIDTH);
    }
    else
    {
        // variable width font, read width data
        width = pgm_read_byte(font + FONT_WIDTH_TABLE + c);
    }
    return width;
}

#endif