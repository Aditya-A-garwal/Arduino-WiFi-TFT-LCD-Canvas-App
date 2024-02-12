#ifndef __WIDGETS_CANVAS_H__
#define __WIDGETS_CANVAS_H__

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

class Canvas {

    MCUFRIEND_kbv *tft;

    uint16_t x;
    uint16_t y;

    uint16_t w;
    uint16_t h;

public:

    Canvas(uint16_t x, uint16_t y, uint16_t w, uint16_t h, MCUFRIEND_kbv *tft);

    void    draw() const;
    bool    update(uint16_t touchX, uint16_t touchY, uint16_t curThickness, uint16_t curColor);

    void    clear();

    uint16_t readPixel(uint16_t r, uint16_t c) const;
    void    writePixel(uint16_t r, uint16_t c, uint16_t color);

    uint16_t height() const;
    uint16_t width() const;

private:


};

#endif
