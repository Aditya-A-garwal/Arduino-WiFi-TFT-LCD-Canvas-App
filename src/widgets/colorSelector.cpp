#include "widgets/colorSelector.h"
#include "constants.h"
#include "helper.h"

ColorSelector::ColorSelector(uint16_t x, uint16_t y, uint16_t hpad, uint16_t vpad, uint16_t paintRadius, MCUFRIEND_kbv *tft)
    : x {x}
    , y {y}
    , hpad {hpad}
    , vpad {vpad}
    , paintRadius {paintRadius}
    , tft {tft}
{}


void ColorSelector::draw() const {

    for (uint16_t r = 0; r < 3; ++r) {

        for (uint16_t c = 0; c < 3; ++c) {

            uint16_t i = x + (c * hpad);
            uint16_t j = y + (r * vpad);
            uint16_t color = colors[r][c];

            tft->fillCircle(i, j, paintRadius, color);
            tft->drawCircle(i, j, paintRadius + 2, WHITE);
        }
    }
}

bool ColorSelector::update(uint16_t touchX, uint16_t touchY) {

    uint32_t i, j, d;

    for (uint16_t r = 0; r < 3; ++r) {

        for (uint16_t c = 0; c < 3; ++c) {

            i = x + (c * hpad);
            j = y + (r * vpad);

            d = distance(i, j, touchX, touchY);

            if (d <= PAINT_RADIUS) {

                curColor = colors[r][c];
                return true;
            }
        }
    }

    return false;
}

uint16_t ColorSelector::getColor() const {
    return curColor;
}
