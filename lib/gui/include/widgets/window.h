/**
 * @file                    window.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   This file declares the `Window` and 'WindowStyle' class, which provide a minimal and implementation of `Frame`
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_WINDOW_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_WINDOW_H__

#include "widget.h"
#include "frame.h"

#include "vector"
#include "algorithm"

/**
 * @brief                   Class to encapsulate style information of a window
 *
 */
class WindowStyle {

protected:

    constexpr static uint16_t DEFAULT_BG_COLOR = BLACK;
    constexpr static uint16_t DEFAULT_BORDER_COLOR = WHITE;

    constexpr static unsigned DEFAULT_BORDER_WIDTH = 1;
    constexpr static unsigned DEFAULT_BORDER_RADIUS = 0;

public:

    /** Color of window background */
    uint16_t bg_color {DEFAULT_BG_COLOR};
    /** Color of window border */
    uint16_t border_color {DEFAULT_BORDER_COLOR};

    /** Thickness of border */
    unsigned border_w {DEFAULT_BORDER_WIDTH};
    /** Radius of border (0 means not rounded( */
    unsigned border_radius {DEFAULT_BORDER_RADIUS};

    /** Sets the background color */
    WindowStyle *set_bg_color(uint16_t new_color);
    /** Gets the background color */
    uint16_t get_bg_color() const;

    /** Sets the border color */
    WindowStyle *set_border_color(uint16_t new_color);
    /** Gets the border color */
    uint16_t get_border_color() const;

    /** Sets the border thickness */
    WindowStyle *set_border_width(unsigned new_border_width);
    /** Gets the border thickness */
    unsigned get_border_width() const;

    /** Sets the border radius */
    WindowStyle *set_border_radius(unsigned new_border_radius);
    /** Gets the border radius */
    unsigned get_border_radius() const;
};

/**
 * @brief                   Class that provides a minimal and flexible implementation of `Frame` and also includes styling
 *
 */
class Window : public Frame {

protected:

    /** Reference to parent frame */
    Frame *parent {nullptr};

    /** Flag that indicates if the window is dirty or not */
    bool dirty {false};
    /** Flag that indicates if the window's visibility has been changed or not */
    bool visibility_changed {false};
    /** Flag to indicate if the window is hidden or visible */
    bool visible {true};

    /** X-coordinate of the window relative to its parent (offset from left-edge) */
    unsigned widget_x;
    /** Y-coordinate of the window relative to its parent (offset from top-edge) */
    unsigned widget_y;

    /** X-coordinate of the window relative to the display (offset from left-edge) */
    unsigned widget_absolute_x;
    /** Y-coordinate of the window relative to the display (offset from top-edge) */
    unsigned widget_absolute_y;

    /** Width of window (number of columns occupied) */
    unsigned widget_w;
    /** Height of window (number of rows occupied) */
    unsigned widget_h;

    /** List of children of the window */
    std::vector<BasicWidget *> children;

    /** Style information about the window */
    WindowStyle style;

public:

    /**
     * @brief               Default constructor disabled (use the `create` method)
     *
     */
    Window() = delete;

    /**
     * @brief               Dynamically create a new window instance
     *
     * @warning             This method returns a nullptr if a window instance could not be created
     *
     * @param parent        The frame that should own this window
     * @param x             X-coordinate of the window, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the window, within `parent` (offset from top-edge)
     * @param width         Number of columns occupied by the window
     * @param height        Number of rows occupied by the window
     *
     * @return              A pointer to the window instance (nullptr if the creation failed)
     *
     */
    static Window *create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);

    /**
     * @brief               Get a reference to the style object of the button to change its appearance
     *
     * @see                 `WindowStyle`
     *
     * @return              Pointer to the style object
     *
     */
    WindowStyle *get_style();

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

    // Frame overrides

    Window *set_at(unsigned x, unsigned y, uint16_t color) override;
    uint16_t get_at(unsigned x, unsigned y) const override;

    Window *draw_line(unsigned x0, unsigned y0, unsigned x1, unsigned y1, uint16_t color) override;

    Window *draw_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) override;
    Window *fill_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) override;

    Window *draw_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;
    Window *fill_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;

    Window *draw_circle(unsigned x, unsigned y, unsigned r, uint16_t color) override;
    Window *fill_circle(unsigned x, unsigned y, unsigned r, uint16_t codlor) override;

    Window *get_text_bounds(const char *text, unsigned text_size, unsigned x, unsigned y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) override;

    Window *set_font(const GFXfont *f);

    Window *print(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color) override;
    Window *print_opaque(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color, uint16_t bg_color) override;

    Window *draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height);

    Window *add_child(BasicWidget *child) override;
    unsigned get_children_count() const override;

    Window *send_front(BasicWidget *child, unsigned amt) override;
    Window *send_back(BasicWidget *child, unsigned amt) override;

    void collect_dirty_widgets(RingQueueInterface<BasicWidget *> *dirty_widgets) override;
    void collect_overlapped_widgets(BasicWidget *dirty, BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) override;
    void collect_underlapped_widgets(BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) override;


protected:

    /**
     * @brief               Construct a new Window object
     *
     * @param parent        The frame that should own this window
     * @param x             X-coordinate of the window, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the window, within `parent` (offset from top-edge)
     * @param width         Number of columns occupied by the window
     * @param height        Number of rows occupied by the window
     *
     */
    Window(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);
};

#endif
