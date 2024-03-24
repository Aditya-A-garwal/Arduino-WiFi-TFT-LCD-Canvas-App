/**
 * @file                    keyboard.cpp
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   This file implements the methods of the `Keyboard` class
 *
 */

#include "widgets/keyboard.h"

Keyboard::Keyboard(Frame *parent)
: Window(parent, 0, parent->get_height() - 160, parent->get_width(), 160)
{
    (&style)
    ->set_bg_color(GRAY)
    ->set_border_radius(0)
    ->set_border_width(0);

    exit = Button::create(this, 3, 136, 20, 20);

    shift = Button::create(this, 17, (32 * 3) + 1, 62 + 1, 31);
    space = Button::create(this, 32 + 32 - 5, (32 * 4) + 1, 32 * 6, 31);
    backspace = Button::create(this, (9 * 32) + 1 - 5, (32 * 4) + 1, 31, 31);

    exit
    ->set_message("X")
    ->get_style()
    ->set_text_size(1)
    ->set_bg_color(RED)
    ->set_fg_color(BLACK)
    ->set_border_width(0)
    ->set_border_radius(10);

    shift
    ->set_message("~")
    ->get_style()
    ->set_bg_color(blend_color(GRAY, BLACK, 100))
    ->set_pressed_bg_color(blend_color(GRAY, BLACK, 200))
    ->set_fg_color(WHITE)
    ->set_pressed_fg_color(WHITE)
    ->set_border_width(1)
    ->set_border_radius(2)
    ->set_text_size(2);

    space
    ->set_message(" ")
    ->get_style()
    ->set_bg_color(blend_color(GRAY, BLACK, 100))
    ->set_pressed_bg_color(blend_color(GRAY, BLACK, 200))
    ->set_fg_color(WHITE)
    ->set_pressed_fg_color(WHITE)
    ->set_border_width(1)
    ->set_border_radius(2)
    ->set_text_size(2);

    backspace
    ->set_message("<<")
    ->get_style()
    ->set_bg_color(blend_color(GRAY, BLACK, 100))
    ->set_pressed_bg_color(blend_color(GRAY, BLACK, 200))
    ->set_fg_color(WHITE)
    ->set_pressed_fg_color(WHITE)
    ->set_border_width(1)
    ->set_border_radius(2)
    ->set_text_size(2);


    for (unsigned i = 0, idx = 0; i < NUM_ROWS; ++i) {
        for (unsigned j = 0; j < NUM_KEYS[i]; ++j, ++idx) {

            if (i <= 1) {
                keys[idx] = Button::create(this, (32 * j) + 1, (32 * i) + 1, 31, 31);
            }
            else if (i <= 2) {
                keys[idx] = Button::create(this, (32 * j) + 17, (32 * i) + 1, 31, 31);
            }
            else if (i <= 3) {
                keys[idx] = Button::create(this, (32 * j) + 17 + 64, (32 * i) + 1, 31, 31);
            }
            else {
                keys[idx] = Button::create(this, ((j == 0) ? 32 : (8 * 32) + 1) - 5, (32 * i) + 1, 31, 31);
            }

            keys[idx]
            ->set_message(KEYBOARD_KEYS[0][i][j])
            ->get_style()
            ->set_bg_color(blend_color(GRAY, BLACK, 100))
            ->set_pressed_bg_color(blend_color(GRAY, BLACK, 200))
            ->set_fg_color(WHITE)
            ->set_pressed_fg_color(WHITE)
            ->set_border_width(1)
            ->set_border_radius(2)
            ->set_text_size(2);
        }
    }
}

void Keyboard::update_keys() {

    for (unsigned i = 1, idx = NUM_KEYS[0]; i < (NUM_ROWS-1); ++i) {
        for (unsigned j = 0; j < NUM_KEYS[i]; ++j, ++idx) {
            keys[idx]->set_message(KEYBOARD_KEYS[shift_i][i][j]);
        }
    }
}

Keyboard *Keyboard::create(Frame *parent) {

    Keyboard *keyboard = new (std::nothrow) Keyboard(parent);
    if (keyboard != nullptr) {
        parent->add_child(keyboard);
    }
    return keyboard;
}

Keyboard *Keyboard::set_shift_index(unsigned new_shift_i) {
    shift_i = new_shift_i;
    update_keys();
    dirty = true;
}

unsigned Keyboard::get_shift_index() const { return shift_i; }

