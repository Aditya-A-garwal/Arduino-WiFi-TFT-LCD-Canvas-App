#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_KEYBOARD_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_KEYBOARD_H__

#include "widget.h"
#include "window.h"
#include "button.h"

class Keyboard : public Window, public InteractiveWidget {

public:

    constexpr static unsigned NUM_ROWS = 5;
    constexpr static unsigned NUM_KEYS[NUM_ROWS] = {
        10,
        10,
        9,
        7,
        2
    };
    constexpr static unsigned NUM_KEYS_TOTAL = 38;
    static_assert(NUM_KEYS[0] + NUM_KEYS[1] + NUM_KEYS[2] + NUM_KEYS[3] + NUM_KEYS[4] == NUM_KEYS_TOTAL);
    static_assert(sizeof(NUM_KEYS) / sizeof(NUM_KEYS[0]) == NUM_ROWS);

    constexpr static const char *KEYBOARD_KEYS[3][5][10] {
        {
            {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
            {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
            {"a", "s", "d", "f", "g", "h", "j", "k", "l"},
                {"z", "x", "c", "v", "b", "n", "m"},
            {",", "."},
        },
        {
            {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
            {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
            {"A", "S", "D", "F", "G", "H", "J", "K", "L"},
                {"Z", "X", "C", "V", "B", "N", "M"},
            {",", "."},
        },
        {
            {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
            {"+", "=", "/", "_", "<", ">", "[", "]", "{", "}"},
            {"@", "#", "$", "%", "^", "&", "*", "(", ")"},
                {"-", "'", "\"", ":", ";", "!", "?"},
            {",", "."},
        }
    };

    struct keyboard_state_t {
        uint16_t key;
        union {
            struct {
                uint8_t backspace: 1;
                uint8_t shift: 1;
                uint8_t exit: 1;
            };
            uint8_t special;
        };
    };

protected:

    constexpr static unsigned DEBOUNCE_THRESH = 200;

    bool pressed {false};
    bool enabled {true};

    Button *exit {nullptr};
    Button *shift {nullptr};
    Button *space {nullptr};
    Button *backspace {nullptr};

    Button *keys[NUM_KEYS_TOTAL];

    unsigned shift_i {0};

    keyboard_state_t press_info {};
    keyboard_state_t release_info {};

    callback_t on_press {nullptr};
    callback_t on_release {nullptr};

    RingQueueInterface<callback_event_t> *event_queue {nullptr};

    unsigned *args {nullptr};

    unsigned last_press_epoch {0};

public:

    static Keyboard *create(Frame *parent);

    Keyboard *set_shift_index(unsigned new_shift_i);
    unsigned get_shift_index() const;

    bool propagate_press(unsigned x, unsigned y) override;
    bool propagate_release(unsigned x, unsigned y) override;

    // InteractiveWidget overrides

    Keyboard *set_onpress(callback_t callback) override;
    Keyboard *reset_onpress() override;

    Keyboard *set_onrelease(callback_t callback) override;
    Keyboard *reset_onrelease() override;

    Keyboard *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) override;
    Keyboard *reset_event_queue() override;

    Keyboard *set_args(unsigned *new_args) override;
    Keyboard *reset_args() override;
    unsigned *get_args() override;

    bool is_pressed() const;

    Keyboard *set_interactable(bool new_state) override;
    bool get_interactable() const override;


protected:

    Keyboard(Frame *parent);

    void assign_keys();
};

#endif
