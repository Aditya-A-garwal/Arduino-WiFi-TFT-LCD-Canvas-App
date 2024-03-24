/**
 * @file                    keyboard.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   File that declares the `Keyboard` class, which provides a smartphone-like keyboard
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_KEYBOARD_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_KEYBOARD_H__

#include "widget.h"
#include "window.h"
#include "button.h"

/**
 * @brief                   Class that provides a smartphone-like keyboard
 *
 */
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

    /**
     * @brief               Structure to represent information about a single press/release event in callbacks
     *
     */
    struct keyboard_state_t {
        /** ASCII code of key */
        uint16_t key;
        union {
            struct {
                /** Flag to indicate if backspace was pressed */
                uint8_t backspace: 1;
                /** Flag to indicate if shift was pressed */
                uint8_t shift: 1;
                /** Flag to indicate if exit was pressed */
                uint8_t exit: 1;
            };
            /** Flag to indicate if the key was backspace/shift/exit or a regular character */
            uint8_t special;
        };
    };

protected:

    /** The cooldown period between two presses (used to prevent "bouncing", where a single press is detected as multiple) */
    constexpr static unsigned DEBOUNCE_THRESH = 200;

    /** Flag to indicate if the bitmap is in a pressed state or not */
    bool pressed {false};
    /** Flag to indicate if interacting with the bitmap is enabled or disabled */
    bool enabled {true};

    /** Exit key (used to minimize keyboard) */
    Button *exit {nullptr};
    /** Shift Key (used to switch between keys) */
    Button *shift {nullptr};
    /** Space Bar */
    Button *space {nullptr};
    /** Backspace Key */
    Button *backspace {nullptr};

    /** Array of regular character keys */
    Button *keys[NUM_KEYS_TOTAL];

    /** Current shift value (between 0 and 2 inclusive) */
    unsigned shift_i {0};

    /** Information about the latest press event (a reference to this is passed to the callback) */
    keyboard_state_t press_info {};
    /** Information about the latest release event (a reference to this is passed to the callback) */
    keyboard_state_t release_info {};

    /** Function to call when the bitmap is pressed */
    callback_t on_press {nullptr};
    /** Function to call when the bitmap is released */
    callback_t on_release {nullptr};

    /** Pointer to arguments passed to callbacks */
    unsigned *args {nullptr};

    /** Reference to event queue for posting events */
    RingQueueInterface<callback_event_t> *event_queue {nullptr};

    /** The epoch when the widget was last pressed */
    unsigned last_press_epoch {0};

public:

    /**
     * @brief               Default constructor disabled (use the `create` method)
     *
     */
    Keyboard() = delete;

    /**
     * @brief               Dynamically create a new keyboard instance
     *
     * @warning             This method returns a nullptr if a keyboard instance could not be created
     *
     * @param parent        The frame that should own this keyboard
     *
     * @return              A pointer to the keyboard instance (nullptr if the creation fails)
     *
     */
    static Keyboard *create(Frame *parent);

    /**
     * @brief               Set the value of the shift index (determines what characters are being shown)
     *
     * @param new_shift_i   New value of shift index
     *
     * @return              Pointer to the keyboard instance (allows chaining method calls)
     *
     */
    Keyboard *set_shift_index(unsigned new_shift_i);

    /**
     * @brief               Get the value of the shift index (used to determine what characters are being shown)
     *
     * @return              Value of the shift index
     */
    unsigned get_shift_index() const;

    // Window overrides

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

    /**
     * @brief               Construct a new Keyboard object
     *
     * @param parent        The frame that should own this keyboard
     *
     */
    Keyboard(Frame *parent);

    /**
     * @brief               Update the keys based on the shift position
     *
     */
    void update_keys();
};

#endif
