#ifndef __WIDGETS_BUTTON_H__
#define __WIDGETS_BUTTON_H__

#include "widget.h"

class Button : Widget {

    MCUFRIEND_kbv *tft;

    unsigned x;
    unsigned y;

    unsigned w;
    unsigned h;

    uint16_t color;

    char *msg;

public:

    Button(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color, char *msg, MCUFRIEND_kbv *tft);

    void    draw() const override;
    void    clear() const override;

    bool    update(unsigned touchX, unsigned touchY) override;

    unsigned height() const override;
    unsigned width() const override;

private:


};

#endif
