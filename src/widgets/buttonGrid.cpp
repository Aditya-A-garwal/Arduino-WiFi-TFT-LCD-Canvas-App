#include "widgets/buttonGrid.h"
#include "constants.h"
#include "helper.h"

ButtonGrid::ButtonGrid(unsigned x, unsigned y, unsigned w, unsigned h, unsigned cc, unsigned rc, unsigned button_w, unsigned button_h, MCUFRIEND_kbv *tft)
    : x {x}
    , y {y}
    , w {w}
    , h {h}
    , cc {cc}
    , rc {rc}
    , button_w {button_w}
    , button_h {button_h}
    , tft {tft}
    , slot {}
{}

void ButtonGrid::draw() const {

    unsigned i, j;

    tft->fillRect(x, y, w, h, BLACK);
    tft->drawRect(x, y, w, h, GRAY);

    tft->fillRoundRect(x + CLOSE_X, y + CLOSE_Y, CLOSE_W, CLOSE_W, CLOSE_W / 2, RED);

    tft->setTextColor(WHITE, BLACK);
    tft->setTextSize(2);

    for (unsigned r = 0; r < rc; ++r) {

        for (unsigned c = 0; c < cc; ++c) {

            j = y + VPAD + (button_h + VPAD)*r;
            i = x + HPAD + (button_w + HPAD)*c;

            tft->drawRect(i, j, button_w, button_h, WHITE);

            tft->setCursor(i, j);

            tft->print(msg);
            tft->print(1 + r + (c * rc));

            // delay(2000);
        }
    }
}

bool ButtonGrid::update(unsigned touch_x, unsigned touch_y) {

    if (inRange(touch_x, x + CLOSE_X, x + CLOSE_X + CLOSE_W)
        && inRange(touch_y, y + CLOSE_Y, y + CLOSE_Y + CLOSE_W)) { // close this menu

        slot = 0;
        return true;
    }

    for (unsigned r = 0; r < rc; ++r) {
        for (unsigned c = 0; c < cc; ++c) {

            if (inRange(
                    touch_x,
                    x + HPAD + (button_w + HPAD)*c,
                    x + (button_w + HPAD)*(c + 1)) &&
                inRange(
                    touch_y,
                    y + VPAD + (button_h + VPAD)*r,
                    y + (button_h + VPAD)*(r + 1))) {

                slot = 1 + r + (c * rc);
                return true;
            }
        }
    }

    return false;
}

void ButtonGrid::clear() const {

    tft->fillRect(x, y, w, h, BLACK);
}

unsigned ButtonGrid::height() const {
    return h;
}

unsigned ButtonGrid::width() const {
    return w;
}

unsigned ButtonGrid::getSlot() const {
    return slot;
}
