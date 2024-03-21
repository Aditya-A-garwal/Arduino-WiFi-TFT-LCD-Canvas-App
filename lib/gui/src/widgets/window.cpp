#include "widgets/window.h"

Window::Window(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height)
    : parent {parent}
    , widget_x {x}
    , widget_y {y}
    , widget_absolute_x {x + parent->get_absolute_x()}
    , widget_absolute_y {y + parent->get_absolute_y()}
    , widget_w {width}
    , widget_h {height}
{}

Window *Window::create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height) {
    Window *window = new Window(parent, x, y, width, height);
    if (window != nullptr) {
        parent->add_child(window);
    }
    return window;
}

WindowStyle *Window::get_style() {

    dirty = true;
    return &style;
}

// BasicWidget overrides

Frame *Window::get_parent() { return parent; }

unsigned Window::get_x() const { return widget_x; }
unsigned Window::get_y() const { return widget_y; }

unsigned Window::get_absolute_x() const { return widget_absolute_x; }
unsigned Window::get_absolute_y() const { return widget_absolute_y; }

unsigned Window::get_width() const { return widget_w; }
unsigned Window::get_height() const { return widget_h; }

bool Window::get_dirty() const { return dirty; }
bool Window::get_visibility_changed() const { return visibility_changed; }

void Window::set_dirty() { dirty = true; }
void Window::set_visibility_changed() { visibility_changed = true; }

void Window::draw() {

    dirty = false;
    visibility_changed = false;

    if (style.border_radius != 0) {
        if (style.border_w > 1) {
            parent->fill_round_rect(
                    widget_x,
                    widget_y,
                    widget_w,
                    widget_h,
                    style.border_radius,
                    style.border_color
            );
            parent->fill_round_rect(
                    widget_x + style.border_w,
                    widget_y + style.border_w,
                    widget_w - 2*style.border_w,
                    widget_h - 2*style.border_w,
                    style.border_radius,
                    style.bg_color
            );
        }
        if (style.border_w == 1) {
            parent->fill_round_rect(
                    widget_x,
                    widget_y,
                    widget_w,
                    widget_h,
                    style.border_radius,
                    style.bg_color
            );
            parent->draw_round_rect(
                    widget_x,
                    widget_y,
                    widget_w,
                    widget_h,
                    style.border_radius,
                    style.border_color
            );
        }
        else {
            parent->fill_round_rect(
                    widget_x + style.border_w,
                    widget_y + style.border_w,
                    widget_w - 2*style.border_w,
                    widget_h - 2*style.border_w,
                    style.border_radius,
                    style.bg_color
            );
        }
    }
    else {
        if (style.border_w > 1) {
            parent->fill_rect(
                    widget_x,
                    widget_y,
                    widget_w,
                    widget_h,
                    style.border_color
            );
            parent->fill_rect(
                    widget_x + style.border_w,
                    widget_y + style.border_w,
                    widget_w - 2*style.border_w,
                    widget_h - 2*style.border_w,
                    style.bg_color
            );
        }
        if (style.border_w == 1) {
            parent->fill_rect(
                    widget_x,
                    widget_y,
                    widget_w,
                    widget_h,
                    style.bg_color
            );
            parent->draw_rect(
                    widget_x,
                    widget_y,
                    widget_w,
                    widget_h,
                    style.border_color
            );
        }
        else {
            parent->fill_rect(
                    widget_x,
                    widget_y,
                    widget_w,
                    widget_h,
                    style.bg_color
            );
        }
    }

    for (auto it = children.rbegin(); it != children.rend(); ++it) {

        BasicWidget *child = *it;

        if (child->get_visibility()) {
            child->draw();
        }
    }
}

void Window::clear() {

    dirty = false;
    visibility_changed = false;

    parent->fill_rect(widget_x, widget_y, widget_w, widget_h, BLACK);
}

bool Window::get_intersection(unsigned int x, unsigned int y) const {
    return (widget_x <= x && x <= (widget_x + widget_w))
           && (widget_y <= y && y <= (widget_y + widget_h));
}

bool Window::get_intersection(BasicWidget *other) const {

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

bool Window::propagate_press(unsigned x, unsigned y) {

    if (!get_intersection(x, y)) {
        return false;
    }

    x -= widget_x;
    y -= widget_y;

    for (auto &child : children) {
        if (child->get_visibility() && child->propagate_press(x, y)) {
            break;
        }
    }

    return true;
}

bool Window::propagate_release(unsigned x, unsigned y) {

    if (!get_intersection(x, y)) {
        return false;
    }

    x -= widget_x;
    y -= widget_y;

    for (auto &child : children) {
        if (child->get_visibility() && child->propagate_release(x, y)) {
            break;
        }
    }

    return true;
}

bool Window::get_visibility() const { return visible; }

void Window::set_visibility(bool new_visibility) {

    if (visible == new_visibility) {
        return;
    }

    dirty = true;
    visibility_changed = true;

    visible = new_visibility;
}

// Frame overrides

Window *Window::set_at(unsigned int x, unsigned int y, uint16_t color) {
    parent->set_at(widget_x + x, widget_y + y, color);
    return this;
}

uint16_t Window::get_at(unsigned int x, unsigned int y) const { return parent->get_at(widget_x + x, widget_y + y); }

Window *Window::draw_line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, uint16_t color) {
    parent->draw_line(widget_x + x0, widget_y + y0, widget_x + x0, widget_y + y0, color);
    return this;
}

Window *Window::draw_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint16_t color) {
    parent->draw_rect(widget_x + x, widget_y + y, w, h, color);
    return this;
}

Window *Window::fill_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint16_t color) {
    parent->fill_rect(widget_x + x, widget_y + y, w, h, color);
    return this;
}

