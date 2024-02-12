#include "widgets/thicknessSelector.h"
#include "constants.h"
#include "helper.h"

ThicknessSelector::ThicknessSelector(uint16_t x, uint16_t y, uint16_t pad, MCUFRIEND_kbv *tft)
    : x {x}
    , y {y}
    , pad {pad}
    , curSelected {radii[0]}
    , tft {tft}
{}

void ThicknessSelector::setColor(uint16_t clr) {
    color = clr;
}

void ThicknessSelector::draw() const {

    uint16_t i, j, t;

    for (uint16_t c = 0; c < 4; ++c) {

        i = x + (pad * c);
        j = y;
        t = radii[c];

        (color == BLACK)
        ? tft->fillCircle(i, j, t, TFT_DARKGREY)
        : tft->fillCircle(i, j, t, color);

        (t == curSelected)
        ? tft->drawCircle(i, j, t + 3, WHITE)
        : tft->drawCircle(i, j, t + 3, BLACK);
    }
}

bool ThicknessSelector::update(uint16_t touchX, uint16_t touchY) {

    uint32_t i, j, t, d;

    for (uint16_t c = 0; c < 4; ++c) {

        i = x + (pad * c);
        j = y;
        t = radii[c];

        d = distance(i, j, touchX, touchY);

        // the pen is in this color if the distance between its centre
        // and the pen is less than the radius of the paint
        if (d <= t) {

            curSelected = t;

            return true;
        }
    }

    return false;
}

uint16_t ThicknessSelector::getThickness() const {
    return curSelected;
}
