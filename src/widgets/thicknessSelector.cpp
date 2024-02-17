#include "widgets/thicknessSelector.h"
#include "constants.h"
#include "helper.h"

ThicknessSelector::ThicknessSelector(unsigned x, unsigned y, MCUFRIEND_kbv *tft)
    : x {x}
    , y {y}
    , curSelected {RADII[0]}
    , tft {tft}
{}

void ThicknessSelector::setColor(uint16_t clr) {
    color = clr;
}

void ThicknessSelector::draw() const {

    unsigned i, j, t;

    for (unsigned c = 0; c < 4; ++c) {

        i = x + (PAD * c);
        j = y;
        t = RADII[c];

        (color == BLACK)
        ? tft->fillCircle(i, j, t, TFT_DARKGREY)
        : tft->fillCircle(i, j, t, color);

        (t == curSelected)
        ? tft->drawCircle(i, j, t + 3, WHITE)
        : tft->drawCircle(i, j, t + 3, BLACK);
    }
}

void ThicknessSelector::clear() const { //todo

}

bool ThicknessSelector::update(unsigned touch_x, unsigned touch_y) {

    unsigned i, j, t, d;

    for (unsigned c = 0; c < 4; ++c) {

        i = x + (PAD * c);
        j = y;
        t = RADII[c];

        d = distance(i, j, touch_x, touch_y);

        // the pen is in this color if the distance between its centre
        // and the pen is less than the radius of the paint
        if (d <= t) {

            curSelected = t;

            return true;
        }
    }

    return false;
}

unsigned ThicknessSelector::height() const {
    return HEIGHT;
}

unsigned ThicknessSelector::width() const {
    return WIDTH;
}

uint16_t ThicknessSelector::getThickness() const {
    return curSelected;
}
