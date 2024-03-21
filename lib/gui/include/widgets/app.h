#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_APP_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_APP_H__

#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

#include "vector"
#include "set"

#include "widget.h"
#include "frame.h"

class View;

class App : public BasicWidget, public DrawableWidget {

protected:

#ifdef READ_PIXEL_ENABLED
    using display_t = MCUFRIEND_kbv;
#else
    using display_t = Adafruit_GFX;
#endif

    display_t *display {nullptr};

    std::vector<View *> views;
    View *active_view {nullptr};

    RingQueue<InteractiveWidget::callback_event_t, 8> event_queue;
    RingQueue<BasicWidget *, 48> dirty_widgets;

public:

    static App *create(MCUFRIEND_kbv *displayPtr);

    App *add_view(View *child);

    App *make_active_view(View *child);
    View *get_active_view() const;

    RingQueueInterface<InteractiveWidget::callback_event_t> *get_event_queue();
    App *push_event(const InteractiveWidget::callback_event_t &event);

    App *execute_event_logic();
    App *collect_dirty_widgets();
    App *update_dirty_widgets();

    // BasicWidget overrides

    BasicWidget *get_parent() override;

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

    // Drawable overrides

    App *set_at(unsigned x, unsigned y, uint16_t color) override;
    uint16_t get_at(unsigned x, unsigned y) const override;

    App *draw_line(unsigned x0, unsigned y0, unsigned x1, unsigned y1, uint16_t color) override;

    App *draw_rect(unsigned x0, unsigned y0, unsigned w, unsigned h, uint16_t color) override;
    App *fill_rect(unsigned x0, unsigned y0, unsigned w, unsigned h, uint16_t color) override;

    App *draw_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;
    App *fill_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;

    App *draw_circle(unsigned x, unsigned y, unsigned r, uint16_t color) override;
    App *fill_circle(unsigned x, unsigned y, unsigned r, uint16_t color) override;

    App *get_text_bounds(const char *text, unsigned text_size, unsigned x, unsigned y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) override;

    App *set_font(const GFXfont *f);

    App *print(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color) override;
    App *print_opaque(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color, uint16_t bg_color) override;

    App *draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height);

protected:

    App(MCUFRIEND_kbv *display);
};

#endif
