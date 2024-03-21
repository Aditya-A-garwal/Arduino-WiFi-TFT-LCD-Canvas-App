#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_COLOR_SELECTOR_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_COLOR_SELECTOR_H__

#include "widget.h"
#include "frame.h"

class ColorSelector : public BasicWidget, public InteractiveWidget {

protected:

    constexpr static unsigned PAINT_RADIUS = 12;
    constexpr static unsigned H_PAD = 12;
    constexpr static unsigned V_PAD = 12;

    constexpr static unsigned WIDTH = (H_PAD * (3 + 1)) + (PAINT_RADIUS * (3 * 2));
    constexpr static unsigned HEIGHT = (V_PAD * (3 + 1)) + (PAINT_RADIUS * (3 * 2));

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

    uint16_t colors[9] {};

    callback_t on_press {nullptr};
    callback_t on_release {nullptr};

    RingQueueInterface<callback_event_t> *event_queue {nullptr};

    unsigned *args {nullptr};

public:

    ColorSelector() = delete;

    static ColorSelector *create(Frame *parent, unsigned x, unsigned y);

    ColorSelector *set_color(unsigned pos, uint16_t new_color);
    uint16_t get_color(unsigned pos) const;

    // BasicWidget overrides

    Frame *get_parent() override;

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

    ColorSelector *set_onpress(callback_t callback) override;
    ColorSelector *reset_onpress() override;

    ColorSelector *set_onrelease(callback_t callback) override;
    ColorSelector *reset_onrelease() override;

    ColorSelector *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) override;
    ColorSelector *reset_event_queue() override;

    ColorSelector *set_args(unsigned *new_args) override;
    ColorSelector *reset_args() override;
    unsigned *get_args() override;

    bool is_pressed() const override;

    ColorSelector *set_interactable(bool new_state) override;
    bool get_interactable() const override;

protected:

    ColorSelector(Frame *parent, unsigned x, unsigned y);
};

#endif
