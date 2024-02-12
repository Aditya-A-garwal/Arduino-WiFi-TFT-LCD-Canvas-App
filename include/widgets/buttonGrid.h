#ifndef __WIDGETS_BUTTON_GRID_H__
#define __WIDGETS_BUTTON_GRID_H__

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

#include "constants.h"

class ButtonGrid {

    const char *msg = "SLOT"" ";

    const uint16_t SLOT_CLOSE_X = 2;
    const uint16_t SLOT_CLOSE_Y = 2;
    const uint16_t SLOT_CLOSE_W = 13;

    const uint16_t SLOT_MENU_C = GRAY;

    const uint16_t SLOT_OPTION_X = 23;
    const uint16_t SLOT_OPTION_Y = 8;

    const uint16_t SLOT_OPTION_W = SAVE_W;
    const uint16_t SLOT_OPTION_H = SAVE_H;
    const uint16_t SLOT_OPTION_C = WHITE;

    MCUFRIEND_kbv *tft;

    uint16_t x;
    uint16_t y;

    uint16_t w;
    uint16_t h;

    uint16_t cc;
    uint16_t rc;

    uint16_t slot;

public:

    ButtonGrid(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c, uint16_t r, MCUFRIEND_kbv *tft);

    void    draw() const;
    bool    update(uint16_t touchX, uint16_t touchY);

    void    clear();

    uint16_t getSlot() const;

private:


};

#endif
