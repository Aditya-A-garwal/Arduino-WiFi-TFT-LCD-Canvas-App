#include "algorithm"

#include "widgets/app.h"
#include "widgets/view.h"

App::App(MCUFRIEND_kbv *display)
: display {display}
{
    clear();
}

App *App::create(MCUFRIEND_kbv *display) {
    App *app = new (std::nothrow) App(display);
    return app;
}

App *App::add_view(View *child) {
    views.emplace_back(child);
    return this;
}

App *App::make_active_view(View *child) {

    if (std::find(views.begin(), views.end(), child) == views.end()) {
        return this;
    }
    active_view = child;
    active_view->set_dirty(); //todo all my children should be marked as clean (including bounding box and visibility and stuff) (possibly use force_clean method)
    clear();

    return this;
}

View *App::get_active_view() const {
    return active_view;
}

RingQueueInterface<InteractiveWidget::callback_event_t> *App::get_event_queue() {
    return &event_queue;
}

App *App::push_event(const InteractiveWidget::callback_event_t &event) {
    event_queue.push(event);
}

App *App::execute_event_logic() {

    InteractiveWidget::callback_t cb;
    unsigned *args;

    while (event_queue.get_size() != 0) {

        cb = event_queue.front().cb;
        args = event_queue.front().args;

        cb(args);

        event_queue.pop();
    }

    return this;
}

App *App::collect_dirty_widgets() {

    if (active_view->get_dirty()) {
        dirty_widgets.push(active_view);
    }
    else {
        ((View *)active_view)->collect_dirty_widgets(&dirty_widgets);
    }

    return this;
}

App *App::update_dirty_widgets() {

    while (dirty_widgets.get_size() != 0) {

        BasicWidget *dirty;
        BasicWidget *parent;
        BasicWidget *child;

        dirty = dirty_widgets.front();
        dirty_widgets.pop();

        if (dirty->get_visibility()) {

            dirty->draw();

            child = dirty;
            parent = dirty->get_parent();

            while (parent != this) {
                ((Frame *)parent)->collect_overlapped_widgets(dirty, child, &dirty_widgets);

                child = parent;
                parent = parent->get_parent();
            }
        }
        else {

            dirty->clear();

            child = dirty;
            parent = dirty->get_parent();

            ((Frame *)parent)->collect_underlapped_widgets(dirty, &dirty_widgets);

            while (parent != this) {
                ((Frame *)parent)->collect_overlapped_widgets(dirty, child, &dirty_widgets);

                child = parent;
                parent = parent->get_parent();
            }
        }
    }

    return this;
}

// BasicWidget overrides

BasicWidget *App::get_parent() { return nullptr; }

unsigned App::get_x() const { return 0; }
unsigned App::get_y() const { return 0; }

unsigned App::get_absolute_x() const { return 0; }
unsigned App::get_absolute_y() const { return 0; }

unsigned App::get_width() const { return display->width(); }
unsigned App::get_height() const { return display->height(); }

bool App::get_dirty() const { return false; }
bool App::get_visibility_changed() const { return false; }

void App::set_dirty() { return; }
void App::set_visibility_changed() { return; }

void App::draw() { active_view->draw(); }

void App::clear() { display->fillRect(0, 0, display->width(), display->height(), BLACK); }

bool App::get_intersection(unsigned int x, unsigned int y) const { return true; }
bool App::get_intersection(BasicWidget *other) const { return true; }

bool App::propagate_press(unsigned x, unsigned y) { return active_view->propagate_press(x, y); }
bool App::propagate_release(unsigned x, unsigned y) { return active_view->propagate_release(x, y); }

bool App::get_visibility() const { return true; }
void App::set_visibility(bool new_visibility) { return; }

// Frame overrides

App *App::set_at(unsigned int x, unsigned int y, uint16_t color) {
    display->writePixel(x, y, color);
    return this;
}

uint16_t App::get_at(unsigned int x, unsigned int y) const {

#ifdef READ_PIXEL_ENABLED
    return display->readPixel(x, y);
#else
    return 0;
#endif
}

App *App::draw_line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, uint16_t color) {
    display->drawLine(x0, y0, x1, y1, color);
    return this;
}

App *App::draw_rect(unsigned int x0, unsigned int y0, unsigned int w, unsigned int h, uint16_t color) {
    display->drawRect(x0, y0, w, h, color);
    return this;
}

App *App::fill_rect(unsigned int x0, unsigned int y0, unsigned int w, unsigned int h, uint16_t color) {
    display->fillRect(x0, y0, w, h, color);
    return this;
}

App *App::draw_round_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int r, uint16_t color) {
    display->drawRoundRect(x, y, w, h, r, color);
    return this;
}

App *App::fill_round_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int r, uint16_t color) {
    display->fillRoundRect(x, y, w, h, r, color);
    return this;
}

App *App::draw_circle(unsigned int x, unsigned int y, unsigned int r, uint16_t color) {
    display->drawCircle(x, y, r, color);
    return this;
}

App *App::fill_circle(unsigned int x, unsigned int y, unsigned int r, uint16_t color) {
    display->fillCircle(x, y, r, color);
    return this;
}

App *App::get_text_bounds(const char *text, unsigned int text_size, unsigned int x, unsigned int y, int16_t *x1, int16_t *y1,
                     uint16_t *w, uint16_t *h) {
    display->setTextSize(text_size);
    display->getTextBounds(text, x, y, x1, y1, w, h);
    return this;
}

App *App::set_font(const GFXfont *f) {
    display->setFont(f);
    return this;
}

App *App::print(const char *text, unsigned int x, unsigned int y, unsigned int text_size, uint16_t fg_color) {

    display->setCursor(x, y);
    display->setTextColor(fg_color);
    display->setTextSize(text_size);

    display->print(text);

    return this;
}

App *App::print_opaque(const char *text, unsigned int x, unsigned int y, unsigned int text_size, uint16_t fg_color,
                       uint16_t bg_color) {

    display->setCursor(x, y);
    display->setTextColor(fg_color, bg_color);
    display->setTextSize(text_size);

    display->print(text);

    return this;
}

App *App::draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height) {

    display->drawRGBBitmap(x, y, data, width, height);
    return this;
}

