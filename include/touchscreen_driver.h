/**
 * @file                    touchscreen_driver.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   Declarations of classes required to interact with a 4-wire resistive touchscreen
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_TOUCHSCREEN_DRIVER_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_TOUCHSCREEN_DRIVER_H__

#include "Arduino.h"
#include "TouchScreen.h"

/**
 * @brief                   Class for interacting with a resistive touchscreen
 */
class Touchscreen {

    /** Minimum raw coordinate in X-axis (for UNO R4 WiFi) */
    constexpr static unsigned XBEGIN = 177;
    /** Maximum raw coordinate in X-axis (for UNO R4 WiFi) */
    constexpr static unsigned XEND = 863;
    /** Minimum raw coordinate in Y-axis (for UNO R4 WiFi) */
    constexpr static unsigned YBEGIN = 121;
    /** Maximum raw coordinate in Y-axis (for UNO R4 WiFi) */
    constexpr static unsigned YEND = 950;

    /** Default value for member `pressure_lo` */
    constexpr static unsigned DEFAULT_PRESSURE_LO = 10;
    /** Default value for member `pressure_hi` */
    constexpr static unsigned DEFAULT_PRESSURE_HI = 1400;

    /** Internal instance of the touchscreen library */
    TouchScreen ts;
    /** Internal variable to store a touch point */
    TSPoint p;

    /** X+ pin of the 4-wire resistive touchscreen */
    int xp;
    /** Y+ pin of the 4-wire resistive touchscreen */
    int yp;
    /** X- pin of the 4-wire resistive touchscreen */
    int xm;
    /** Y- pin of the 4-wire resistive touchscreen */
    int ym;

    /** Lower bound on the pressure below which the touch gets rejected */
    unsigned pressure_lo {DEFAULT_PRESSURE_LO};
    /** Higher bound on the pressure after which the touch gets rejected */
    unsigned pressure_hi {DEFAULT_PRESSURE_HI};

    /** Number of columns on the touchscreen (in pixels) */
    unsigned width {320};
    /** Number of rows on the touchscreen (in pixels) */
    unsigned height {480};

    /** Flag indicating if the screen is currently pressed */
    bool is_pressed {false};
    /** Flag indicating if an unconsumed press is available */
    bool press_available {false};
    /** Flag indicating if an unconsumed release is available */
    bool release_available {false};

    /** X-ccoordinate of the last press event (only valid if `press_available` is true) */
    unsigned press_x {0};
    /** Y-ccoordinate of the last press event (only valid if `press_available` is true) */
    unsigned press_y {0};

    /** X-ccoordinate of the last release event (only valid if `release_available` is true) */
    unsigned release_x {0};
    /** Y-ccoordinate of the last release event (only valid if `release_available` is true) */
    unsigned release_y {0};

    /** X-ccoordinate of the current touch (only valid if `is_pressed` is true) */
    unsigned cur_x {0};
    /** Y-ccoordinate of the current touch (only valid if `is_pressed` is true) */
    unsigned cur_y {0};

public:

    /**
     * @brief               Default constructor disabled (use the parameter constructor)
     */
    Touchscreen() = delete;

    /**
     * @brief               Constructor for Touchscreen class
     *
     * @param xp            Microcontroller pin connected to X+ pin of the touchscreen
     * @param yp            Microcontroller pin connected to Y+ pin of the touchscreen
     * @param xm            Microcontroller pin connected to X- pin of the touchscreen
     * @param ym            Microcontroller pin connected to Y- pin of the touchscreen
     */
    Touchscreen(int xp, int yp, int xm, int ym);

    /**
     * @brief               Sets the width and height of the touchscreen (in pixels)
     *
     * @param new_width     Number of columns on the touchscreenn (in pixels)
     * @param new_height    Number of rows on the touchscreen (in pixels)
     */
    void set_dimensions(unsigned new_width, unsigned new_height);

    /**
     * @brief               Set the pressure bounds within which a measurement is considered a valid touch
     *
     * @param new_pressure_lo   Lower bound on pressure below which a touch is rejected
     * @param new_pressure_hi   Higher bound on pressure after which a touch is rejected
     */
    void set_pressure_bounds(unsigned plo, unsigned phi);

    /**
     * @brief               Returns the coordinates of the latest press event (if available)
     *
     * @param x             Pointer to store X-coordinate of the latest press (unchanged if no press event is available)
     * @param y             Pointer to store Y-coordinate of the latest press (unchanged if no press event is available)
     *
     * @return true         If a press event was available (the values pointed to by the pointers have been changed)
     * @return false        If a press event was not available (the values pointed to by the pointers have not been changed)
     */
    bool get_press(unsigned *x, unsigned *y);

    /**
     * @brief               Returns the coordinates of the latest release event (if available)
     *
     * @param x             Pointer to store X-coordinate of the latest release (unchanged if no press event is available)
     * @param y             Pointer to store Y-coordinate of the latest release (unchanged if no press event is available)
     *
     * @return true         If a release event was available (the values pointed to by the pointers have been changed)
     * @return false        If a release event was not available (the values pointed to by the pointers have not been changed)
     */
    bool get_release(unsigned *x, unsigned *y);

    /**
     * @brief               Returns the coordinates of the currently touched point on the screen (if available)
     *
     * @param y             Pointer to store Y-coordinate of the currently touched location (unchanged if the screen is not being touched)
     * @param x             Pointer to store X-coordinate of the currently touched location (unchanged if the screen is not being touched)
     *
     * @return true         If the screen is being touched (the values pointed to by the pointers have been changed)
     * @return false        If the screen is not being touched (the values pointed to by the pointers have not been changed)
     */
    bool get_stylus_position(unsigned *x, unsigned *y);

    /**
     * @brief               Reads the current status of the touchscreen and updates the press and release events
     *
     */
    void read_screen();

private:

    /**
     * @brief               Resets the pins to be digital outputs
     *
     */
    void reset_pin_functions();

    /**
     * @brief               Convert the coordinates from their raw-values to pixel locations
     *
     * @param x             Pointer to store converted X-coordinate
     * @param y             Pointer to store converted Y-coordinate
     */
    void convert(unsigned *x, unsigned *y);
};

#endif
