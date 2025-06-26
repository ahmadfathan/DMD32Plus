#include "DMDContainer.h"
#include <cstring>
#include <cstdlib>
#include "Arduino.h"
#include "utils.h"
#include "constants.h"

DMDContainer::DMDContainer(int16_t x0, int16_t y0, int16_t w, int16_t h)
{
    _x0 = x0;
    _y0 = y0;
    _w = w;
    _h = h;

    _buf = (uint8_t *)malloc(w * h);
    memset(_buf, 0, w * h);
}

uint8_t *DMDContainer::getBufferData()
{
    return _buf;
}

uint8_t DMDContainer::appendChar(int16_t x, int16_t y, uint8_t letter)
{
    if (x > _w || y > _h)
        return 0;
    if (_font == NULL)
        return 0;

    unsigned char c = letter;
    uint8_t height = pgm_read_byte(_font + FONT_HEIGHT);
    if (c == ' ')
    {
        int charWide = charWidthOfFont(' ', _font);
        return charWide;
    }
    uint8_t width = 0;
    uint8_t bytes = (height + 7) / 8;

    uint8_t firstChar = pgm_read_byte(_font + FONT_FIRST_CHAR);
    uint8_t charCount = pgm_read_byte(_font + FONT_CHAR_COUNT);

    uint16_t index = 0;

    if (c < firstChar || c >= (firstChar + charCount))
        return 0;
    c -= firstChar;

    if (pgm_read_byte(_font + FONT_LENGTH) == 0 && pgm_read_byte(_font + FONT_LENGTH + 1) == 0)
    {
        // zero length is flag indicating fixed width font (array does not contain width data entries)
        width = pgm_read_byte(_font + FONT_FIXED_WIDTH);
        index = c * bytes * width + FONT_WIDTH_TABLE;
    }
    else
    {
        // variable width font, read width data, to get the index
        for (uint8_t i = 0; i < c; i++)
        {
            index += pgm_read_byte(_font + FONT_WIDTH_TABLE + i);
        }
        index = index * bytes + charCount + FONT_WIDTH_TABLE;
        width = pgm_read_byte(_font + FONT_WIDTH_TABLE + c);
    }
    if (x < -width || y < -height)
        return width;

    for (uint8_t j = 0; j < width; j++)
    { // Width
        for (uint8_t i = bytes - 1; i < 254; i--)
        { // Vertical Bytes
            uint8_t data = pgm_read_byte(_font + index + j + (i * width));

            int offset = (i * 8);
            if ((i == bytes - 1) && bytes > 1)
            {
                offset = height - 8;
            }

            for (uint8_t k = 0; k < 8; k++)
            {
                if ((offset + k >= i * 8) && (offset + k <= height))
                {
                    int16_t posX = j + x + _x0;
                    int16_t posY = k + y + _y0 + offset;

                    if (posX < 0)
                        continue;
                    if (posX > (_x0 + _w))
                        continue;

                    _buf[posY * _w + posX] = (data & (1 << k)) >> k;
                }
            }
        }
    }

    return width;
}

uint16_t DMDContainer::appendText(int16_t x, int16_t y, const char *text, uint16_t length)
{
    uint16_t width = 0;

    for (uint16_t i = 0; i < length; i++)
    {
        width += appendChar(width + x, y, text[i]) + 1;
    }

    return width;
}

int16_t DMDContainer::getX0()
{
    return _x0;
}

int16_t DMDContainer::getY0()
{
    return _y0;
}

int16_t DMDContainer::getW()
{
    return _w;
}

int16_t DMDContainer::getH()
{
    return _h;
}

int16_t DMDContainer::getX1()
{
    return _w + _x0;
}

int16_t DMDContainer::getY1()
{
    return _h + _y0;
}

void DMDContainer::setFont(const uint8_t *font)
{
    _font = font;
}

void DMDContainer::clear()
{
    memset(_buf, 0, _w * _h);
}