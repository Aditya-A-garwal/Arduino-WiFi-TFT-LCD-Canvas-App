/**
 * @file                    label.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   File that declares the `Label` class, which is a simple and flexible implementation of a box to display readonly text
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_LABEL_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_LABEL_H__

#include "Arduino.h"

#include "widget.h"
#include "frame.h"

/**
 * @brief                   Class to encapsulate style information of a label
 *
 */
class LabelStyle {

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

    /** Color of label border */
    uint16_t border_color {DEFAULT_BORDER_COLOR};
    /** Color of label background */
    uint16_t bg_color {DEFAULT_BG_COLOR};
    /** Color of label text */
    uint16_t fg_color {DEFAULT_FG_COLOR};

    /** Sets the horizontal alignment of text */
    LabelStyle *set_horizontal_alignment(HorizontalAlignment new_h_align);
    /** Gets the horizontal alignment of text */
    HorizontalAlignment get_horizontal_alignment() const;

    /** Sets the vertical alignment of text */
    LabelStyle *set_vertical_alignment(VerticalAlignment new_v_align);
    /** Gets the vertical alignment of text */
    VerticalAlignment get_vertical_alignment() const;

    /** Sets the size of the text */
    LabelStyle *set_text_size(unsigned new_text_size);
    /** Gets the size of the text */
    unsigned get_text_size() const;

    /** Sets the border radius */
    LabelStyle *set_border_radius(unsigned new_border_radius);
    /** Gets the border radius */
    unsigned get_border_radius() const;

    /** Sets the border thickness */
    LabelStyle *set_border_width(unsigned new_border_w);
    /** Gets the border thickness */
    unsigned get_border_width() const;

    /** Sets the border color */
    LabelStyle *set_border_color(uint16_t new_border_color);
    /** Gets the border color */
    uint16_t get_border_color() const;

    /** Sets the text color */
    LabelStyle *set_fg_color(uint16_t new_fg_color);
    /** Gets the text color */
    uint16_t get_fg_color() const;

    /** Sets the background color */
    LabelStyle *set_bg_color(uint16_t new_bg_color);
    /** Gets the background color */
    uint16_t get_bg_color() const;
};

/**
 * @brief                   Class that provides a simple and flexible implementation of a box to display readonly text, includes styling
 *
 */
class Label : public BasicWidget {

protected:

    /** Reference to parent frame */
    Frame *parent {nullptr};

    /** Flag that indicates if the bitmap is dirty or not */
    bool dirty {false};
    /** Flag that indicates if the bitmap's visibility has been changed or not */
    bool visibility_changed {false};
    /** Flag to indicate if the bitmap is hidden or visible */
    bool visible {true};

    /** X-coordinate of the label relative to its parent (offset from left-edge) */
    unsigned widget_x;
    /** Y-coordinate of the label relative to its parent (offset from top-edge) */
    unsigned widget_y;

    /** X-coordinate of the label relative to the display (offset from left-edge) */
    unsigned widget_absolute_x;
    /** Y-coordinate of the label relative to the display (offset from top-edge) */
    unsigned widget_absolute_y;

    /** Width of label (number of columns occupied) */
    unsigned widget_w;
    /** Height of label (number of rows occupied) */
    unsigned widget_h;

    /** Text to display on the label */
    String message {};

    /** Style information about the label */
    LabelStyle style;

public:

    /**
     * @brief               Default constructor disabled (use the `create` method)
     *
     */
    Label() = delete;

    /**
     * @brief               Dynamically create a new label instance
     *
     * @warning             This method returns a nullptr if the label instance could not be created
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
    static Label *create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);

    /**
     * @brief               Set the text that the label should display
     *
     * @param msg_ptr       Pointer to message string
     *
     * @return              Pointer to the button (allows chaining method calls)
     *
     */
    Label *set_message(const char msg_ptr[]);

    /**
     * @brief               Get the text that is on the label
     *
     * @return              An owned copy of the text
     *
     */
    String get_message() const;

    /**
     * @brief               Get the number of characters in the text
     *
     * @return              Number of characters in the text shown by the label
     *
     */
    unsigned get_message_len() const;

    /**
     * @brief               Append a character to the text on the label
     *
     * @param ch            Character to append
     *
     * @return              Pointer to the label (allows chaining method calls)
     *
     */
    Label *append_to_message(char ch);

    /**
     * @brief               Pop the last character from the text on the label
     *
     * @return              Pointer to the label (allows chaining method calls)
     */
    Label *pop_from_message();

    /**
     * @brief               Get a reference to the style object of the label to change its appearance
     *
     * @see                 `LabelStyle`
     *
     * @return              Pointer to the style object
     *
     */
    LabelStyle *get_style();

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

protected:

    /**
     * @brief               Construct a new Label object
     *
     * @param parent        The frame that should own this label
     * @param x             X-coordinate of the label, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the label, within `parent` (offset from top-edge)
     * @param width         Number of columns occupied by the label
     * @param height        Number of rows occupied by the label
     *
     */
    Label(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);

    /**
     * @brief               Helper function to render the text
     *
     */
    void render_text();
};

#endif
