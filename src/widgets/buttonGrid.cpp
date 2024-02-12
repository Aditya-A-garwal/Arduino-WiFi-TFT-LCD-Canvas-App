#include "widgets/buttonGrid.h"
#include "constants.h"
#include "helper.h"

ButtonGrid::ButtonGrid(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c, uint16_t r, MCUFRIEND_kbv *tft)
    : x {x}
    , y {y}
    , w {w}
    , h {h}
    , cc {c}
    , rc {r}
    , tft {tft}
{}

void ButtonGrid::draw() const {

    tft->fillRect(x, y, w, h, BLACK);
    tft->drawRect(x, y, w, h, GRAY);

    tft->fillRoundRect(x + SLOT_CLOSE_X, y + SLOT_CLOSE_Y, SLOT_CLOSE_W, SLOT_CLOSE_W, SLOT_CLOSE_W / 2, RED);

    tft->setTextColor(WHITE, BLACK);
    tft->setTextSize(2);

    for (uint16_t r = 0; r < rc; ++r) {
        for (uint16_t c = 0; c < cc; ++c) {

            uint16_t i = x + SLOT_OPTION_X*(c + 1) + SLOT_OPTION_W*(c);
            uint16_t j = y + SLOT_OPTION_Y*(r + 1) + SLOT_OPTION_H*(r);

            tft->drawRect(
                i,
                j,
                SLOT_OPTION_W,
                SLOT_OPTION_H,
                WHITE
            );

            tft->setCursor(i, j);

            tft->print(msg);
            tft->print(1 + r + (c * rc));
        }
    }
}

bool ButtonGrid::update(uint16_t touchX, uint16_t touchY) {

    // close button pressed
    if (inRange(touchX, x + SLOT_CLOSE_X, x + SLOT_CLOSE_X + SLOT_CLOSE_W) && inRange(touchY, y + SLOT_CLOSE_Y, y + SLOT_CLOSE_Y + SLOT_CLOSE_W)) { // close this menu

        slot = 0;
        return true;
    }

    for (uint16_t r = 0; r < rc; ++r) {
        for (uint16_t c = 0; c < cc; ++c) {

            if (!inRange(
                    touchX,
                    x + SLOT_OPTION_X*(c + 1) + SLOT_OPTION_W*(c),
                    x + SLOT_OPTION_X*(c + 1) + SLOT_OPTION_W*(c + 1)) ||
                !inRange(
                    touchY,
                    y + SLOT_OPTION_Y*(r + 1) + SLOT_OPTION_H*(r),
                    y + SLOT_OPTION_Y*(r + 1) + SLOT_OPTION_H*(r + 1))) {
                continue;
            }

            slot = 1 + r + (c * rc);
            return true;
        }
    }

    return false;
}

void ButtonGrid::clear() {

    tft->fillRect(x, y, w, h, BLACK);
}

uint16_t ButtonGrid::getSlot() const {
    return slot;
}
