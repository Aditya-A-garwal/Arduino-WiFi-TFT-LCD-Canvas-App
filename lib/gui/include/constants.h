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

constexpr uint16_t blend_color(uint16_t color1, uint16_t color2, unsigned d) {

    uint32_t b1 = (color1 >> 0) & 0x1F;
    uint32_t b2 = (color2 >> 0) & 0x1F;

    uint32_t g1 = (color1 >> 5) & 0x3F;
    uint32_t g2 = (color2 >> 5) & 0x3F;

    uint32_t r1 = (color1 >> 11) & 0x1F;
    uint32_t r2 = (color2 >> 11) & 0x1F;

    uint32_t b = ((b2 * d) + (b1 * (256 - d))) / 256;
    uint32_t g = ((g2 * d) + (g1 * (256 - d))) / 256;
    uint32_t r = ((r2 * d) + (r1 * (256 - d))) / 256;

    return (r << 11) | (g << 5) | (b << 0);
}

constexpr uint16_t code_2_color(uint8_t code) {

    switch(code) {
        case 0:         return RED;
        case 1:         return GREEN;
        case 2:         return BLUE;
        case 3:         return CYAN;
        case 4:         return MAGENTA;
        case 5:         return YELLOW;
        case 6:         return WHITE;
        case 7:         return GRAY;
        case 8:         return BLACK;
    }
}

constexpr uint8_t color_2_code(uint16_t color) {

    switch(color) {
        case RED:       return 0;
        case GREEN:     return 1;
        case BLUE:      return 2;
        case CYAN:      return 3;
        case MAGENTA:   return 4;
        case YELLOW:    return 5;
        case WHITE:     return 6;
        case GRAY:      return 7;
        case BLACK:     return 8;
    }
}

#endif
