#ifndef __WIDGETS_CANVAS_H__
#define __WIDGETS_CANVAS_H__

#include "widget.h"

class Canvas : Widget {

    MCUFRIEND_kbv *tft;

    unsigned x;
    unsigned y;

    unsigned w;
    unsigned h;

    unsigned thickness;
    uint16_t color;

public:

    Canvas(unsigned x, unsigned y, unsigned w, unsigned h, MCUFRIEND_kbv *tft);

    void    draw() const override;
    void    clear() const override;

    bool    update(unsigned touch_x, unsigned touch_y) override;

    unsigned height() const override;
    unsigned width() const override;

    unsigned heightInternal() const;
    unsigned widthInternal() const;

    void    setThickness(unsigned curThickness);
    void    setColor(uint16_t curColor);

    void    clearDrawing();

    uint16_t readPixel(unsigned r, unsigned c) const;
    void    writePixel(unsigned r, unsigned c, uint16_t color);


private:


};

#endif
