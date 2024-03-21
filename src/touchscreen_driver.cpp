#include "touchscreen_driver.h"

Touchscreen::Touchscreen(int xp, int yp, int xm, int ym)
    : xp {xp}
    , yp {yp}
    , xm {xm}
    , ym {ym}
    , ts(xp, yp, xm, ym, 300)
{}

void Touchscreen::set_dimensions(unsigned w, unsigned h) {
    width = w;
    height = h;
}

void Touchscreen::set_pressure(unsigned plo, unsigned phi) {
    pressure_lo = plo;
    pressure_hi = phi;
}

bool Touchscreen::get_press(unsigned *x, unsigned *y) {

    if (!press_available) {
        return false;
    }

    press_available = false;

    *x = press_x;
    *y = press_y;

    return true;
}

bool Touchscreen::get_release(unsigned *x, unsigned *y) {

    if (!release_available) {
        return false;
    }

    release_available = false;

    *x = release_x;
    *y = release_y;

    return true;
}

bool Touchscreen::get_stylus_position(unsigned *x, unsigned *y) {

    if (!is_pressed) {
        return false;
    }

    *x = cur_x;
    *y = cur_y;

    return true;
}

void Touchscreen::read_screen() {

    unsigned x, y;
    bool flag {false};

    p = ts.getPoint();
    if (pressure_lo <= p.z && p.z <= pressure_hi) {

        x = p.x, y = p.y;
        convert(&x, &y);

        flag = true;
    }

    if (!flag && is_pressed) { // new release

        is_pressed = false;
        release_available = true;

        release_x = press_x;
        release_y = press_y;
    }
    else if (flag) {
        if (is_pressed) { // slide (should be ignored)

            release_available = false;
            press_available = false;
        }
        else { // new press

            is_pressed = true;
            press_available = true;

            press_x = x;
            press_y = y;
        }

        cur_x = x;
        cur_y = y;
    }

    reset();
}


void Touchscreen::reset() {
    pinMode(xp, OUTPUT);
    pinMode(yp, OUTPUT);
    pinMode(xm, OUTPUT);
    pinMode(ym, OUTPUT);
}

void Touchscreen::convert(unsigned *x, unsigned *y) {

    *x = constrain(*x, XBEGIN, XEND);
    *y = constrain(*y, YBEGIN, YEND);

    *x = map(*x, XBEGIN, XEND, 0, width - 1);
    *y = map(*y, YBEGIN, YEND, height - 1, 0);
}
