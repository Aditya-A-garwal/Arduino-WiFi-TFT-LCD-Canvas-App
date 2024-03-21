#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_TOUCHSCREEN_DRIVER_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_TOUCHSCREEN_DRIVER_H__

#include "Arduino.h"
#include "TouchScreen.h"

class Touchscreen {

    //  UNO R4
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

    Touchscreen(int xp, int yp, int xm, int ym);

    void set_dimensions(unsigned w, unsigned h);
    void set_pressure(unsigned plo, unsigned phi);

    bool get_press(unsigned *x, unsigned *y);
    bool get_release(unsigned *x, unsigned *y);
    bool get_stylus_position(unsigned *x, unsigned *y);

    void read_screen();

private:

    void reset();
    void convert(unsigned *x, unsigned *y);
};

#endif
