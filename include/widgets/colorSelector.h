#ifndef __WIDGETS_COLOR_SELECTOR_H__
#define __WIDGETS_COLOR_SELECTOR_H__

#include "widget.h"

#include "constants.h"

class ColorSelector : Widget {

    constexpr static uint16_t COLORS[3][3] {
        {RED, GREEN, BLUE},
        {CYAN, MAGENTA, YELLOW},
        {WHITE, GRAY, BLACK}
    };

    constexpr static unsigned HPAD = 35;
    constexpr static unsigned VPAD = 35;
    constexpr static unsigned PAINT_RADIUS = 12;

    constexpr static unsigned HEIGHT = (PAINT_RADIUS/2) + (3*HPAD);
    constexpr static unsigned WIDTH = (PAINT_RADIUS/2) + (3*VPAD);

    MCUFRIEND_kbv *tft;

    unsigned x;
    unsigned y;

    uint16_t curColor;

public:

    ColorSelector(unsigned x, unsigned y, MCUFRIEND_kbv *tft);

    void    draw() const override;
    void    clear() const override;

    bool    update(unsigned touch_x, unsigned touch_y) override;

    unsigned height() const;
    unsigned width() const;

    uint16_t getColor() const;

private:


};

#endif
