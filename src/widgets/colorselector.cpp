/**
 * @file                    colorselector.cpp
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   This file implements the methods of the `ColorSelector` class
 *
 */

#include "widgets/colorselector.h"

ColorSelector::ColorSelector(Frame *parent, unsigned x, unsigned y)
    : parent {parent}
    , widget_x {x}
    , widget_y {y}
    , widget_absolute_x {x + parent->get_absolute_x() }
    , widget_absolute_y {y + parent->get_absolute_y() }
{}

ColorSelector *ColorSelector::create(Frame *parent, unsigned x, unsigned y) {
    ColorSelector *colorselector = new (std::nothrow) ColorSelector(parent, x, y);
    if (colorselector != nullptr) {
        parent->add_child(colorselector);
    }
    return colorselector;
}

ColorSelector *ColorSelector::set_color(unsigned pos, uint16_t new_color) {
    if (pos > sizeof(colors) / sizeof(colors[0])) {
        return this;
    }

    if (new_color == colors[pos]) {
        return this;
    }

    dirty = true;
    colors[pos] = new_color;
    return this;
}

uint16_t ColorSelector::get_color(unsigned pos) const {
    if (pos > sizeof(colors) / sizeof(colors[0])) {
        return 0;
    }
    return colors[pos];
}

// BasicWidget overrides

Frame *ColorSelector::get_parent() { return parent; }

unsigned ColorSelector::get_x() const { return widget_x; }
unsigned ColorSelector::get_y() const { return widget_y; }

unsigned ColorSelector::get_absolute_x() const { return widget_absolute_x; }
unsigned ColorSelector::get_absolute_y() const { return widget_absolute_y; }

unsigned ColorSelector::get_width() const { return WIDTH; }
unsigned ColorSelector::get_height() const { return HEIGHT; }

bool ColorSelector::get_dirty() const { return dirty; }
bool ColorSelector::get_visibility_changed() const { visibility_changed; }

void ColorSelector::set_dirty() { dirty = true; }
void ColorSelector::set_visibility_changed() { visibility_changed = true; }

void ColorSelector::draw() {

    unsigned x, y;

    dirty = false;
    visibility_changed = false;

    for (unsigned c = 0; c < 3; ++c) {

        x = widget_x + (H_PAD * (c + 1)) + (PAINT_RADIUS * (2*c + 1));
        for (unsigned r = 0; r < 3; ++r) {

            y = widget_y + (V_PAD * (r + 1)) + (PAINT_RADIUS * (2*r + 1));

            parent->fill_circle(x, y, PAINT_RADIUS, colors[(3 * r) + c]);
            parent->draw_circle(x, y, PAINT_RADIUS + 3, WHITE);
        }
    }
}
void ColorSelector::clear() {

    dirty = false;
    visibility_changed = false;

    parent->fill_rect(widget_x, widget_y, WIDTH, HEIGHT, BLACK);
}

bool ColorSelector::get_intersection(unsigned x, unsigned y) const {
    return (widget_x <= x && x <= (widget_x + WIDTH))
        && (widget_y <= y && y <= (widget_y + HEIGHT));
}

bool ColorSelector::get_intersection(BasicWidget *other) const {

    unsigned x0 = other->get_absolute_x();
    unsigned y0 = other->get_absolute_y();

    unsigned x1 = x0 + other->get_width();
    unsigned y1 = y0 + other->get_height();

    if ((
        (widget_absolute_x <= x0 && x0 <= (widget_absolute_x + WIDTH))
        || (widget_absolute_x <= x1 && x1 <= (widget_absolute_x + WIDTH))
    )
    && (
        (widget_absolute_y <= y0 && y0 <= (widget_absolute_y + HEIGHT))
        || (widget_absolute_y <= y1 && y1 <= (widget_absolute_y + HEIGHT))
    )) {
        return true;
    }

    if ((
        (x0 <= widget_absolute_x && widget_absolute_x <= x1)
        || (x0 <= (widget_absolute_x + WIDTH) && (widget_absolute_x + WIDTH) <= x1)
    )
    && (
        (y0 <= widget_absolute_y && widget_absolute_y <= y1)
        || (y0 <= (widget_absolute_y + HEIGHT) && (widget_absolute_y + HEIGHT) <= y1)
    )) {
        return true;
    }

    return false;
}

