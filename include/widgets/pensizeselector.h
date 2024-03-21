#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_PEN_SIZE_SELECTOR_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_PEN_SIZE_SELECTOR_H__

#include "widget.h"
#include "frame.h"

class PenSizeSelector : public BasicWidget, public InteractiveWidget {

protected:

    constexpr static unsigned H_PAD = 17;

    constexpr static unsigned WIDTH = 8 * H_PAD;
    constexpr static unsigned HEIGHT = 29;

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

    uint16_t color {};
    uint16_t sizes[4] {};

    unsigned active_size {0};

    callback_t on_press {nullptr};
    callback_t on_release {nullptr};

    RingQueueInterface<callback_event_t> *event_queue {nullptr};

    unsigned *args {nullptr};

public:

    PenSizeSelector() = delete;

    static PenSizeSelector *create(Frame *parent, unsigned x, unsigned y);

    PenSizeSelector *set_size(unsigned pos, uint16_t new_size);
    uint16_t get_size(unsigned pos) const;

    PenSizeSelector *set_active_size(unsigned pos);
    uint16_t get_active_size() const;

    PenSizeSelector *set_color(uint16_t new_color);
    uint16_t get_color() const;

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

    PenSizeSelector *set_onpress(callback_t callback)  override;
    PenSizeSelector *reset_onpress()  override;

    PenSizeSelector *set_onrelease(callback_t callback)  override;
    PenSizeSelector *reset_onrelease()  override;

    PenSizeSelector *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue)  override;
    PenSizeSelector *reset_event_queue()  override;

    PenSizeSelector *set_args(unsigned *new_args)  override;
    PenSizeSelector *reset_args()  override;
    unsigned *get_args()  override;

    bool is_pressed() const override;

    PenSizeSelector *set_interactable(bool new_state) override;
    bool get_interactable() const override;

protected:

    PenSizeSelector(Frame *parent, unsigned x, unsigned y);
};

#endif
