#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_VIEW_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_VIEW_H__

#include "vector"

#include "frame.h"
#include "app.h"

class View : public Frame {

    friend class App;

protected:

    App *app {nullptr};
    bool dirty {false};

    std::vector<BasicWidget *> children;

public:

    View() = delete;

    static View *create(App *parent);

    // BasicWidget overrides

    App *get_parent() override;

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

    View *set_at(unsigned x, unsigned y, uint16_t color) override;
    uint16_t get_at(unsigned x, unsigned y) const override;

    View *draw_line(unsigned x0, unsigned y0, unsigned x1, unsigned y1, uint16_t color) override;

    View *draw_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) override;
    View *fill_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) override;

    View *draw_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;
    View *fill_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;

    View *draw_circle(unsigned x, unsigned y, unsigned r, uint16_t color) override;
    View *fill_circle(unsigned x, unsigned y, unsigned r, uint16_t color) override;

    View *get_text_bounds(const char *text, unsigned text_size, unsigned x, unsigned y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) override;

    View *print(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color) override;
    View *print_opaque(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color, uint16_t bg_color) override;

    View *set_font(const GFXfont *f);

    View *draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height);

    View *add_child(BasicWidget *child) override;
    unsigned get_children_count() const override;

    View *send_front(BasicWidget *child, unsigned amt) override;
    View *send_back(BasicWidget *child, unsigned amt) override;

    void collect_dirty_widgets(RingQueueInterface<BasicWidget *> *dirty_widgets) override;
    void collect_overlapped_widgets(BasicWidget *dirty, BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) override;
    // void collect_intersecting_widgets(BasicWidget *dirty, BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) override;
    void collect_underlapped_widgets(BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) override;

protected:

    View(App *parent);
};

#endif
