#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_BITMAP_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_BITMAP_H__

#include "widget.h"
#include "frame.h"

class Bitmap : public BasicWidget, public InteractiveWidget {

protected:

    constexpr static unsigned DEBOUNCE_THRESH = 150;

    Frame *parent {nullptr};

    bool dirty {false};
    bool visibility_changed {false};

    bool visible {true};
    bool pressed {false};
    bool enabled {true};

    unsigned widget_x {0};
    unsigned widget_y {0};

    unsigned widget_absolute_x {0};
    unsigned widget_absolute_y {0};

    unsigned widget_w {0};
    unsigned widget_h {0};

    unsigned last_press_epoch {0};

    const uint16_t *data;

    callback_t on_press {nullptr};
    callback_t on_release {nullptr};

    unsigned *args;

    RingQueueInterface<callback_event_t> *event_queue;

public:

    static Bitmap *create(Frame *parent, const uint16_t *data, unsigned x, unsigned y,  unsigned width, unsigned height);

    // BasicWidget overrides

    Frame *get_parent();

    unsigned get_x() const override;
    unsigned get_y() const override;

    unsigned get_absolute_x() const override;
    unsigned get_absolute_y() const override;

    unsigned get_width() const override;
    unsigned get_height() const override;

    bool get_dirty() const override;
    bool get_visibility_changed() const override;

    void set_dirty() override;
    void set_visibility_changed() override;

    void draw() override;
    void clear() override;

    bool get_intersection(unsigned x, unsigned y) const override;
    bool get_intersection(BasicWidget *other) const override;

    bool propagate_press(unsigned x, unsigned y) override;
    bool propagate_release(unsigned x, unsigned y) override;

    bool get_visibility() const override;
    void set_visibility(bool new_visibility) override;

    // InteractiveWidget overrides

    Bitmap *set_onpress(callback_t callback) override;
    Bitmap *reset_onpress() override;

    Bitmap *set_onrelease(callback_t callback) override;
    Bitmap *reset_onrelease() override;

    Bitmap *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) override;
    Bitmap *reset_event_queue() override;

    Bitmap *set_args(unsigned *new_args) override;
    Bitmap *reset_args() override;
    unsigned *get_args() override;

    bool is_pressed() const override;

    Bitmap *set_interactable(bool new_state) override;
    bool get_interactable() const override;

protected:

    Bitmap(Frame *parent, const uint16_t *data, unsigned x, unsigned y, unsigned width, unsigned height);
};

#endif