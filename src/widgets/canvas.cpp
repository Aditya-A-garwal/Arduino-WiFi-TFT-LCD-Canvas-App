#include "widgets/canvas.h"
#include "constants.h"
#include "helper.h"

Canvas::Canvas(uint16_t x, uint16_t y, uint16_t w, uint16_t h, MCUFRIEND_kbv *tft)
    : x {x}
    , y {y}
    , w {w}
    , h {h}
    , tft {tft}
{}

void Canvas::draw() const {
    tft->drawRect(x, y, w, h, WHITE);
}

bool Canvas::update(uint16_t touchX, uint16_t touchY, uint16_t curThickness, uint16_t curColor) {

    if (inRange(touchX, x + curThickness + 2, x + w - curThickness - 2)
        && inRange(touchY, y + curThickness + 2, y + h - curThickness - 2)) {

            tft->fillCircle(touchX, touchY, curThickness, curColor);
            return true;
    }
    return false;
}

void Canvas::clear() {
    tft->fillRect(x + 1, y + 1, w - 2, h - 2, BLACK);
}


uint16_t Canvas::readPixel(uint16_t r, uint16_t c) const {

    return tft->readPixel(x + c, y + r);
}

void Canvas::writePixel(uint16_t r, uint16_t c, uint16_t color) {

    tft->writePixel(x + c, y + r, color);
}

uint16_t Canvas::height() const {
    return h;
}

uint16_t Canvas::width() const {
    return w;
}
