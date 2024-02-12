#ifndef __WIDGETS_COLOR_SELECTOR_H__
#define __WIDGETS_COLOR_SELECTOR_H__

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

#include "constants.h"

class ColorSelector {

    constexpr static uint16_t colors[3][3] {
        {RED, GREEN, BLUE},
        {CYAN, MAGENTA, YELLOW},
        {WHITE, GRAY, BLACK}
    };

    MCUFRIEND_kbv *tft;

    uint16_t x;
    uint16_t y;

    uint16_t hpad;
    uint16_t vpad;

    uint16_t paintRadius;

    uint16_t curColor = WHITE;

public:

    ColorSelector(uint16_t x, uint16_t y, uint16_t hpad, uint16_t vpad, uint16_t paintRadius, MCUFRIEND_kbv *tft);

    void    draw() const;
    bool    update(uint16_t touchX, uint16_t touchY);

    uint16_t getColor() const;

private:


};

#endif