bool ColorSelector::propagate_press(unsigned x, unsigned y) {

    unsigned x1, y1;

    if (!get_intersection(x, y)) {
        return false;
    }

    if (pressed || !enabled) {
        return true;
    }

    {
        unsigned cur_epoch = millis();
        if ((cur_epoch - last_press_epoch) < DEBOUNCE_THRESH) {
            return true;
        }

        last_press_epoch = cur_epoch;
    }

    pressed = true;

    x -= widget_x;
    y -= widget_y;

    for (unsigned c = 0; c < 3; ++c) {

        x1 = (H_PAD * (c + 1)) + (PAINT_RADIUS * (2*c + 1));
        x1 = (x1 > x) ? (x1 - x) : (x - x1);
        x1 *= x1;
        for (unsigned r = 0; r < 3; ++r) {

            y1 = (V_PAD * (r + 1)) + (PAINT_RADIUS * (2*r + 1));
            y1 = (y1 > y) ? (y1 - y) : (y - y1);
            y1 *= y1;

            if ((x1 + y1) > (PAINT_RADIUS * PAINT_RADIUS)) {
                continue;;
            }

            if (event_queue != nullptr && on_press != nullptr) {
                event_queue->push({on_press, (unsigned *)&colors[(r * 3) + c]});
            }
            break;
        }
    }

    return true;
}
bool ColorSelector::propagate_release(unsigned x, unsigned y) {

    unsigned x1, y1;

    if (!get_intersection(x, y)) {
        return false;
    }

    if (!pressed) {
        return true;
    }

    pressed = false;

    x -= widget_x;
    y -= widget_y;

    for (unsigned c = 0; c < 3; ++c) {

        x1 = (H_PAD * (c + 1)) + (PAINT_RADIUS * (2*c + 1));
        x1 = (x1 > x) ? (x1 - x) : (x - x1);
        x1 *= x1;
        for (unsigned r = 0; r < 3; ++r) {

            y1 = (V_PAD * (r + 1)) + (PAINT_RADIUS * (2*r + 1));
            y1 = (y1 > y) ? (y1 - y) : (y - y1);
            y1 *= y1;

            if ((x1 + y1) > (PAINT_RADIUS * PAINT_RADIUS)) {
                continue;;
            }

            if (event_queue != nullptr && on_release != nullptr) {
                event_queue->push({on_release, (unsigned *)&colors[(r * 3) + c]});
            }
            break;
        }
    }

    return true;
}

bool ColorSelector::get_visibility() const { return visible; }
void ColorSelector::set_visibility(bool new_visibility) {

    if (visible == new_visibility) {
        return;
    }

    dirty = true;
    visibility_changed = true;

    visible = new_visibility;
}

// InteractiveWidget overrides

ColorSelector *ColorSelector::set_onpress(callback_t callback) {
    on_press = callback;
    return this;
}

ColorSelector *ColorSelector::reset_onpress() {
    on_press = nullptr;
    return this;
}

ColorSelector *ColorSelector::set_onrelease(callback_t callback) {
    on_release = callback;
    return this;
}

ColorSelector *ColorSelector::reset_onrelease() {
    on_release = nullptr;
    return this;
}

ColorSelector *ColorSelector::set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) {
    event_queue = new_event_queue;
    return this;
}

ColorSelector *ColorSelector::reset_event_queue() {
    event_queue = nullptr;
    return this;
}

ColorSelector *ColorSelector::set_args(unsigned *new_args) {
    args = new_args;
    return this;
}

ColorSelector *ColorSelector::reset_args() {
    args = nullptr;
    return this;
}

unsigned *ColorSelector::get_args() { return args; }

bool ColorSelector::is_pressed() const { return pressed; }

ColorSelector *ColorSelector::set_interactable(bool new_state) {
    if (new_state == enabled) {
        return this;
    }

    dirty = true;
    new_state = new_state;
    return this;
}

bool ColorSelector::get_interactable() const { return enabled; }
