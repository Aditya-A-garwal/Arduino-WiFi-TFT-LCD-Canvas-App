#include "helper.h"
#include "constants.h"

uint16_t code2color(uint8_t code) {

    switch (code) {

        case 0:
            return RED;
        case 1:
            return GREEN;
        case 2:
            return BLUE;
        case 3:
            return CYAN;
        case 4:
            return MAGENTA;
        case 5:
            return YELLOW;
        case 6:
            return WHITE;
        case 7:
            return GRAY;
        case 8:
            return BLACK;
    }
}

uint8_t color2code(uint16_t clr) {

    switch (clr) {

        case RED:
            return 0;
        case GREEN:
            return 1;
        case BLUE:
            return 2;
        case CYAN:
            return 3;
        case MAGENTA:
            return 4;
        case YELLOW:
            return 5;
        case WHITE:
            return 6;
        case GRAY:
            return 7;
        case BLACK:
            return 8;
    }
}

bool inRange(unsigned value, unsigned lo, unsigned hi) {
    return lo <= value && value <= hi;
}

uint32_t distance(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {

    uint32_t i = (x0 > x1) ? (x0 - x1) : (x1 - x0);
    uint32_t j = (y0 > y1) ? (y0 - y1) : (y1 - y0);

    return sqrt(i*i + j*j);
}
