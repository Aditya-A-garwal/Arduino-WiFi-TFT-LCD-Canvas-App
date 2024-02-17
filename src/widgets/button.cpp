#include "widgets/button.h"
#include "constants.h"
#include "helper.h"

Button::Button(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color, char *msg, MCUFRIEND_kbv *tft)
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

void Button::clear() const {

    tft->fillRect(x, y, w, h, BLACK);
}

bool Button::update(unsigned touchX, unsigned touchY) {

    if (!inRange(touchX, x, x + w) || !inRange(touchY, y, y + h)) {
        return false;
    }

    return true;
}

unsigned Button::height() const {
    return h;
}

unsigned Button::width() const {
    return w;
}
