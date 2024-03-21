#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_WIDGET_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_WIDGET_H__

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "constants.h"

#include "functional"
#include "queue"

#include "Fonts/PlusJakartaSans6pt7b.h" // size 1
#include "Fonts/PlusJakartaSans9pt7b.h" // size 2
#include "Fonts/PlusJakartaSans15pt7b.h" // size 3
#include "Fonts/PlusJakartaSans21pt7b.h" // size 4

#define FUNCTION_PTR_CALLBACK
#define READ_PIXEL_ENABLED

#define font_1 (&PlusJakartaSans_SemiBold6pt7b)
#define font_2 (&PlusJakartaSans_SemiBold9pt7b)
#define font_3 (&PlusJakartaSans_SemiBold15pt7b)
#define font_4 (&PlusJakartaSans_SemiBold21pt7b)

template <typename T>
class RingQueueInterface {

public:

    virtual void push(const T &e) = 0;
    virtual const T &front() = 0;
    virtual void pop() = 0;
    virtual unsigned get_size() const = 0;
};

template <typename T, unsigned CAPACITY>
class RingQueue : public RingQueueInterface<T> {

protected:

    T arr[CAPACITY];
    unsigned head = 0;
    unsigned tail = 0;
    unsigned size = 0;

public:

    void push(const T &e) override {
        arr[head] = e;
        (head += 1) %= CAPACITY;
        ++size;
    }

    const T &front() override {
        return arr[tail];
    }

    void pop() override {
        (tail += 1) %= CAPACITY;
        --size;
    }

    unsigned get_size() const override {
        return size;
    }
};

class BasicWidget {

public:

    virtual BasicWidget *get_parent() = 0;

    virtual unsigned get_x() const = 0;
    virtual unsigned get_y() const = 0;

    virtual unsigned get_absolute_x() const = 0;
    virtual unsigned get_absolute_y() const = 0;

    virtual unsigned get_width() const = 0;
    virtual unsigned get_height() const = 0;

    virtual bool get_dirty() const = 0;
    virtual bool get_visibility_changed() const = 0;

    virtual void set_dirty() = 0;
    virtual void set_visibility_changed() = 0;

    virtual void draw() = 0;
    virtual void clear() = 0;

    virtual bool get_intersection(unsigned x, unsigned y) const = 0;
    virtual bool get_intersection(BasicWidget *other) const = 0;

    virtual bool propagate_press(unsigned x, unsigned y) = 0;
    virtual bool propagate_release(unsigned x, unsigned y) = 0;

    virtual bool get_visibility() const = 0;
    virtual void set_visibility(bool new_visibility) = 0;

    virtual bool is_frame() const { return false; }
};

class InteractiveWidget {

public:

#ifdef FUNCTION_PTR_CALLBACK
    using callback_t = void (*)(unsigned *);
#else
    using callback_t = std::function<void (unsigned *)>;
#endif

    struct callback_event_t {
        callback_t cb;
        unsigned *args;
    };

    virtual InteractiveWidget *set_onpress(callback_t cb) = 0;
    virtual InteractiveWidget *reset_onpress() = 0;

    virtual InteractiveWidget *set_onrelease(callback_t cb) = 0;
    virtual InteractiveWidget *reset_onrelease() = 0;

    virtual InteractiveWidget *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) = 0;
    virtual InteractiveWidget *reset_event_queue() = 0;

    virtual InteractiveWidget *set_args(unsigned *new_args) = 0;
    virtual InteractiveWidget *reset_args() = 0;
    virtual unsigned *get_args() = 0;

    virtual bool is_pressed() const = 0;

    virtual InteractiveWidget *set_interactable(bool new_state) = 0;
    virtual bool get_interactable() const = 0;
};

class DrawableWidget {

public:

    virtual DrawableWidget *set_at(unsigned x, unsigned y, uint16_t color) = 0;
    virtual uint16_t get_at(unsigned x, unsigned y) const = 0;

    virtual DrawableWidget *draw_line(unsigned x0, unsigned y0, unsigned x1, unsigned y1, uint16_t color) = 0;

    virtual DrawableWidget *draw_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) = 0;
    virtual DrawableWidget *fill_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) = 0;

    virtual DrawableWidget *draw_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) = 0;
    virtual DrawableWidget *fill_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) = 0;

    virtual DrawableWidget *draw_circle(unsigned x, unsigned y, unsigned r, uint16_t color) = 0;
    virtual DrawableWidget *fill_circle(unsigned x, unsigned y, unsigned r, uint16_t color) = 0;

    virtual DrawableWidget *get_text_bounds(const char *text, unsigned text_size, unsigned x, unsigned y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) = 0;

    virtual DrawableWidget *set_font(const GFXfont *f) = 0;

    virtual DrawableWidget *print(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color) = 0;
    virtual DrawableWidget *print_opaque(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color, uint16_t bg_color) = 0;

    virtual DrawableWidget *draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height) = 0;
};

#endif
