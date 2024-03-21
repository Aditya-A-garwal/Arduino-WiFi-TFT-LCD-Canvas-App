#include "widgets/pensizeselector.h"

PenSizeSelector::PenSizeSelector(Frame *parent, unsigned x, unsigned y)
    : parent {parent}
    , widget_x {x}
    , widget_y {y}
    , widget_absolute_x {x + parent->get_absolute_x() }
    , widget_absolute_y {y + parent->get_absolute_y() }
{}

PenSizeSelector *PenSizeSelector::create(Frame *parent, unsigned x, unsigned y) {
    PenSizeSelector *pensizeselector = new (std::nothrow) PenSizeSelector(parent, x, y);
    if (pensizeselector != nullptr) {
        parent->add_child(pensizeselector);
    }
    return pensizeselector;
}

PenSizeSelector *PenSizeSelector::set_size(unsigned pos, uint16_t new_size) {
    if (pos > sizeof(sizes) / sizeof(sizes[0])) {
        return this;
    }

    dirty = true;
    sizes[pos] = new_size;
    return this;
}

uint16_t PenSizeSelector::get_size(unsigned pos) const {
    if (pos > sizeof(sizes) / sizeof(sizes[0])) {
        return 0;
    }

    return sizes[pos];
}

PenSizeSelector *PenSizeSelector::set_active_size(unsigned pos) {
    if (pos > sizeof(sizes) / sizeof(sizes[0])) {
        return this;
    }

    dirty = true;
    active_size = pos;
    return this;
}
uint16_t PenSizeSelector::get_active_size() const { return sizes[active_size]; }

PenSizeSelector *PenSizeSelector::set_color(uint16_t new_color) {
    if (new_color == color) {
        return this;
    }

    dirty = true;
    color = new_color;
    return this;
}

uint16_t PenSizeSelector::get_color() const {
    return color;
}

// BasicWidget overrides

Frame *PenSizeSelector::get_parent() { return parent; }

unsigned PenSizeSelector::get_x() const { return widget_x; }
unsigned PenSizeSelector::get_y() const { return widget_y; }

unsigned PenSizeSelector::get_absolute_x() const { return widget_absolute_x; }
unsigned PenSizeSelector::get_absolute_y() const { return widget_absolute_y; }

unsigned PenSizeSelector::get_width() const { return WIDTH; }
unsigned PenSizeSelector::get_height() const { return HEIGHT; }

bool PenSizeSelector::get_dirty() const { return dirty; }
bool PenSizeSelector::get_visibility_changed() const { return visibility_changed; }

void PenSizeSelector::set_dirty() { dirty = true; }
void PenSizeSelector::set_visibility_changed() { visibility_changed = true; }

void PenSizeSelector::draw() {

    unsigned x, y, s;

    dirty = false;
    visibility_changed = false;

    for (unsigned c = 0; c < 4; ++c) {

        x = widget_x + (2*c + 1) * H_PAD;
        y = widget_y + (HEIGHT / 2);

        s = sizes[c];

        parent->fill_circle(x, y, s, (color == BLACK)
                                        ? (blend_color(blend_color(BLACK, WHITE, 12), BLUE, 10))
                                        : (color));

        parent->draw_circle(x, y, s + 3, (c == active_size)
                                            ? WHITE
                                            : BLACK);
    }
}
void PenSizeSelector::clear() {

    dirty = false;
    visibility_changed = false;

    parent->fill_rect(widget_x, widget_y, WIDTH, HEIGHT, BLACK);
}

bool PenSizeSelector::get_intersection(unsigned x, unsigned y) const {

    return (widget_x <= x && x <= (widget_x + WIDTH))
        && (widget_y <= y && y <= (widget_y + HEIGHT));
}

bool PenSizeSelector::get_intersection(BasicWidget *other) const {

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

bool PenSizeSelector::propagate_press(unsigned x, unsigned y) {

    unsigned x1, y1;

    if (!get_intersection(x, y)) {
        return false;
    }

    if (pressed || !enabled) {
        return true;
    }

    pressed = true;

    x -= widget_x;
    y -= widget_y;

    y1 = HEIGHT / 2;
    y1 = (y1 > y) ? (y1 - y) : (y - y1);
    y1 *= y1;

    for (unsigned c = 0; c < 4; ++c) {

        x1 = (2*c + 1) * H_PAD;
        x1 = (x1 > x) ? (x1 - x) : (x - x1);
        x1 *= x1;

        if ((x1 + y1) > ((2+sizes[c]) * (2+sizes[c]))) {
            continue;
        }

        if (event_queue != nullptr && on_press != nullptr) {
            event_queue->push({on_press, (unsigned *)&sizes[active_size]});
        }
        break;
    }

    return true;
}
bool PenSizeSelector::propagate_release(unsigned x, unsigned y) {

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

    y1 = HEIGHT / 2;
    y1 = (y1 > y) ? (y1 - y) : (y - y1);
    y1 *= y1;

    for (unsigned c = 0; c < 4; ++c) {

        x1 = (2*c + 1) * H_PAD;
        x1 = (x1 > x) ? (x1 - x) : (x - x1);
        x1 *= x1;

        if ((x1 + y1) > ((2+sizes[c]) * (2+sizes[c]))) {
            continue;
        }

        set_active_size(c);
        if (event_queue != nullptr && on_release != nullptr) {
            event_queue->push({on_release, (unsigned *)&sizes[active_size]});
        }
        break;
    }

    return true;
}

bool PenSizeSelector::get_visibility() const { return visible; }
void PenSizeSelector::set_visibility(bool new_visibility) {
    if (visible == new_visibility) {
        return;
    }

    dirty = true;
    visibility_changed = true;

    visible = new_visibility;
}

// InteractiveWidget overrides

PenSizeSelector *PenSizeSelector::set_onpress(callback_t callback) {
    on_press = callback;
    return this;
}
PenSizeSelector *PenSizeSelector::reset_onpress() {
    on_press = nullptr;
    return this;
}

PenSizeSelector *PenSizeSelector::set_onrelease(callback_t callback) {
    on_release = callback;
    return this;
}
PenSizeSelector *PenSizeSelector::reset_onrelease() {
    on_release = nullptr;
    return this;
}

PenSizeSelector *PenSizeSelector::set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) {
    event_queue = new_event_queue;
    return this;
}

PenSizeSelector *PenSizeSelector::reset_event_queue() {
    event_queue = nullptr;
    return this;
}

PenSizeSelector *PenSizeSelector::set_args(unsigned *new_args) {
    args = new_args;
    return this;
}

PenSizeSelector *PenSizeSelector::reset_args() {
    args = nullptr;
    return this;
}

unsigned *PenSizeSelector::get_args() { return args; }

bool PenSizeSelector::is_pressed() const { return pressed; }

PenSizeSelector *PenSizeSelector::set_interactable(bool new_state) {
    if (new_state == enabled) {
        return this;
    }

    dirty = true;
    new_state = new_state;
    return this;
}

bool PenSizeSelector::get_interactable() const { return enabled; }