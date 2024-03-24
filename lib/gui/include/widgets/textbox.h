/**
 * @file                    label.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   File that declares the `TextBox` class, which is a simple and flexible implementation of a box to display text
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_TEXTBOX_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_TEXTBOX_H__

#include "label.h"

/**
 * @brief                   Class that provides a simple and flexible implementation of a box to display text, includes styling
 *
 */
class TextBox : public Label, public InteractiveWidget {

protected:

    /** The cooldown period between two presses (used to prevent "bouncing", where a single press is detected as multiple) */
    constexpr static unsigned DEBOUNCE_THRESH = 200;

    /** Flag to indicate if the bitmap is in a pressed state or not */
    bool pressed {false};
    /** Flag to indicate if interacting with the bitmap is enabled or disabled */
    bool enabled {true};

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
    TextBox() = delete;

    /**
     * @brief               Dynamically create a new textbox instance
     *
     * @warning             This method returns a nullptr if the textbox instance could not be created
     *
     * @param parent        The frame that should own this label
     * @param x             X-coordinate of the label, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the label, within `parent` (offset from top-edge)
     * @param width         Number of columns occupied by the label
     * @param height        Number of rows occupied by the label
     *
     * @return              A pointer to the label instance (nullptr if the creation fails)
     *
     */
    static TextBox *create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);

    // Label overrides

    bool propagate_press(unsigned x, unsigned y) override;
    bool propagate_release(unsigned x, unsigned y) override;

    // InteractiveWidget overrides

    TextBox *set_onpress(callback_t callback)  override;
    TextBox *reset_onpress()  override;

    TextBox *set_onrelease(callback_t callback)  override;
    TextBox *reset_onrelease()  override;

    TextBox *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) override;
    TextBox *reset_event_queue() override;

    TextBox *set_args(unsigned *new_args) override;
    TextBox *reset_args() override;
    unsigned *get_args() override;

    bool is_pressed() const override;

    TextBox *set_interactable(bool new_state) override;
    bool get_interactable() const override;

protected:

    /**
     * @brief               Construct a new Text Box object
     *
     * @param parent        The frame that should own this label
     * @param x             X-coordinate of the label, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the label, within `parent` (offset from top-edge)
     * @param width         Number of columns occupied by the label
     * @param height        Number of rows occupied by the label
     *
     */
    TextBox(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);
};

#endif
