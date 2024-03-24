/**
 * @file                    button.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   File that declares the `Button` class, which is a simple and flexible implementation of a button
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_BUTTON_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_BUTTON_H__

#include "widget.h"
#include "frame.h"

/**
 * @brief                   Class to encapsulate style information of a button
 *
 */
class ButtonStyle {

protected:

    constexpr static unsigned DEFAULT_TEXT_SIZE = 3;

    constexpr static unsigned DEFAULT_BORDER_RADIUS = 4;
    constexpr static unsigned DEFAULT_BORDER_W = 2;
    constexpr static uint16_t DEFAULT_BORDER_COLOR = blend_color(WHITE, GRAY, 100);

    constexpr static uint16_t DEFAULT_BG_COLOR = BLACK;
    constexpr static uint16_t DEFAULT_FG_COLOR = WHITE;

public:

    /** Enumueration of different ways to align text horizontally */
    enum HorizontalAlignment {
        CENTER_ALIGN = 0,
        LEFT_ALIGN = 1,
        RIGHT_ALIGN = 2
    };

    /** Enumueration of different ways to align text vertically */
    enum VerticalAlignment {
        MIDDLE_ALIGN = 0,
        TOP_ALIGN = 1,
        BOTTOM_ALIGN = 2
    };

    /** Horizontal alignment of the text */
    HorizontalAlignment h_align {HorizontalAlignment::CENTER_ALIGN};
    /** Vertical alignment of the text */
    VerticalAlignment v_align {VerticalAlignment::MIDDLE_ALIGN};

    /** Size of the text */
    unsigned text_size {DEFAULT_TEXT_SIZE};

    /** Radius of border */
    unsigned border_radius {DEFAULT_BORDER_RADIUS};
    /** Thickness of border */
    unsigned border_w {DEFAULT_BORDER_W};

    /** Color of button border */
    uint16_t border_color {DEFAULT_BORDER_COLOR};
    /** Color of button background */
    uint16_t bg_color {DEFAULT_BG_COLOR};
    /** Color of button text */
    uint16_t fg_color {DEFAULT_FG_COLOR};

    /** Color of button border (while unpressed) */
    uint16_t pressed_bg_color {blend_color(DEFAULT_BG_COLOR, BLACK, 100)};
    /** Color of button background (while pressed) */
    uint16_t pressed_fg_color {blend_color(DEFAULT_FG_COLOR, BLACK, 100)};
    /** Color of button text (while pressed) */
    uint16_t pressed_border_color {blend_color(DEFAULT_BORDER_COLOR, BLACK, 100)};

    /** Sets the horizontal alignment of text */
    ButtonStyle *set_horizontal_alignment(HorizontalAlignment new_h_align);
    /** Gets the horizontal alignment of text */
    HorizontalAlignment get_horizontal_alignment() const;

    /** Sets the vertical alignment of text */
    ButtonStyle *set_vertical_alignment(VerticalAlignment new_v_align);
    /** Gets the vertical alignment of text */
    VerticalAlignment get_vertical_alignment() const;

    /** Sets the size of the text */
    ButtonStyle *set_text_size(unsigned new_text_size);
    /** Gets the size of the text */
    unsigned get_text_size() const;

    /** Sets the border radius */
    ButtonStyle *set_border_radius(unsigned new_border_radius);
    /** Gets the border radius */
    unsigned get_border_radius() const;

    /** Sets the border thickness */
    ButtonStyle *set_border_width(unsigned new_border_w);
    /** Gets the border thickness */
    unsigned get_border_width() const;

    /** Sets the border color */
    ButtonStyle *set_border_color(uint16_t new_border_color);
    /** Gets the border color */
    uint16_t get_border_color() const;

    /** Sets the text color */
    ButtonStyle *set_fg_color(uint16_t new_fg_color);
    /** Gets the text color */
    uint16_t get_fg_color() const;

    /** Sets the background color */
    ButtonStyle *set_bg_color(uint16_t new_bg_color);
    /** Gets the background color */
    uint16_t get_bg_color() const;

    /** Sets the border color (while pressed) */
    ButtonStyle *set_pressed_border_color(uint16_t new_pressed_border_color);
    /** Gets the border color (while pressed) */
    uint16_t get_pressed_border_color() const;

