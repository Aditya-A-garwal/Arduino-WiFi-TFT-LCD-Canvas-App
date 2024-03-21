#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_LABEL_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_LABEL_H__

#include "Arduino.h"

#include "widget.h"
#include "frame.h"

class LabelStyle {

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

    LabelStyle *set_horizontal_alignment(HorizontalAlignment new_h_align);
    HorizontalAlignment get_horizontal_alignment() const;

    LabelStyle *set_vertical_alignment(VerticalAlignment new_v_align);
    VerticalAlignment get_vertical_alignment() const;

    LabelStyle *set_text_size(unsigned new_text_size);
    unsigned get_text_size() const;

    LabelStyle *set_border_radius(unsigned new_border_radius);
    unsigned get_border_radius() const;

    LabelStyle *set_border_width(unsigned new_border_w);
    unsigned get_border_width() const;

    LabelStyle *set_border_color(uint16_t new_border_color);
    uint16_t get_border_color() const;

    LabelStyle *set_fg_color(uint16_t new_fg_color);
    uint16_t get_fg_color() const;

    LabelStyle *set_bg_color(uint16_t new_bg_color);
    uint16_t get_bg_color() const;
};

class Label : public BasicWidget {

protected:

    Frame *parent {nullptr};

    bool dirty {false};
    bool visibility_changed {false};

    bool visible {true};

    unsigned widget_x {0};
    unsigned widget_y {0};

    unsigned widget_absolute_x {0};
    unsigned widget_absolute_y {0};

    unsigned widget_w {0};
    unsigned widget_h {0};

    String message {""};

    LabelStyle style;

public:

    static Label *create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);

    Label() = delete;

    Label *set_message(const char msg_ptr[]);
    String get_message() const;

    unsigned get_message_len() const;

    Label *append_to_message(char ch);
    Label *pop_from_message();

    LabelStyle *get_style();

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

protected:

    Label(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);

    void render_text();
};

#endif