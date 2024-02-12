#include "stdint.h"

uint16_t code2color(uint8_t code);
uint8_t color2code(uint16_t clr);

bool inRange(uint16_t value, uint16_t lo, uint16_t hi);

uint32_t distance(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);