bool Keyboard::propagate_press(unsigned x, unsigned y) {

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

    x -= widget_x;
    y -= widget_y;

    press_info.special = 0;

    pressed = true;

    if (shift->propagate_press(x, y)) {

        press_info.shift = 1;
        if (event_queue != nullptr && on_press != nullptr) {
            event_queue->push({on_press, (unsigned *)&press_info});
        }
        return true;
    }

    if (space->propagate_press(x, y)) {

        press_info.key = ' ';
        if (event_queue != nullptr && on_press != nullptr) {
            event_queue->push({on_press, (unsigned *)&press_info});
        }
        return true;
    }

    if (backspace->propagate_press(x, y)) {

        press_info.backspace = 1;
        if (event_queue != nullptr && on_press != nullptr) {
            event_queue->push({on_press, (unsigned *)&press_info});
        }
        return true;
    }

    if (exit->propagate_press(x, y)) {

        press_info.exit = 1;
        if (event_queue != nullptr && on_press != nullptr) {
            event_queue->push({on_press, (unsigned *)&press_info});
        }
        return true;
    }

    for (unsigned i = 0, idx = 0; i < NUM_ROWS; ++i) {
        for (unsigned j = 0; j < NUM_KEYS[i]; ++j, ++idx) {
            if (keys[idx]->propagate_press(x, y)) {

                press_info.key = KEYBOARD_KEYS[shift_i][i][j][0];
                if (event_queue != nullptr && on_press != nullptr) {
                    event_queue->push({on_press, (unsigned *)&press_info});
                }
                return true;
            }
        }
    }

    return true;
}

bool Keyboard::propagate_release(unsigned x, unsigned y) {

    if (!get_intersection(x, y)) {
        return false;
    }

    if (!pressed) {
        return true;
    }

    x -= widget_x;
    y -= widget_y;

    pressed = false;

    release_info.special = 0;

    if (shift->propagate_release(x, y)) {

        (++shift_i) %= 3;
        update_keys();

        release_info.shift = 1;
        if (event_queue != nullptr && on_release != nullptr) {
            event_queue->push({on_release, (unsigned *)&release_info});
        }
        return true;
    }

    if (space->propagate_release(x, y)) {

        release_info.key = ' ';
        if (event_queue != nullptr && on_release != nullptr) {
            event_queue->push({on_release, (unsigned *)&release_info});
        }
        return true;
    }

    if (backspace->propagate_release(x, y)) {

        release_info.backspace = 1;
        if (event_queue != nullptr && on_release != nullptr) {
            event_queue->push({on_release, (unsigned *)&release_info});
        }
        return true;
    }

    if (exit->propagate_release(x, y)) {

        release_info.exit = 1;
        if (event_queue != nullptr && on_release != nullptr) {
            event_queue->push({on_release, (unsigned *)&release_info});
        }

        set_visibility(false);
        return true;
    }

    for (unsigned i = 0, idx = 0; i < NUM_ROWS; ++i) {
        for (unsigned j = 0; j < NUM_KEYS[i]; ++j, ++idx) {
            if (keys[idx]->propagate_release(x, y)) {
                release_info.key = KEYBOARD_KEYS[shift_i][i][j][0];
                if (event_queue != nullptr && on_release != nullptr) {
                    event_queue->push({on_release, (unsigned *)&release_info});
                }
                return true;
            }
        }
    }

    return true;
}

// InteractiveWidget overrides

Keyboard *Keyboard::set_onpress(callback_t callback) {
    on_press = callback;
    return this;
}

Keyboard *Keyboard::reset_onpress() {
    on_press = nullptr;
    return this;
}

Keyboard *Keyboard::set_onrelease(callback_t callback) {
    on_release = callback;
    return this;
}

Keyboard *Keyboard::reset_onrelease() {
    on_release = nullptr;
    return this;
}

Keyboard *Keyboard::set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) {
    event_queue = new_event_queue;
    return this;
}

Keyboard *Keyboard::reset_event_queue() {
    event_queue = nullptr;
    return this;
}

Keyboard *Keyboard::set_args(unsigned *new_args) {
    args = new_args;
    return this;
}

Keyboard *Keyboard::reset_args() {
    args = nullptr;
    return this;
}

unsigned *Keyboard::get_args() { return args; }

bool Keyboard::is_pressed() const { return pressed; }

Keyboard *Keyboard::set_interactable(bool new_state) {
    if (new_state == enabled) {
        return this;
    }

    dirty = true;
    enabled = new_state;
    return this;
}

bool Keyboard::get_interactable() const { return enabled; }
