#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_BUTTON_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_BUTTON_H__

#include "widget.h"
#include "frame.h"

class ButtonStyle {

protected:

    constexpr static unsigned DEFAULT_TEXT_SIZE = 3;

    constexpr static unsigned DEFAULT_BORDER_RADIUS = 4;
    constexpr static unsigned DEFAULT_BORDER_W = 2;
    constexpr static uint16_t DEFAULT_BORDER_COLOR = blend_color(WHITE, GRAY, 100);

    constexpr static uint16_t DEFAULT_BG_COLOR = BLACK;
    constexpr static uint16_t DEFAULT_FG_COLOR = WHITE;

public:

    enum HorizontalAlignment {
        CENTER_ALIGN = 0,
        LEFT_ALIGN = 1,
        RIGHT_ALIGN = 2
    };

    enum VerticalAlignment {
        MIDDLE_ALIGN = 0,
        TOP_ALIGN = 1,
        BOTTOM_ALIGN = 2
    };

    HorizontalAlignment h_align {HorizontalAlignment::CENTER_ALIGN};
    VerticalAlignment v_align {VerticalAlignment::MIDDLE_ALIGN};
    unsigned text_size {DEFAULT_TEXT_SIZE};

    unsigned border_radius {DEFAULT_BORDER_RADIUS};
    unsigned border_w {DEFAULT_BORDER_W};
    uint16_t border_color {DEFAULT_BORDER_COLOR};

    uint16_t bg_color {DEFAULT_BG_COLOR};
    uint16_t fg_color {DEFAULT_FG_COLOR};

    uint16_t pressed_bg_color {blend_color(DEFAULT_BG_COLOR, BLACK, 100)};
    uint16_t pressed_fg_color {blend_color(DEFAULT_FG_COLOR, BLACK, 100)};
    uint16_t pressed_border_color {blend_color(DEFAULT_BORDER_COLOR, BLACK, 100)};

    ButtonStyle *set_horizontal_alignment(HorizontalAlignment new_h_align);
    HorizontalAlignment get_horizontal_alignment() const;

    ButtonStyle *set_vertical_alignment(VerticalAlignment new_v_align);
    VerticalAlignment get_vertical_alignment() const;

    ButtonStyle *set_text_size(unsigned new_text_size);
    unsigned get_text_size() const;

    ButtonStyle *set_border_radius(unsigned new_border_radius);
    unsigned get_border_radius() const;

    ButtonStyle *set_border_width(unsigned new_border_w);
    unsigned get_border_width() const;

    ButtonStyle *set_border_color(uint16_t new_border_color);
    uint16_t get_border_color() const;

    ButtonStyle *set_fg_color(uint16_t new_fg_color);
    uint16_t get_fg_color() const;

    ButtonStyle *set_bg_color(uint16_t new_bg_color);
    uint16_t get_bg_color() const;

    ButtonStyle *set_pressed_bg_color(uint16_t new_pressed_bg_color);
    uint16_t get_pressed_bg_color() const;

    ButtonStyle *set_pressed_fg_color(uint16_t new_pressed_fg_color);
    uint16_t get_pressed_fg_color() const;

    ButtonStyle *set_pressed_border_color(uint16_t new_pressed_border_color);
    uint16_t get_pressed_border_color() const;
};

class Button : public BasicWidget, public InteractiveWidget {

protected:

    constexpr static unsigned DEBOUNCE_THRESH = 200;

    constexpr static unsigned DEFAULT_WIDTH = 64;
    constexpr static unsigned DEFAULT_HEIGHT = 32;

    // constexpr static unsigned DEFAULT_text_size = 2;
    constexpr static uint16_t DEFAULT_BG_COLOR = BLACK;
    constexpr static uint16_t DEFAULT_FG_COLOR = WHITE;

    Frame *parent {nullptr};

    bool dirty {false};
    bool visibility_changed {false};

    bool pressed {false};
    bool visible {true};
    bool enabled {true};

    unsigned widget_x {0};
    unsigned widget_y {0};

    unsigned widget_absolute_x {0};
    unsigned widget_absolute_y {0};

    unsigned widget_w {0};
    unsigned widget_h {0};

    String message;

    callback_t on_press {nullptr};
    callback_t on_release {nullptr};

    unsigned *args {nullptr};

    RingQueueInterface<callback_event_t> *event_queue;

    ButtonStyle style;

    unsigned last_press_epoch {0};

public:

    Button() = delete;

    static Button *create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);
    static Button *create(Frame *parent, unsigned x, unsigned y);

    Button *set_message(const char *messagePtr);
    String get_message() const;

    ButtonStyle *get_style();

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

    Button *set_onpress(callback_t callback) override;
    Button *reset_onpress() override;

    Button *set_onrelease(callback_t callback) override;
    Button *reset_onrelease() override;

    Button *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) override;
    Button *reset_event_queue() override;

    Button *set_args(unsigned *new_args) override;
    Button *reset_args() override;
    unsigned *get_args() override;

    bool is_pressed() const override;

    Button *set_interactable(bool new_state) override;
    bool get_interactable() const override;

protected:

    Button(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);

    void render_text();
};

#endif
