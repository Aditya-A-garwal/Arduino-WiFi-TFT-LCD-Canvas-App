#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_WINDOW_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_WINDOW_H__

#include "widget.h"
#include "frame.h"

#include "vector"
#include "algorithm"

class WindowStyle {

protected:

    constexpr static uint16_t DEFAULT_BG_COLOR = BLACK;
    constexpr static uint16_t DEFAULT_BORDER_COLOR = WHITE;

    constexpr static unsigned DEFAULT_BORDER_WIDTH = 1;
    constexpr static unsigned DEFAULT_BORDER_RADIUS = 0;

public:

    uint16_t bg_color {DEFAULT_BG_COLOR};
    uint16_t border_color {DEFAULT_BORDER_COLOR};

    unsigned border_w {DEFAULT_BORDER_WIDTH};
    unsigned border_radius {DEFAULT_BORDER_RADIUS};

    WindowStyle *set_bg_color(uint16_t new_color);
    uint16_t get_bg_color() const;

    WindowStyle *set_border_color(uint16_t new_color);
    uint16_t get_border_color() const;

    WindowStyle *set_border_width(unsigned new_border_width);
    unsigned get_border_width() const;

    WindowStyle *set_border_radius(unsigned new_border_radius);
    unsigned get_border_radius() const;
};

class Window : public Frame {

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

    std::vector<BasicWidget *> children;

    WindowStyle style;

public:

    Window() = delete;

    static Window *create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);

    WindowStyle *get_style();

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

    // Frame overrides

    Window *set_at(unsigned x, unsigned y, uint16_t color) override;
    uint16_t get_at(unsigned x, unsigned y) const override;

    Window *draw_line(unsigned x0, unsigned y0, unsigned x1, unsigned y1, uint16_t color) override;

    Window *draw_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) override;
    Window *fill_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) override;

    Window *draw_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;
    Window *fill_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;

    Window *draw_circle(unsigned x, unsigned y, unsigned r, uint16_t color) override;
    Window *fill_circle(unsigned x, unsigned y, unsigned r, uint16_t codlor) override;

    Window *get_text_bounds(const char *text, unsigned text_size, unsigned x, unsigned y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) override;

    Window *set_font(const GFXfont *f);

    Window *print(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color) override;
    Window *print_opaque(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color, uint16_t bg_color) override;

    Window *draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height);

    Window *add_child(BasicWidget *child) override;
    unsigned get_children_count() const override;

    Window *send_front(BasicWidget *child, unsigned amt) override;
    Window *send_back(BasicWidget *child, unsigned amt) override;

    void collect_dirty_widgets(RingQueueInterface<BasicWidget *> *dirty_widgets) override;
    void collect_overlapped_widgets(BasicWidget *dirty, BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) override;
    // void collect_intersecting_widgets(BasicWidget *dirty, BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) override;
    void collect_underlapped_widgets(BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) override;


protected:

    Window(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);
};

#endif
