/**
 * @file                    view.cpp
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   This file implemented the methods of the `View` class
 *
 */

#include "algorithm"
#include "widgets/view.h"

View::View(App *app)
    : app {app}
{
    app->add_view(this);
}

View *View::create(App *app) {
    View *view = new View(app);
    return view;
}

// BasicWidget overrides

App *View::get_parent() { return app; }

unsigned View::get_x() const {  return 0; }
unsigned View::get_y() const { return 0; }

unsigned View::get_absolute_x() const {  return 0; }
unsigned View::get_absolute_y() const { return 0; }

unsigned View::get_width() const { return app->get_width(); }
unsigned View::get_height() const { return app->get_height(); }

bool View::get_dirty() const { return dirty; }
bool View::get_visibility_changed() const { return false; }

void View::set_dirty() { dirty = true; }
void View::set_visibility_changed() { return; }

void View::draw() {

    dirty = false;

    for (auto it = children.rbegin(); it != children.rend(); ++it) {

        BasicWidget *child = *it;

        if (child->get_visibility()) {
            child->draw();
        }
    }
}

void View::clear() {

    dirty = false;
    app->clear();
}

bool View::get_intersection(unsigned int x, unsigned int y) const { return true; }
bool View::get_intersection(BasicWidget *other) const { return true; }

bool View::propagate_press(unsigned x, unsigned y) {

    for (auto &child : children) {
        if (child->get_visibility() && child->propagate_press(x, y)) {
            break;
        }
    }

    return true;
}

bool View::propagate_release(unsigned x, unsigned y) {

    for (auto &child : children) {
        if (child->get_visibility() && child->propagate_release(x, y)) {
            break;
        }
    }

    return true;
}

bool View::get_visibility() const { return true; }
void View::set_visibility(bool new_visibility) { return; }

// Frame Overrides

View *View::set_at(unsigned int x, unsigned int y, uint16_t color) {
    app->set_at(x, y, color);
    return this;
}

uint16_t View::get_at(unsigned int x, unsigned int y) const {
    return app->get_at(x, y);
}

View *View::draw_line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, uint16_t color) {
    app->draw_line(x0, y0, x0, y0, color);
    return this;
}

View *View::draw_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint16_t color) {
    app->draw_rect(x, y, w, h, color);
    return this;
}

View *View::fill_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint16_t color) {
    app->fill_rect(x, y, w, h, color);
    return this;
}

View *View::draw_round_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int r,
                                   uint16_t color) {
    app->draw_round_rect(x, y, w, h, r, color);
    return this;
}

View *View::fill_round_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int r,
                                   uint16_t color) {
    app->fill_round_rect(x, y, w, h, r, color);
    return this;
}

View *View::draw_circle(unsigned int x, unsigned int y, unsigned int r, uint16_t color) {
    app->draw_circle(x, y, r, color);
    return this;
}

View *View::fill_circle(unsigned int x, unsigned int y, unsigned int r, uint16_t color) {
    app->fill_circle(x, y, r, color);
    return this;
}

View *View::get_text_bounds(const char *text, unsigned int text_size, unsigned int x, unsigned int y, int16_t *x1,
                              int16_t *y1, uint16_t *w, uint16_t *h) {
    app->get_text_bounds(text, text_size, x, y, x1, y1, w, h);
    return this;
}

View *View::set_font(const GFXfont *f) {
    app->set_font(f);
    return this;
}

View *View::print(const char *text, unsigned int x, unsigned int y, unsigned int text_size, uint16_t fg_color) {
    app->print(text, x, y, text_size, fg_color);
    return this;
}

View *View::print_opaque(const char *text, unsigned int x, unsigned int y, unsigned int text_size, uint16_t fg_color,
                           uint16_t bg_color) {
    app->print_opaque(text, x, y, text_size, fg_color, bg_color);
    return this;
}

View *View::draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height) {
    app->draw_rgb_bitmap(x, y, data, width, height);
    return this;
}

View *View::add_child(BasicWidget *child) {

    children.emplace_back(child);
    return this;
}

unsigned View::get_children_count() const { return children.size(); }

View *View::send_front(BasicWidget *child, unsigned amt) {

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

View *View::send_back(BasicWidget *child, unsigned amt) {

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

void View::collect_dirty_widgets(RingQueueInterface<BasicWidget *> *dirty_widgets) {

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

void View::collect_overlapped_widgets(BasicWidget *dirty, BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) {

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

void View::collect_underlapped_widgets(BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) {

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
