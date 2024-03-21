#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_TOUCHSCREEN_DRIVER_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_TOUCHSCREEN_DRIVER_H__

#include "Arduino.h"
#include "TouchScreen.h"

class Touchscreen {

    //  UNO R4 MINIMA
    constexpr static unsigned XBEGIN = 177;
    constexpr static unsigned XEND = 863;
    constexpr static unsigned YBEGIN = 121;
    constexpr static unsigned YEND = 950;

    TouchScreen ts;
    TSPoint p;

    int xp;
    int yp;
    int xm;
    int ym;

    unsigned pressure_lo {10};
    unsigned pressure_hi {1400};

    unsigned width {320};
    unsigned height {480};


    bool is_pressed {false};
    bool press_available {false};
    bool release_available {false};

    unsigned press_x {0};
    unsigned press_y {0};

    unsigned release_x {0};
    unsigned release_y {0};

    unsigned cur_x {0};
    unsigned cur_y {0};

public:

    Touchscreen() = delete;

    Touchscreen(int xp, int yp, int xm, int ym)
    : xp {xp}
    , yp {yp}
    , xm {xm}
    , ym {ym}
    , ts(xp, yp, xm, ym, 300)
    {}

    void set_dimensions(unsigned w, unsigned h) {
        width = w;
        height = h;
    }

    void set_pressure(unsigned plo, unsigned phi) {
        pressure_lo = plo;
        pressure_hi = phi;
    }

    bool get_press(unsigned *x, unsigned *y) {

        if (!press_available) {
            return false;
        }

        press_available = false;

        *x = press_x;
        *y = press_y;

        return true;
    }

    bool get_release(unsigned *x, unsigned *y) {

        if (!release_available) {
            return false;
        }

        release_available = false;

        *x = release_x;
        *y = release_y;

        return true;
    }

    bool get_stylus_position(unsigned *x, unsigned *y) {

        if (!is_pressed) {
            return false;
        }

        *x = cur_x;
        *y = cur_y;

        return true;
    }

    void read_screen() {

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

private:

    void reset() {
        pinMode(xp, OUTPUT);
        pinMode(yp, OUTPUT);
        pinMode(xm, OUTPUT);
        pinMode(ym, OUTPUT);
    }

    void convert(unsigned *x, unsigned *y) {

        *x = constrain(*x, XBEGIN, XEND);
        *y = constrain(*y, YBEGIN, YEND);

        *x = map(*x, XBEGIN, XEND, 0, width - 1);
        *y = map(*y, YBEGIN, YEND, height - 1, 0);
    }
};

#endif
