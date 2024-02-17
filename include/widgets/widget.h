#ifndef __WIDGETS_WIDGET_H__
#define __WIDGETS_WIDGET_H__

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

class Widget {

public:

    virtual void draw() const;
    virtual void clear() const;

    virtual bool update(unsigned touch_x, unsigned touch_y);

    virtual unsigned height() const;
    virtual unsigned width() const;
};

#endif
