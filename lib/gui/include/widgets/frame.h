#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_FRAME_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_FRAME_H__

#include "widget.h"

class Frame : public BasicWidget, public DrawableWidget {

public:

    virtual Frame *add_child(BasicWidget *child) = 0;
    virtual unsigned get_children_count() const = 0;

    virtual Frame *send_front(BasicWidget *child, unsigned amt) = 0;
    virtual Frame *send_back(BasicWidget *child, unsigned amt) = 0;

    virtual void collect_dirty_widgets(RingQueueInterface<BasicWidget *> *dirty_widgets) = 0; //! this method should only be used by the app/view

    virtual void collect_overlapped_widgets(BasicWidget *dirty, BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) = 0; //! this method should only be used by the app/view
    virtual void collect_underlapped_widgets(BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) = 0; //! this method should only be used by the app/view

    // BasicWidget overrides

    virtual bool is_frame() const override { return true; }
};

#endif