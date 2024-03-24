/**
 * @file                    bitmap.cpp
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   This file implements the methods of the `Bitmap` class
 *
 */

#include "widgets/button.h"

Button::Button(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height)
    : parent {parent}
    , widget_x {x}
    , widget_y {y}
    , widget_absolute_x { x + parent->get_absolute_x() }
    , widget_absolute_y { y + parent->get_absolute_y() }
    , widget_w {width}
    , widget_h {height}
{}

Button *Button::create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height) {
    Button *button = new (std::nothrow) Button(parent, x, y, width, height);
    if (button != nullptr) {
        parent->add_child(button);
    }
    return button;
}

Button *Button::create(Frame *parent, unsigned x, unsigned y) {
    Button *button = new (std::nothrow) Button(parent, x, y, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    if (button != nullptr) {
        parent->add_child(button);
    }
    return button;
}

Button *Button::set_message(const char *messagePtr) {
    dirty = true;
    message = messagePtr;
    return this;
}

String Button::get_message() const { return message; }

ButtonStyle *Button::get_style() {
    dirty = true;
    return &style;
}

// BasicWidget overrides

Frame *Button::get_parent() { return parent; }

unsigned Button::get_x() const { return widget_x; }
unsigned Button::get_y() const { return widget_y; }

unsigned Button::get_absolute_x() const { return widget_absolute_x; }
unsigned Button::get_absolute_y() const { return widget_absolute_y; }

unsigned Button::get_width() const { return widget_w; }
unsigned Button::get_height() const { return widget_h; }

bool Button::get_dirty() const { return dirty; }
bool Button::get_visibility_changed() const { return visibility_changed; }

void Button::set_dirty() { dirty = true; }
void Button::set_visibility_changed() { visibility_changed = true; }

void Button::draw() {

    dirty = false;
    visibility_changed = false;

    if (style.border_radius != 0) {
        if (style.border_w != 0) {
            parent->fill_round_rect(
                    widget_x,
                    widget_y,
                    widget_w,
                    widget_h,
                    style.border_radius,
                    (pressed)
                    ? style.pressed_border_color
                    : style.border_color
            );
        }
        parent->fill_round_rect(
                widget_x + style.border_w,
                widget_y + style.border_w,
                widget_w - 2*style.border_w,
                widget_h - 2*style.border_w,
                style.border_radius,
                (pressed)
                ? style.pressed_bg_color
                : style.bg_color
        );
    }
    else {
        if (style.border_w != 0) {
            parent->fill_rect(
                    widget_x,
                    widget_y,
                    widget_w,
                    widget_h,
                    (pressed)
                    ? style.pressed_border_color
                    : style.border_color
            );
        }
        parent->fill_rect(
                widget_x + style.border_w,
                widget_y + style.border_w,
                widget_w - 2*style.border_w,
                widget_h - 2*style.border_w,
                (pressed)
                ? style.pressed_bg_color
                : style.bg_color
        );
    }

    render_text();
}

void Button::clear() {

    dirty = false;
    visibility_changed = false;

    parent->fill_rect(widget_x, widget_y, widget_w, widget_h, BLACK);
}

bool Button::get_intersection(unsigned int x, unsigned int y) const {
    return (widget_x <= x && x <= (widget_x + widget_w))
           && (widget_y <= y && y <= (widget_y + widget_h));
}

bool Button::get_intersection(BasicWidget *other) const {

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

bool Button::propagate_press(unsigned x, unsigned y) {

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

    dirty = true;

    pressed = true;
    if (event_queue != nullptr && on_press != nullptr) {
        event_queue->push({on_press, args});
    }

    return true;
}

bool Button::propagate_release(unsigned x, unsigned y) {

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

bool Button::get_visibility() const { return visible; }

void Button::set_visibility(bool new_visibility) {

    if (visible == new_visibility) {
        return;
    }

    dirty = true;
    visibility_changed = true;

    visible = new_visibility;
}

// void Button::render_text() {

//     int16_t x1, y1;
//     uint16_t w1, h1;

//     parent->get_text_bounds(
//             message.c_str(),
//             style.text_size,
//             0,
//             0,
//             &x1,
//             &y1,
//             &w1,
//             &h1
//     );
//     parent->print(
//             message.c_str(),
//             (style.h_align == ButtonStyle::HorizontalAlignment::CENTER_ALIGN)
//             ? (2*widget_x + widget_w - w1) / 2
//             : (style.h_align == ButtonStyle::HorizontalAlignment::LEFT_ALIGN)
//             ? widget_x + style.border_w
//             : widget_x + widget_w - style.border_w - w1,
//             (style.v_align == ButtonStyle::VerticalAlignment::MIDDLE_ALIGN)
//             ? (2*widget_y + widget_h - h1) / 2
//             : (style.v_align == ButtonStyle::VerticalAlignment::TOP_ALIGN)
//             ? widget_y + style.border_w
//             : widget_y + widget_h - style.border_w - h1,
//             style.text_size,
//             (pressed)
//             ? style.pressed_fg_color
//             : style.fg_color
//     );
// }

void Button::render_text() {

    int16_t x1, y1;
    uint16_t w1, h1;

    switch (style.text_size) {
        case 1:
            parent->set_font(font_1);
            break;
        case 2:
            parent->set_font(font_2);
            break;
        case 3:
            parent->set_font(font_3);
            break;
        default:
            parent->set_font(font_4);
            break;
    }

    parent->get_text_bounds(message.c_str(), 1, 0, 0, &x1, &y1, &w1, &h1);

    parent->print(
            message.c_str(),
            (style.h_align == ButtonStyle::HorizontalAlignment::CENTER_ALIGN)
            ? (2*widget_x + widget_w - w1) / 2
            : (style.h_align == ButtonStyle::HorizontalAlignment::LEFT_ALIGN)
            ? widget_x + style.border_w
            : widget_x + widget_w - style.border_w - w1,
            ((style.v_align == ButtonStyle::VerticalAlignment::MIDDLE_ALIGN)
            ? (2*widget_y + widget_h - h1) / 2
            : (style.v_align == ButtonStyle::VerticalAlignment::TOP_ALIGN)
            ? widget_y + style.border_w
            : widget_y + widget_h - style.border_w - h1) + h1,
            1, //style.text_size,
            (pressed)
            ? style.pressed_fg_color
            : style.fg_color
    );
}

// InteractiveWidget overrides

Button *Button::set_onpress(callback_t callback) {
    on_press = callback;
    return this;
}

Button *Button::reset_onpress() {
    on_press = nullptr;
    return this;
}

Button *Button::set_onrelease(callback_t callback) {
    on_release = callback;
    return this;
}

Button *Button::reset_onrelease() {
    on_release = nullptr;
    return this;
}

Button *Button::set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) {
    event_queue = new_event_queue;
    return this;
}

Button *Button::reset_event_queue() {
    event_queue = nullptr;
    return this;
}

Button *Button::set_args(unsigned *new_args) {
    args = new_args;
    return this;
}

Button *Button::reset_args() {
    args = nullptr;
    return this;
}

unsigned *Button::get_args() { return args; }

bool Button::is_pressed() const { return pressed; }

Button *Button::set_interactable(bool new_state) {
    if (new_state == enabled) {
        return this;
    }

    dirty = true;
    new_state = new_state;
    return this;
}

bool Button::get_interactable() const { return enabled; }

// Button Style

ButtonStyle *ButtonStyle::set_horizontal_alignment(ButtonStyle::HorizontalAlignment new_h_align) {
    h_align = new_h_align;
    return this;
}

ButtonStyle::HorizontalAlignment ButtonStyle::get_horizontal_alignment() const {
    return h_align;
}

ButtonStyle *ButtonStyle::set_vertical_alignment(ButtonStyle::VerticalAlignment new_v_align) {
    v_align = new_v_align;
    return this;
}

ButtonStyle::VerticalAlignment ButtonStyle::get_vertical_alignment() const {
    return v_align;
}

ButtonStyle *ButtonStyle::set_text_size(unsigned int new_text_size) {
    text_size = new_text_size;
    return this;
}

unsigned ButtonStyle::get_text_size() const {
    return text_size;
}

ButtonStyle *ButtonStyle::set_border_radius(unsigned int new_border_radius) {
    border_radius = new_border_radius;
    return this;
}

unsigned ButtonStyle::get_border_radius() const {
    return border_radius;
}

ButtonStyle *ButtonStyle::set_border_width(unsigned int new_border_w) {
    border_w = new_border_w;
    return this;
}

unsigned ButtonStyle::get_border_width() const {
    return border_w;
}

ButtonStyle *ButtonStyle::set_border_color(uint16_t new_border_color) {
    border_color = new_border_color;
    return this;
}

uint16_t ButtonStyle::get_border_color() const {
    return border_color;
}

ButtonStyle *ButtonStyle::set_fg_color(uint16_t new_fg_color) {
    fg_color = new_fg_color;
    return this;
}

uint16_t ButtonStyle::get_fg_color() const {
    return fg_color;
}

ButtonStyle *ButtonStyle::set_bg_color(uint16_t new_bg_color) {
    bg_color = new_bg_color;
    return this;
}

uint16_t ButtonStyle::get_bg_color() const {
    return bg_color;
}

ButtonStyle *ButtonStyle::set_pressed_bg_color(uint16_t new_pressed_bg_color) {

    pressed_bg_color = new_pressed_bg_color;
    return this;
}

uint16_t ButtonStyle::get_pressed_bg_color() const {
    return pressed_bg_color;
}

ButtonStyle *ButtonStyle::set_pressed_fg_color(uint16_t new_pressed_fg_color) {
    pressed_fg_color = new_pressed_fg_color;
    return this;
}

uint16_t ButtonStyle::get_pressed_fg_color() const {
    return pressed_fg_color;
}

ButtonStyle *ButtonStyle::set_pressed_border_color(uint16_t new_pressed_border_color) {
    pressed_border_color = new_pressed_border_color;
    return this;
}

uint16_t ButtonStyle::get_pressed_border_color() const {
    return pressed_border_color;
}
