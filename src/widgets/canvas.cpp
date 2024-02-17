#include "widgets/canvas.h"
#include "constants.h"
#include "helper.h"

Canvas::Canvas(unsigned x, unsigned y, unsigned w, unsigned h, MCUFRIEND_kbv *tft)
    : x {x}
    , y {y}
    , w {w}
    , h {h}
    , tft {tft}
    , thickness {}
    , color {}
{}

void Canvas::draw() const {
    tft->drawRect(x, y, w, h, WHITE);
}

void Canvas::clear() const {
    tft->fillRect(x, y, w, h, BLACK);
}

bool Canvas::update(unsigned touch_x, unsigned touch_y) {

    if (inRange(touch_x, x + thickness + 2, x + w - thickness - 2)
        && inRange(touch_y, y + thickness + 2, y + h - thickness - 2)) {

            tft->fillCircle(touch_x, touch_y, thickness, color);
            return true;
    }
    return false;
}

unsigned Canvas::height() const {
    return h;
}

unsigned Canvas::width() const {
    return w;
}

unsigned Canvas::heightInternal() const {
    return h - 2;
}

unsigned Canvas::widthInternal() const {
    return w - 2;
}

void Canvas::setThickness(unsigned curThickness) {
    thickness = curThickness;
}

void Canvas::setColor(uint16_t curColor) {
    color = curColor;
}

void Canvas::clearDrawing() {
    tft->fillRect(x + 1, y + 1, w - 2, h - 2, BLACK);
}


uint16_t Canvas::readPixel(unsigned r, unsigned c) const {

    return tft->readPixel(x + c + 1, y + r + 1);
}

void Canvas::writePixel(unsigned r, unsigned c, uint16_t color) {

    tft->writePixel(x + c + 1, y + r + 1, color);
}
