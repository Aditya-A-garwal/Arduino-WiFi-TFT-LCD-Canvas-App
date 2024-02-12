#include "widgets/logo.h"
#include "constants.h"
#include "helper.h"

Logo::Logo(uint16_t x, uint16_t y, uint16_t color, MCUFRIEND_kbv *tft)
    : x {x}
    , y {y}
    , color {color}
    , tft {tft}
{}

void Logo::draw() const {

    tft->setCursor(x, y);
    tft->setTextColor(color);
    tft->setTextSize(3);

    tft->print("Dumblebots");
}

bool Logo::update() {
    return false;
}