    /** Sets the background color (while pressed) */
    ButtonStyle *set_pressed_bg_color(uint16_t new_pressed_bg_color);
    /** Gets the background color (while pressed) */
    uint16_t get_pressed_bg_color() const;

    /** Sets the text color (while pressed) */
    ButtonStyle *set_pressed_fg_color(uint16_t new_pressed_fg_color);
    /** Gets the text color (while pressed) */
    uint16_t get_pressed_fg_color() const;
};

/**
 * @brief                   Class that provides a simple and flexible implementation of a button and includes styling
 *
 */
class Button : public BasicWidget, public InteractiveWidget {

protected:

    /** The cooldown period between two pressed (used to prevent "bouncing", where a single press is detected as multiple) */
    constexpr static unsigned DEBOUNCE_THRESH = 200;

    constexpr static unsigned DEFAULT_WIDTH = 64;
    constexpr static unsigned DEFAULT_HEIGHT = 32;

    constexpr static uint16_t DEFAULT_BG_COLOR = BLACK;
    constexpr static uint16_t DEFAULT_FG_COLOR = WHITE;

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

    /** X-coordinate of the button relative to its parent (offset from left-edge) */
    unsigned widget_x;
    /** Y-coordinate of the button relative to its parent (offset from top-edge) */
    unsigned widget_y;

    /** X-coordinate of the button relative to the display (offset from left-edge) */
    unsigned widget_absolute_x;
    /** Y-coordinate of the button relative to the display (offset from top-edge) */
    unsigned widget_absolute_y;

    /** Width of button (number of columns occupied) */
    unsigned widget_w;
    /** Height of button (number of rows occupied) */
    unsigned widget_h;

    /** Text to display on the button */
    String message {};

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

    /** Style information about the button */
    ButtonStyle style;

public:

    /**
     * @brief               Default constructor disabled (use the `create` method)
     *
     */
    Button() = delete;

    /**
     * @brief               Dynamically create a new button instance
     *
     * @warning             This method returns a nullptr if the button instance could not be created
     *
     * @param parent        The frame that should own this button
     * @param x             X-coordinate of the button, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the button, within `parent` (offset from top-edge)
     *
     * @return              A pointer to the button instance (nullptr if the creation fails)
     *
     */
    static Button *create(Frame *parent, unsigned x, unsigned y);

    /**
     * @brief               Dynamically create a new button instance
     *
     * @warning             This method returns a nullptr if the button instance could not be created
     *
     * @param parent        The frame that should own this button
     * @param x             X-coordinate of the button, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the button, within `parent` (offset from top-edge)
     * @param width         Number of columns occupied by the button
     * @param height        Number of rows occupied by the button
     *
     * @return              A pointer to the button instance (nullptr if the creation fails)
     *
     */
    static Button *create(Frame *parent, unsigned x, unsigned y, unsigned w, unsigned h);

    /**
     * @brief               Set the text that the button should display
     *
     * @param messagePtr    Pointer to message string
     *
     * @return              Pointer to the button (allows chaining method calls)
     *
     */
    Button *set_message(const char *messagePtr);

    /**
     * @brief               Get the text that the button is displaying
     *
     * @return              An owned copy of the text that the button is displaying
     *
     */
    String get_message() const;

    /**
     * @brief               Get a reference to the style object of the button to change its appearance
     *
     * @see                 `ButtonStyle`
     *
     * @return              Pointer to the style object
     *
     */
    ButtonStyle *get_style();

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

    Button *set_onpress(callback_t callback) override;
    Button *reset_onpress() override;

    Button *set_onrelease(callback_t callback) override;
    Button *reset_onrelease() override;

    Button *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) override;
    Button *reset_event_queue() override;

    Button *set_args(unsigned *new_args) override;
    Button *reset_args() override;
    unsigned *get_args() override;

    bool is_pressed() const override;

    Button *set_interactable(bool new_state) override;
    bool get_interactable() const override;

protected:

    /**
     * @brief               Construct a new Button object
     *
     * @param parent        The frame that should own this button
     * @param x             X-coordinate of the button, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the button, within `parent` (offset from top-edge)
     * @param width         Number of columns occupied by the button
     * @param height        Number of rows occupied by the button
     *
     */
    Button(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);

    /**
     * @brief               Helper function to render the text
     *
     */
    void render_text();
};

#endif
