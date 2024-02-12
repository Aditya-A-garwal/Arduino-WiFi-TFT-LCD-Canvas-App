#ifndef __WIDGETS_BUTTON_H__
#define __WIDGETS_BUTTON_H__

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

class Button {

    MCUFRIEND_kbv *tft;

    uint16_t x;
    uint16_t y;

    uint16_t w;
    uint16_t h;

    uint16_t color;

    char *msg;

public:

    Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, char *msg, MCUFRIEND_kbv *tft);

    void    draw() const;
    bool    update(uint16_t touchX, uint16_t touchY);

private:


};

#endif