Window *Window::draw_round_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int r, uint16_t color) {
    parent->draw_round_rect(widget_x + x, widget_y + y, w, h, r, color);
    return this;
}

Window *Window::fill_round_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int r, uint16_t color) {
    parent->fill_round_rect(widget_x + x, widget_y + y, w, h, r, color);
    return this;
}

Window *Window::draw_circle(unsigned int x, unsigned int y, unsigned int r, uint16_t color) {
    parent->draw_circle(widget_x + x, widget_y + y, r, color);
    return this;
}

Window *Window::fill_circle(unsigned int x, unsigned int y, unsigned int r, uint16_t color) {
    parent->fill_circle(widget_x + x, widget_y + y, r, color);
    return this;
}

Window *Window::get_text_bounds(const char *text, unsigned int text_size, unsigned int x, unsigned int y, int16_t *x1,
                             int16_t *y1, uint16_t *w, uint16_t *h) {
    parent->get_text_bounds(text, text_size, widget_x + x, widget_y + y, x1, y1, w, h);
    *x1 -= widget_x;
    *y1 -= widget_y;

    return this;
}

Window *Window::set_font(const GFXfont *f) {
    parent->set_font(f);
    return this;
}

Window *Window::print(const char *text, unsigned int x, unsigned int y, unsigned int text_size, uint16_t fg_color) {
    parent->print(text, widget_x + x, widget_y + y, text_size, fg_color);
    return this;
}

Window *Window::print_opaque(const char *text, unsigned int x, unsigned int y, unsigned int text_size, uint16_t fg_color,
                          uint16_t bg_color) {
    parent->print_opaque(text, widget_x + x, widget_y + y, text_size, fg_color, bg_color);
    return this;
}

Window *Window::draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height) {
    parent->draw_rgb_bitmap(x + widget_x, y + widget_y, data, width, height);
    return this;
}

Window *Window::add_child(BasicWidget *child) {

    children.emplace_back(child);
    return this;
}

unsigned Window::get_children_count() const { return children.size(); }

Window *Window::send_front(BasicWidget *child, unsigned amt) {

    unsigned cur_z, new_z;
    std::vector<BasicWidget *>::iterator it;

    it = std::find(children.begin(), children.end(), child);
    if (it == children.end()) {
        return this;
    }

    cur_z = it - children.begin();
    new_z = (amt == 0)
    ? 0
    : cur_z - min(amt, cur_z);

    if (cur_z == new_z) {
        return this;
    }

    for (unsigned i = cur_z; i > new_z; --i) {
        children[i] = children[i - 1];
    }
    children[new_z] = child;
    child->set_dirty();

    return this;
}

Window *Window::send_back(BasicWidget *child, unsigned amt) {

    unsigned cur_z, new_z;
    std::vector<BasicWidget *>::iterator it;

    it = std::find(children.begin(), children.end(), child);
    if (it == children.end()) {
        return this;
    }

    cur_z = it - children.begin();
    new_z = (amt == 0)
    ? children.size() - 1
    : min(children.size() - 1, cur_z + amt);

    if (cur_z == new_z) {
        return this;
    }

    for (unsigned i = cur_z; i < new_z; ++i) {
        children[i] = children[i + 1];
    }
    children[new_z] = child;
    child->set_dirty();

    return this;
}

void Window::collect_dirty_widgets(RingQueueInterface<BasicWidget *> *dirty_widgets) {

    BasicWidget *child;

    for (auto it = children.rbegin(); it != children.rend(); ++it) {

        child = *it;

        if (child->get_dirty()) {
            dirty_widgets->push(child);
            continue;
        }

        if (child->get_visibility() && child->is_frame()) {
            ((Frame *)child)->collect_dirty_widgets(dirty_widgets);
        }
    }
}

void Window::collect_overlapped_widgets(BasicWidget *dirty, BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) {

    std::vector<BasicWidget *>::reverse_iterator it;
    BasicWidget *current;

    it = std::find(children.rbegin(), children.rend(), child);
    if (it == children.rend()) {
        return;
    }

    for (++it; it != children.rend(); ++it) {
        current = *it;

        if (current->get_dirty() || !current->get_visibility()) {
            continue;
        }
        // if (dirty->get_intersection(current)) {
        if (current->get_intersection(dirty)) {
            current->set_dirty();
            overlapping_widgets->push(current);
        }
    }
}

void Window::collect_underlapped_widgets(BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) {

    BasicWidget *current;

    for (auto it = children.rbegin(); it != children.rend(); ++it) {

        current = *it;

        if (current == child) {
            break;
        }

        if (current->get_dirty() || !current->get_visibility()) {
            continue;
        }

        // if (dirty->get_intersection(current)) {
        if (current->get_intersection(child)) {
            current->set_dirty();
            overlapping_widgets->push(current);
        }
    }
}

WindowStyle *WindowStyle::set_bg_color(uint16_t new_color) {
    bg_color = new_color;
}

uint16_t WindowStyle::get_bg_color() const {
    return bg_color;
}

WindowStyle *WindowStyle::set_border_color(uint16_t new_color) {
    border_color = new_color;
    return this;
}

uint16_t WindowStyle::get_border_color() const {
    return border_color;
}

WindowStyle *WindowStyle::set_border_width(unsigned new_border_width) {
    border_w = new_border_width;
    return this;
}

unsigned WindowStyle::get_border_width() const {
    return border_w;
}

WindowStyle *WindowStyle::set_border_radius(unsigned new_border_radius) {
    border_radius = new_border_radius;
    return this;
}

unsigned WindowStyle::get_border_radius() const {
    return border_radius;
}
