#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "Arduino.h"

// display constants

constexpr uint16_t RED     = 0xF800;
constexpr uint16_t GREEN   = 0x07E0;
constexpr uint16_t BLUE    = 0x001F;
constexpr uint16_t CYAN    = 0x07FF;
constexpr uint16_t MAGENTA = 0xF81F;
constexpr uint16_t YELLOW  = 0xFFE0;
constexpr uint16_t WHITE   = 0xFFFF;
constexpr uint16_t GRAY    = 0x520A;
constexpr uint16_t BLACK   = 0x0000;

constexpr unsigned DISPLAY_WIDTH   = 320;
constexpr unsigned DISPLAY_HEIGHT  = 480;

// touch constants

constexpr unsigned PRESSURE_LEFT = 10;
constexpr unsigned PRESSURE_RIGHT = 1400;

const int XP = 8;
const int XM = A2;
const int YP = A3;
const int YM = 9;

//  UNO R4 MINIMA
const int XBEGIN = 177;
const int XEND = 863;
const int YBEGIN = 121;
const int YEND = 950;

// application constants

constexpr unsigned CANVAS_X = 4;
constexpr unsigned CANVAS_Y = 10;

constexpr unsigned CANVAS_W = 312;
constexpr unsigned CANVAS_H = 332;

constexpr unsigned CANVAS_BUFFER_W = 32;
constexpr unsigned CANVAS_BUFFER_H = 32;

constexpr unsigned CANVAS_BUFFER_MAX_SEGMENTS = 18;
constexpr unsigned CLIENT_BUFFER_MAX_SEGMENTS = 105;


// constexpr unsigned COLOR_SELECTOR_X = 40;
// constexpr unsigned COLOR_SELECTOR_Y = CANVAS_Y + CANVAS_H + 30;
constexpr unsigned COLOR_SELECTOR_X = 12;
constexpr unsigned COLOR_SELECTOR_Y = CANVAS_Y + CANVAS_H + 7;

constexpr unsigned THICKNESS_SELECTOR_X = 145;
constexpr unsigned THICKNESS_SELECTOR_Y = COLOR_SELECTOR_Y - 3;


const unsigned LOAD_X = THICKNESS_SELECTOR_X;
const unsigned LOAD_Y = COLOR_SELECTOR_Y + 35;

const unsigned LOAD_W = 72;
const unsigned LOAD_H = 24;

const uint16_t LOAD_C = BLUE;


const unsigned SAVE_X = LOAD_X;
const unsigned SAVE_Y = LOAD_Y + 32;

const unsigned SAVE_W = LOAD_W;
const unsigned SAVE_H = LOAD_H;

const uint16_t SAVE_C = GREEN;


const unsigned CLEAR_X = LOAD_X + LOAD_W + 4;
const unsigned CLEAR_Y = LOAD_Y;

const unsigned CLEAR_W = SAVE_W;
const unsigned CLEAR_H = SAVE_H;

const uint16_t CLEAR_C = RED;


const unsigned INFO_X = CLEAR_X;
const unsigned INFO_Y = SAVE_Y;

const unsigned INFO_W = SAVE_W;
const unsigned INFO_H = SAVE_H;

const uint16_t INFO_C = CYAN;

const unsigned CONNECT_X = LOAD_X;
const unsigned CONNECT_Y = SAVE_Y + 32;

const unsigned CONNECT_W = 4 + (SAVE_W * 2);
const unsigned CONNECT_H = SAVE_H;

const uint16_t CONNECT_C = WHITE;


const unsigned NUM_SLOTS = 6;

const unsigned SLOT_MENU_X = COLOR_SELECTOR_X + 20;
const unsigned SLOT_MENU_Y = COLOR_SELECTOR_Y + 20;

const unsigned SLOT_MENU_W = SAVE_W * 3;
const unsigned SLOT_MENU_H = (SAVE_H * 9) / 2;


#endif