#include "widgets/label.h"

Label::Label(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height)
    : parent {parent}
    , widget_x {x}
    , widget_y {y}
    , widget_absolute_x { x + parent->get_absolute_x() }
    , widget_absolute_y { y + parent->get_absolute_y() }
    , widget_w {width}
    , widget_h {height}
{}

Label *Label::create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height) {
    Label *label = new (std::nothrow) Label(parent, x, y, width, height);
    if (label != nullptr) {
        parent->add_child(label);
    }
    return label;
}

Label *Label::set_message(const char msg_ptr[]) {
    dirty = true;
    message = msg_ptr;

    // render_text();

    return this;
}

String Label::get_message() const {
    return message;
}

unsigned Label::get_message_len() const {
    return message.length();
}

Label *Label::append_to_message(char ch) {
    dirty = true;
    message += ch;

    // render_text();

    return this;
}

Label *Label::pop_from_message() {

    if (message.length() <= 0) {
        return this;
    }

    dirty = true;
    message.remove(message.length() - 1);

    // render_text();

    return this;
}

LabelStyle *Label::get_style() {
    dirty = true;
    return &style;
}

// BasicWidget overrides

Frame *Label::get_parent() { return parent; }

unsigned Label::get_x() const { return widget_x; }
unsigned Label::get_y() const { return widget_y; }

unsigned Label::get_absolute_x() const { return widget_absolute_x; }
unsigned Label::get_absolute_y() const { return widget_absolute_y; }

unsigned Label::get_width() const { return widget_w; }
unsigned Label::get_height() const { return widget_h; }

bool Label::get_dirty() const { return dirty; }
bool Label::get_visibility_changed() const { return visibility_changed; }

void Label::set_dirty() { dirty = true; }
void Label::set_visibility_changed() { visibility_changed = true; }

void Label::draw() {

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
                    style.border_color
            );
        }
        parent->fill_round_rect(
                widget_x + style.border_w,
                widget_y + style.border_w,
                widget_w - 2*style.border_w,
                widget_h - 2*style.border_w,
                style.border_radius,
                style.bg_color
        );
    }
    else {
        if (style.border_w != 0) {
            parent->fill_rect(
                    widget_x,
                    widget_y,
                    widget_w,
                    widget_h,
                    style.border_color
            );
        }
        parent->fill_rect(
                widget_x + style.border_w,
                widget_y + style.border_w,
                widget_w - 2*style.border_w,
                widget_h - 2*style.border_w,
                style.bg_color
        );
    }

    render_text();
}

void Label::clear() {

    dirty = false;
    visibility_changed = false;

    parent->fill_rect(widget_x, widget_y, widget_w, widget_h, BLACK);
}

bool Label::get_intersection(unsigned x, unsigned y) const {
    return (widget_x <= x && x <= (widget_x + widget_w))
        && (widget_y <= y && y <= (widget_y + widget_h));
}

bool Label::get_intersection(BasicWidget *other) const {

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

bool Label::propagate_press(unsigned x, unsigned y) {

    if (!get_intersection(x, y)) {
        return false;
    }

    return true;
}

bool Label::propagate_release(unsigned x, unsigned y) {

    if (!get_intersection(x, y)) {
        return false;
    }

    return true;
}

bool Label::get_visibility() const { return visible; }
void Label::set_visibility(bool new_visibility) {

    if (visible == new_visibility) {
        return;
    }

    dirty = true;
    visibility_changed = true;

    visible = new_visibility;
}

// void Label::render_text() {

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
//             (style.h_align == LabelStyle::HorizontalAlignment::CENTER_ALIGN)
//             ? (2*widget_x + widget_w - w1) / 2
//             : (style.h_align == LabelStyle::HorizontalAlignment::LEFT_ALIGN)
//             ? widget_x + style.border_w
//             : widget_x + widget_w - style.border_w - w1,
//             (style.v_align == LabelStyle::VerticalAlignment::MIDDLE_ALIGN)
//             ? (2*widget_y + widget_h - h1) / 2
//             : (style.v_align == LabelStyle::VerticalAlignment::TOP_ALIGN)
//             ? widget_y + style.border_w
//             : widget_y + widget_h - style.border_w - h1,
//             style.text_size,
//             style.fg_color
//     );
// }

void Label::render_text() {

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

    parent->get_text_bounds(
            message.c_str(),
            1,
            0,
            0,
            &x1,
            &y1,
            &w1,
            &h1
    );

    parent->print(
            message.c_str(),
            (style.h_align == LabelStyle::HorizontalAlignment::CENTER_ALIGN)
            ? (2*widget_x + widget_w - w1) / 2
            : (style.h_align == LabelStyle::HorizontalAlignment::LEFT_ALIGN)
            ? widget_x + style.border_w
            : widget_x + widget_w - style.border_w - w1,
            ((style.v_align == LabelStyle::VerticalAlignment::MIDDLE_ALIGN)
            ? (2*widget_y + widget_h - h1) / 2
            : (style.v_align == LabelStyle::VerticalAlignment::TOP_ALIGN)
            ? widget_y + style.border_w
            : widget_y + widget_h - style.border_w - h1) + h1,
            1,// style.text_size,
            style.fg_color
    );
}

// Label Style

LabelStyle *LabelStyle::set_horizontal_alignment(LabelStyle::HorizontalAlignment new_h_align) {
    h_align = new_h_align;
    return this;
}

LabelStyle::HorizontalAlignment LabelStyle::get_horizontal_alignment() const {
    return h_align;
}

LabelStyle *LabelStyle::set_vertical_alignment(LabelStyle::VerticalAlignment new_v_align) {
    v_align = new_v_align;
    return this;
}

LabelStyle::VerticalAlignment LabelStyle::get_vertical_alignment() const {
    return v_align;
}

LabelStyle *LabelStyle::set_text_size(unsigned int new_text_size) {
    text_size = new_text_size;
    return this;
}

unsigned LabelStyle::get_text_size() const {
    return text_size;
}

LabelStyle *LabelStyle::set_border_radius(unsigned int new_border_radius) {
    border_radius = new_border_radius;
    return this;
}

unsigned LabelStyle::get_border_radius() const {
    return border_radius;
}

LabelStyle *LabelStyle::set_border_width(unsigned int new_border_w) {
    border_w = new_border_w;
    return this;
}

unsigned LabelStyle::get_border_width() const {
    return border_w;
}

LabelStyle *LabelStyle::set_border_color(uint16_t new_border_color) {
    border_color = new_border_color;
    return this;
}

uint16_t LabelStyle::get_border_color() const {
    return border_color;
}

LabelStyle *LabelStyle::set_fg_color(uint16_t new_fg_color) {
    fg_color = new_fg_color;
    return this;
}

uint16_t LabelStyle::get_fg_color() const {
    return fg_color;
}

LabelStyle *LabelStyle::set_bg_color(uint16_t new_bg_color) {
    bg_color = new_bg_color;
    return this;
}

uint16_t LabelStyle::get_bg_color() const {
    return bg_color;
}
