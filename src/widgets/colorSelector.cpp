#include "widgets/colorSelector.h"
#include "constants.h"
#include "helper.h"

ColorSelector::ColorSelector(unsigned x, unsigned y, MCUFRIEND_kbv *tft)
    : x {x}
    , y {y}
    , tft {tft}
    , curColor {WHITE}
{}


void ColorSelector::draw() const {

    unsigned i, j;
    uint16_t color;

    for (unsigned r = 0; r < 3; ++r) {

        for (unsigned c = 0; c < 3; ++c) {

            i = x + (c * HPAD);
            j = y + (r * VPAD);
            color = COLORS[r][c];

            tft->fillCircle(i, j, PAINT_RADIUS, color);
            tft->drawCircle(i, j, PAINT_RADIUS + 2, WHITE);
        }
    }
}

void ColorSelector::clear() const {
    tft->fillRect(x, y, WIDTH, HEIGHT, BLACK);
}

bool ColorSelector::update(unsigned touch_x, unsigned touch_y) {

    uint32_t i, j, d;

    for (unsigned r = 0; r < 3; ++r) {

        for (unsigned c = 0; c < 3; ++c) {

            i = x + (c * HPAD);
            j = y + (r * VPAD);

            d = distance(i, j, touch_x, touch_y);

            if (d <= PAINT_RADIUS) {

                curColor = COLORS[r][c];
                return true;
            }
        }
    }

    return false;
}

unsigned ColorSelector::height() const { //todo
    return HEIGHT;
}

unsigned ColorSelector::width() const { //todo
    return WIDTH;
}

uint16_t ColorSelector::getColor() const {
    return curColor;
}
