#include "widgets/textbox.h"

TextBox::TextBox(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height)
    : Label(parent, x, y, width, height)
{}

TextBox *TextBox::create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height) {
    TextBox *textbox = new (std::nothrow) TextBox(parent, x, y, width, height);
    if (textbox != nullptr) {
        parent->add_child(textbox);
    }
    return textbox;
}

bool TextBox::propagate_press(unsigned x, unsigned y) {

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

bool TextBox::propagate_release(unsigned x, unsigned y) {

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

// InteractiveWidget overrides

TextBox *TextBox::set_onpress(callback_t callback) {
    on_press = callback;
    return this;
}

TextBox *TextBox::reset_onpress() {
    on_press = nullptr;
    return this;
}

TextBox *TextBox::set_onrelease(callback_t callback) {
    on_release = callback;
    return this;
}

TextBox *TextBox::reset_onrelease() {
    on_release = nullptr;
    return this;
}

TextBox *TextBox::set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) {
    event_queue = new_event_queue;
    return this;
}

TextBox *TextBox::reset_event_queue() {
    event_queue = nullptr;
    return this;
}

TextBox *TextBox::set_args(unsigned *new_args) {
    args = new_args;
    return this;
}

TextBox *TextBox::reset_args() {
    args = nullptr;
    return this;
}

unsigned *TextBox::get_args() { return args; }

bool TextBox::is_pressed() const { return pressed; }

TextBox *TextBox::set_interactable(bool new_state) {
    if (new_state == enabled) {
        return this;
    }

    dirty = true;
    new_state = new_state;
    return this;
}

bool TextBox::get_interactable() const { return enabled; }
