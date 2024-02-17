#ifndef __WIDGETS_THICKNESS_SELECTOR_H__
#define __WIDGETS_THICKNESS_SELECTOR_H__

#include "widget.h"

class ThicknessSelector : Widget {

    static constexpr unsigned RADII[4] = {3, 5, 7, 9};
    static constexpr unsigned PAD = 35;

    static constexpr unsigned HEIGHT = 0;
    static constexpr unsigned WIDTH = 0;

    MCUFRIEND_kbv *tft;

    unsigned x;
    unsigned y;

    uint16_t color;
    unsigned curSelected;

public:

    ThicknessSelector(unsigned x, unsigned y, MCUFRIEND_kbv *tft);

    void    setColor(uint16_t clr);

    void    draw() const override;
    void    clear() const override;

    bool    update(unsigned touch_x, unsigned touch_y) override;

    unsigned height() const override;
    unsigned width() const override;

    uint16_t getThickness() const;

private:


};


#endif
