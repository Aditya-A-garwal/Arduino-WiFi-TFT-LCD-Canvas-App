#include "widgets/button.h"
#include "constants.h"
#include "helper.h"

Button::Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, char *msg, MCUFRIEND_kbv *tft)
    : x {x}
    , y {y}
    , w {w}
    , h {h}
    , color {color}
    , msg {msg}
    , tft {tft}
{}

void Button::draw() const {

    tft->drawRect(x, y, w, h, color);

    tft->setCursor(x, y);
    tft->setTextColor(color, BLACK);
    tft->setTextSize(2);

    tft->print(msg);
}

bool Button::update(uint16_t touchX, uint16_t touchY) {

    if (!inRange(touchX, x, x + w) || !inRange(touchY, y, y + h)) {
        return false;
    }

    return true;
}
