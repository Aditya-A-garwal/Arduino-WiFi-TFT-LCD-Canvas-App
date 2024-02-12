#ifndef __WIDGETS_THICKNESS_SELECTOR_H__
#define __WIDGETS_THICKNESS_SELECTOR_H__

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

class ThicknessSelector {

    const uint16_t radii[4] = {3, 5, 7, 9};

    MCUFRIEND_kbv *tft;

    uint16_t x;
    uint16_t y;

    uint16_t pad;

    uint16_t color;
    uint16_t curSelected;

public:

    ThicknessSelector(uint16_t x, uint16_t y, uint16_t pad, MCUFRIEND_kbv *tft);

    void    setColor(uint16_t clr);

    void    draw() const;
    bool    update(uint16_t touchX, uint16_t touchY);
    uint16_t getThickness() const;

private:


};


#endif
