#ifndef DMD_CONTAINER_H
#define DMD_CONTAINER_H

#include "stdint.h"

class DMDContainer
{
public:
    DMDContainer(int16_t x0, int16_t y0, int16_t w, int16_t h);
    uint8_t *getBufferData();
    uint8_t appendChar(int16_t x, int16_t y, uint8_t letter);
    uint16_t appendText(int16_t x, int16_t y, const char* text, uint16_t length);
    int16_t getX0();
    int16_t getY0();
    int16_t getW();
    int16_t getH();
    int16_t getX1();
    int16_t getY1();
    void setFont(const uint8_t *font);
    void clear();

private:
    int16_t _x0, _y0, _w, _h;
    uint8_t *_buf;
    const uint8_t *_font;
};

#endif