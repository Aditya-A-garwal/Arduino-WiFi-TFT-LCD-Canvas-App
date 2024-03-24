/**
 * @file                    bitmap.cpp
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   This file implements the methods of the `Bitmap` class
 *
 */

#include "widgets/bitmap.h"

Bitmap::Bitmap(Frame *parent, const uint16_t *data, unsigned x, unsigned y, unsigned width, unsigned height)
    : parent {parent}
    , data {data}
    , widget_x {x}
    , widget_y {y}
    , widget_absolute_x { x + parent->get_absolute_x() }
    , widget_absolute_y { y + parent->get_absolute_x() }
    , widget_w {width}
    , widget_h {height}
{}

Bitmap *Bitmap::create(Frame *parent, const uint16_t *data, unsigned x, unsigned y, unsigned width, unsigned height) {
    Bitmap *bitmap = new (std::nothrow) Bitmap(parent, data, x, y, width, height);
    if (bitmap != nullptr) {
        parent->add_child(bitmap);
    }
    return bitmap;
}

// BasicWidget overrides

Frame *Bitmap::get_parent() { return parent; }

unsigned Bitmap::get_x() const { return widget_x; }
unsigned Bitmap::get_y() const { return widget_y; }

unsigned Bitmap::get_absolute_x() const { return widget_absolute_x; }
unsigned Bitmap::get_absolute_y() const { return widget_absolute_y; }

unsigned Bitmap::get_width() const { return widget_w; }
unsigned Bitmap::get_height() const { return widget_h; }

bool Bitmap::get_dirty() const { return dirty; }
bool Bitmap::get_visibility_changed() const { return visibility_changed; }

void Bitmap::set_dirty() { dirty = true; }
void Bitmap::set_visibility_changed() { visibility_changed = true; }

void Bitmap::draw() {

    dirty = false;
    visibility_changed = false;

    parent->draw_rgb_bitmap(widget_x, widget_y, data, widget_w, widget_h);
}

void Bitmap::clear() {

    dirty = false;
    visibility_changed = false;

    parent->fill_rect(widget_x, widget_y, widget_w, widget_h, BLACK);
}

bool Bitmap::get_intersection(unsigned int x, unsigned int y) const {
    return (widget_x <= x && x <= (widget_x + widget_w))
           && (widget_y <= y && y <= (widget_y + widget_h));
}

bool Bitmap::get_intersection(BasicWidget *other) const {

    unsigned x0 = other->get_absolute_x();
    unsigned y0 = other->get_absolute_y();

    unsigned x1 = x0 + other->get_width();
    unsigned y1 = y0 + other->get_height();

    if ((
        (widget_absolute_x <= x0 && x0 <= (widget_absolute_x + widget_w))
        || (widget_absolute_x <= x1 && x1 <= (widget_absolute_x + widget_w))
    )
    && (
        (widget_absolute_y <= y0 && y0 <= (widget_absolute_y + widget_h))
        || (widget_absolute_y <= y1 && y1 <= (widget_absolute_y + widget_h))
    )) {
        return true;
    }

    if ((
        (x0 <= widget_absolute_x && widget_absolute_x <= x1)
        || (x0 <= (widget_absolute_x + widget_w) && (widget_absolute_x + widget_w) <= x1)
    )
    && (
        (y0 <= widget_absolute_y && widget_absolute_y <= y1)
        || (y0 <= (widget_absolute_y + widget_h) && (widget_absolute_y + widget_h) <= y1)
    )) {
        return true;
    }

    return false;
}

bool Bitmap::propagate_press(unsigned x, unsigned y) {

    if (!get_intersection(x, y)) {
        return false;
    }
    if (pressed || !enabled) {
        return true;
    }

    unsigned cur_epoch = millis();
    if ((cur_epoch - last_press_epoch) < DEBOUNCE_THRESH) {
        return true;
    }
    last_press_epoch = cur_epoch;

    dirty = true;
    pressed = true;

    if (event_queue != nullptr && on_press != nullptr) {
        event_queue->push({on_press, args});
    }

    return true;
}

bool Bitmap::propagate_release(unsigned x, unsigned y) {

    if (!get_intersection(x, y)) {
        return false;
    }
    if (!pressed) {
        return true;
    }

    dirty = true;
    pressed = false;

    if (event_queue != nullptr && on_release != nullptr) {
        event_queue->push({on_release, args});
    }

    return true;
}

bool Bitmap::get_visibility() const { return visible; }

void Bitmap::set_visibility(bool new_visibility) {

    if (visible == new_visibility) {
        return;
    }

    dirty = true;
    visibility_changed = true;

    visible = new_visibility;
}

// InteractiveWidget overrides

Bitmap *Bitmap::set_onpress(callback_t callback) {
    on_press = callback;
    return this;
}

Bitmap *Bitmap::reset_onpress() {
    on_press = nullptr;
    return this;
}

Bitmap *Bitmap::set_onrelease(callback_t callback) {
    on_release = callback;
    return this;
}

Bitmap *Bitmap::reset_onrelease() {
    on_release = nullptr;
    return this;
}

Bitmap *Bitmap::set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) {
    event_queue = new_event_queue;
    return this;
}

Bitmap *Bitmap::reset_event_queue() {
    event_queue = nullptr;
    return this;
}

Bitmap *Bitmap::set_args(unsigned *new_args) {
    args = new_args;
    return this;
}

Bitmap *Bitmap::reset_args() {
    args = nullptr;
    return this;
}

unsigned *Bitmap::get_args() { return args; }

bool Bitmap::is_pressed() const { return pressed; }

Bitmap *Bitmap::set_interactable(bool new_state) {
    if (new_state == enabled) {
        return this;
    }

    dirty = true;
    new_state = new_state;
    return this;
}

bool Bitmap::get_interactable() const { return enabled; }
