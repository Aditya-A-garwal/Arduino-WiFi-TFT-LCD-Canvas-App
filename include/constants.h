#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "Arduino.h"

// display constants

const uint16_t RED     = 0xF800;
const uint16_t GREEN   = 0x07E0;
const uint16_t BLUE    = 0x001F;
const uint16_t CYAN    = 0x07FF;
const uint16_t MAGENTA = 0xF81F;
const uint16_t YELLOW  = 0xFFE0;
const uint16_t WHITE   = 0xFFFF;
const uint16_t GRAY    = 0x520A;
const uint16_t BLACK   = 0x0000;

const uint16_t DISPLAY_WIDTH   = 320;
const uint16_t DISPLAY_HEIGHT  = 480;

// touch constants

const uint16_t PRESSURE_LEFT = 10;
const uint16_t PRESSURE_RIGHT = 1200;

const int XP = 8;
const int XM = A2;
const int YP = A3;
const int YM = 9;

//  UNO R3
// const int XBEGIN = 129;
// const int XEND = 902;
// const int YBEGIN = 94;
// const int YEND = 961;

//  UNO R4 MINIMA
const int XBEGIN = 177;
const int XEND = 863;
const int YBEGIN = 121;
const int YEND = 950;

// application constants

const uint16_t LOGO_X = 80;
const uint16_t LOGO_Y = 4;
const uint16_t LOGO_C = WHITE;


const uint16_t CANVAS_W = 312;
const uint16_t CANVAS_H = 312;

const uint16_t CANVAS_X = 4;
const uint16_t CANVAS_Y = 30;


const uint16_t COLOR_SELECTOR_X = 40;
const uint16_t COLOR_SELECTOR_Y = CANVAS_Y + CANVAS_H + 30;

const uint16_t COLOR_SELECTOR_HPAD = 35;
const uint16_t COLOR_SELECTOR_VPAD = 35;

const uint16_t PAINT_RADIUS = 12;


const uint16_t THICKNESS_SELECTOR_X = 160;
const uint16_t THICKNESS_SELECTOR_Y = COLOR_SELECTOR_Y;
const uint16_t THICKNESS_SELECTOR_PAD = 35;


const uint16_t SAVE_X = 144;
const uint16_t SAVE_Y = COLOR_SELECTOR_Y + 56;

const uint16_t SAVE_W = 72;
const uint16_t SAVE_H = 28;

const uint16_t SAVE_C = GREEN;


const uint16_t LOAD_X = SAVE_X;
const uint16_t LOAD_Y = COLOR_SELECTOR_Y + 20;

const uint16_t LOAD_W = SAVE_W;
const uint16_t LOAD_H = SAVE_H;

const uint16_t LOAD_C = BLUE;


const uint16_t CLEAR_X = LOAD_X + 80;
const uint16_t CLEAR_Y = LOAD_Y;

const uint16_t CLEAR_W = SAVE_W;
const uint16_t CLEAR_H = SAVE_H;

const uint16_t CLEAR_C = RED;


const uint16_t INFO_X = CLEAR_X;
const uint16_t INFO_Y = SAVE_Y;

const uint16_t INFO_W = SAVE_W;
const uint16_t INFO_H = SAVE_H;

const uint16_t INFO_C = CYAN;


const uint16_t NUM_SLOTS = 6;

const uint16_t SLOT_MENU_X = COLOR_SELECTOR_X - 8;
const uint16_t SLOT_MENU_Y = COLOR_SELECTOR_Y - 8;

const uint16_t SLOT_MENU_W = SAVE_W * 3;
const uint16_t SLOT_MENU_H = SAVE_H * 4;

#endif