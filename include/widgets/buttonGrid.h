#ifndef __WIDGETS_BUTTON_GRID_H__
#define __WIDGETS_BUTTON_GRID_H__

#include "constants.h"
#include "widget.h"

class ButtonGrid : Widget {

    static constexpr char *msg = "SLOT"" ";

    static constexpr unsigned CLOSE_X = 3;
    static constexpr unsigned CLOSE_Y = 3;
    static constexpr unsigned CLOSE_W = 13;

    static constexpr unsigned HPAD = 23;
    static constexpr unsigned VPAD = 8;

    static constexpr uint16_t BUTTON_C = WHITE;

    MCUFRIEND_kbv *tft;

    unsigned x;
    unsigned y;

    unsigned w;
    unsigned h;

    unsigned cc;
    unsigned rc;

    unsigned button_w;
    unsigned button_h;

    unsigned slot;

public:

    ButtonGrid(unsigned x, unsigned y, unsigned w, unsigned h, unsigned cc, unsigned rc, unsigned button_w, unsigned button_h, MCUFRIEND_kbv *tft);

    void    draw() const override;
    void    clear() const override;

    bool    update(unsigned touch_x, unsigned touch_y) override;

    unsigned height() const override;
    unsigned width() const override;

    unsigned getSlot() const;

private:


};

#endif
