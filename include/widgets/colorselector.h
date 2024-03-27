/**
 * @file                    colorselector.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   File that declares the `ColorSelector` class, which is used to create a grid of colors to pick from from
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_COLOR_SELECTOR_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_COLOR_SELECTOR_H__

#include "widgets/widget.h"
#include "widgets/frame.h"

/**
 * @brief                   Class that implements a widget which is used to create a grid of colors to pick from from
 *
 */
class ColorSelector : public BasicWidget, public InteractiveWidget {

protected:

    /** The cooldown period between two presses (used to prevent "bouncing", where a single press is detected as multiple) */
    constexpr static unsigned DEBOUNCE_THRESH = 200;

    constexpr static unsigned PAINT_RADIUS = 12;
    constexpr static unsigned H_PAD = 12;
    constexpr static unsigned V_PAD = 12;

    constexpr static unsigned WIDTH = (H_PAD * (3 + 1)) + (PAINT_RADIUS * (3 * 2));
    constexpr static unsigned HEIGHT = (V_PAD * (3 + 1)) + (PAINT_RADIUS * (3 * 2));

    /** Reference to parent frame */
    Frame *parent {nullptr};

    /** Flag that indicates if the bitmap is dirty or not */
    bool dirty {false};
    /** Flag that indicates if the bitmap's visibility has been changed or not */
    bool visibility_changed {false};
    /** Flag to indicate if the bitmap is hidden or visible */
    bool visible {true};
    /** Flag to indicate if the bitmap is in a pressed state or not */
    bool pressed {false};
    /** Flag to indicate if interacting with the bitmap is enabled or disabled */
    bool enabled {true};

    /** X-coordinate of the color selector relative to its parent (offset from left-edge) */
    unsigned widget_x;
    /** Y-coordinate of the color selector relative to its parent (offset from top-edge) */
    unsigned widget_y;

    /** X-coordinate of the color selector relative to the display (offset from left-edge) */
    unsigned widget_absolute_x;
    /** Y-coordinate of the color selector relative to the display (offset from top-edge) */
    unsigned widget_absolute_y;

    /** Array of 16-bit colors to show */
    uint16_t colors[9] {};

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
    ColorSelector() = delete;

    /**
     * @brief               Dynamically create a new color selector instance
     *
     * @warning             This method returns a nullptr if a color selector instance could not be created
     *
     * @param parent        The frame that should own this color selector
     * @param x             X-coordinate of the color selector, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the color selector, within `parent` (offset from top-edge)
     *
     * @return              A pointer to the color selector instance (nullptr if the creation fails)
     *
     */
    static ColorSelector *create(Frame *parent, unsigned x, unsigned y);

    /**
     * @brief               Set the color at a particular position
     *
     * @param pos           Position of the color (starting from the top-left and moving right and down)
     * @param new_color     16-bit color
     *
     * @return              Pointer to the color selector (allows chaining method calls)
     *
     */
    ColorSelector *set_color(unsigned pos, uint16_t new_color);

    /**
     * @brief               Get the color at a particular position
     *
     * @param pos           Position of the color (starting from the top-left and moving right and down)
     *
     * @return              16-bit color at the position
     *
     */
    uint16_t get_color(unsigned pos) const;

    // BasicWidget overrides

    Frame *get_parent() override;

    unsigned get_x() const override;
    unsigned get_y() const override;

    unsigned get_absolute_x() const override;
    unsigned get_absolute_y() const override;

    unsigned get_width() const override;
    unsigned get_height() const override;

    bool get_dirty() const override;
    bool get_visibility_changed() const override;

    void set_dirty() override;
    void set_visibility_changed() override;

    void draw() override;
    void clear() override;

    bool get_intersection(unsigned x, unsigned y) const override;
    bool get_intersection(BasicWidget *other) const override;

    bool propagate_press(unsigned x, unsigned y) override;
    bool propagate_release(unsigned x, unsigned y) override;

    bool get_visibility() const override;
    void set_visibility(bool new_visibility) override;

    // InteractiveWidget overrides

    ColorSelector *set_onpress(callback_t callback) override;
    ColorSelector *reset_onpress() override;

    ColorSelector *set_onrelease(callback_t callback) override;
    ColorSelector *reset_onrelease() override;

    ColorSelector *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) override;
    ColorSelector *reset_event_queue() override;

    ColorSelector *set_args(unsigned *new_args) override;
    ColorSelector *reset_args() override;
    unsigned *get_args() override;

    bool is_pressed() const override;

    ColorSelector *set_interactable(bool new_state) override;
    bool get_interactable() const override;

protected:

    /**
     * @brief               Construct a new Color Selector object
     *
     * @param parent        The frame that should own this color selector
     * @param x             X-coordinate of the color selector, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the color selector, within `parent` (offset from top-edge)
     *
     */
    ColorSelector(Frame *parent, unsigned x, unsigned y);
};

#endif
