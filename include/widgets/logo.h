#ifndef __WIDGETS_LOGO_H__
#define __WIDGETS_LOGO_H__

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

class Logo {

    MCUFRIEND_kbv *tft;

    uint16_t x;
    uint16_t y;

    uint16_t color;

public:

    Logo(uint16_t x, uint16_t y, uint16_t color, MCUFRIEND_kbv *tft);

    void    draw() const;
    bool    update();

private:


};

#endif
