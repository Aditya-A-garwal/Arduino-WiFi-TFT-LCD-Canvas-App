/**
 * @file                    touchscreen_constants.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   Constants required to initialize/operate the 4-wire touchscreen
 *
 */

#include "Arduino.h"

/** Lower bound on the pressure below which the touch gets rejected */
constexpr unsigned PRESSURE_LO = 10;
/** Higher bound on the pressure after which the touch gets rejected */
constexpr unsigned PRESSURE_RIGHT = 1400;

/** Positive X pin of the 4-wire touchscreen */
constexpr int XP = 8;
/** Negative X pin of the 4-wire touchscreen */
constexpr int XM = A2;
/** Positive Y pin of the 4-wire touchscreen */
constexpr int YP = A3;
/** Negative Y pin of the 4-wire touchscreen */
constexpr int YM = 9;