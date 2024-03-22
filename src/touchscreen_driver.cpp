/**
 * @file                    touchscreen_driver.cpp
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   Definitions for members of `Touchscreen` class as given in [touchscreen_driver.h](touchscreen_driver.h)
 *
 */

#include "touchscreen_driver.h"

Touchscreen::Touchscreen(int xp, int yp, int xm, int ym)
    : xp {xp}
    , yp {yp}
    , xm {xm}
    , ym {ym}
    , ts(xp, yp, xm, ym, 300)
{}

void Touchscreen::set_dimensions(unsigned new_width, unsigned new_height) {
    width = new_width;
    height = new_height;
}

void Touchscreen::set_pressure_bounds(unsigned new_pressure_lo, unsigned new_pressure_hi) {
    pressure_lo = new_pressure_lo;
    pressure_hi = new_pressure_hi;
}

bool Touchscreen::get_press(unsigned *x, unsigned *y) {

    // if no press is available, return false without modifying the references
    // otherwise, update the press_available flag and store the coordinates in the references before returning true

    if (!press_available) {
        return false;
    }

    press_available = false;
    *x = press_x;
    *y = press_y;
    return true;
}

bool Touchscreen::get_release(unsigned *x, unsigned *y) {

    // if no release is available, return false without modifying the references
    // otherwise, update the release_available flag and store the coordinates in the references before returning true

    if (!release_available) {
        return false;
    }

    release_available = false;
    *x = release_x;
    *y = release_y;
    return true;
}

bool Touchscreen::get_stylus_position(unsigned *x, unsigned *y) {

    // if the screen is not currently pressed, return false without modifying the references
    // otherwise, update the references and return true

    if (!is_pressed) {
        return false;
    }

    *x = cur_x;
    *y = cur_y;
    return true;
}

void Touchscreen::read_screen() {

    // measure the current state of the screen and update the events accordingly
    // reset the pins to be digital outputs on completion

    unsigned x;
    unsigned y;
    bool is_pressed_new {false};

    p = ts.getPoint();
    if (pressure_lo <= p.z && p.z <= pressure_hi) {

        x = p.x;
        y = p.y;
        convert(&x, &y);
        is_pressed_new = true;
    }

    if (!is_pressed_new && is_pressed) { // the screen has been released

        is_pressed = false;
        release_available = true;

        release_x = press_x;
        release_y = press_y;
    }
    else if (is_pressed_new) { // the screen is currently pressed; no changed from last time
        if (is_pressed) { // position of touch not changed

            release_available = false;
            press_available = false;
        }
        else { // position of touch has been changed

            is_pressed = true;
            press_available = true;

            press_x = x;
            press_y = y;
        }

        cur_x = x;
        cur_y = y;
    }

    reset_pin_functions();
}


void Touchscreen::reset_pin_functions() {
    pinMode(xp, OUTPUT);
    pinMode(yp, OUTPUT);
    pinMode(xm, OUTPUT);
    pinMode(ym, OUTPUT);
}

void Touchscreen::convert(unsigned *x, unsigned *y) {

    *x = constrain(*x, XBEGIN, XEND);
    *y = constrain(*y, YBEGIN, YEND);

    *x = map(*x, XBEGIN, XEND, 0, width - 1);
    *y = map(*y, YBEGIN, YEND, height - 1, 0); // Y-coordinate is normally flipped
}
