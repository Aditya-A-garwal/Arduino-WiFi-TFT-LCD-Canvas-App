/**
 * @file                    view.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   This file declares the `View` class, which is the immediate child of an app in the widget-tree and contains all other widgets
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_VIEW_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_VIEW_H__

#include "vector"

#include "frame.h"
#include "app.h"

/**
 * @brief                   Views are immediate children of the app and group related widgets into a UI that together perform a task
 *
 *                          A single view is displayed at a time (called the active-view in `App`)
 *
 *                          All events are passed from the app to the active-view, who passes it down further
 */
class View : public Frame {

    friend class App;

protected:

    /** Reference to `App` instance that owns this view */
    App *app {nullptr};
    /** Whether this view is dirty and needs to be re-drawn */
    bool dirty {false};

    /** List of children of the view */
    std::vector<BasicWidget *> children;

public:

    /**
     * @brief               Default constructor disabled (use the `create` method)\
     *
     */
    View() = delete;

    /**
     * @brief               Dynamically create a new view instance
     *
     * @param parent        The app instance that should own this view
     *
     * @return              A pointer to the view instance (nullptr if the creation fails)
     */
    static View *create(App *parent);

    // BasicWidget overrides

    /**
     * @brief               Get a reference to the app instance that owns this view
     *
     * @return              Reference to the parent of this widget
     *
     */
    App *get_parent() override;

    /**
     * @brief               Get the X-coordinate of this widget within its parent's bounding box (offset from parent's left-edge)
     *
     * @return              0 (the view covers the entire screen)
     *
     */
    unsigned get_x() const override;

    /**
     * @brief               Get the Y-coordinate of this widget within its parent's bounding box (offset from parent's top-edge)
     *
     * @return              0 (the view covers the entire screen)
     *
     */
    unsigned get_y() const override;

    /**
     * @brief               Get the X-coordinate of this widget on the display (offset from screen's left-edge)
     *
     * @return              0 (the view covers the entire screen)
     *
     */
    unsigned get_absolute_x() const override;

    /**
     * @brief               Get the X-coordinate of this widget on the display (offset from screen's left-edge)
     *
     * @return              0 (the view covers the entire screen)
     *
     */
    unsigned get_absolute_y() const override;

    /**
     * @brief               Get the number of columns in this widget's bounding box
     *
     * @return              Width of the app
     *
     */
    unsigned get_width() const override;

    /**
     * @brief               Get the number of rows in this widget's bounding box
     *
     * @return              Height of the app
     *
     */
    unsigned get_height() const override;

    /**
     * @brief               Report if this view is dirty and needs redrawing
     *
     * @note                A view only becomes dirty when it is swapped-in to become the active-view, and the app is responsible for setting this
     *
     * @return false        If the view is not dirty
     * @return true         If the view is dirty
     *
     */
    bool get_dirty() const override;

    /**
     * @brief               Report if this widget's visibility was changed and it requires to be cleared/redrawn
     *
     * @note                See also `View::get_dirty`
     *
     * @return false        Always
     *
     */
    bool get_visibility_changed() const override;

    /**
     * @brief               Change the widget's status to dirty (requiring redrawing), irrespective of its current state
     *
     * @note                This method should only be used within the `App` class
     *
     */
    void set_dirty() override;

    /**
     * @brief               Change the widget's status to having its visibility changed (required clearing/redrawing), irrespective of its current state
     *
     * @note                This method does not do anything when called on a view
     *
     */
    void set_visibility_changed() override;

    /**
     * @brief               Recursively draw the children of the view
     *
     */
    void draw() override;

    /**
     * @brief               Clear the screen
     *
     */
    void clear() override;

    /**
     * @brief               Check whether a point intersects with this widget's bounding box
     *
     * @note                The coordinates of the point must be relative to its parent, not the screen
     *
     * @param x             X-coordinate of point (offset from parent's left-edge)
     * @param y             Y-coordiante of point (offset from parents' top-edge)
     *
     * @return true         Always
     *
     */
    bool get_intersection(unsigned x, unsigned y) const override;

    /**
     * @brief               Check whether another widget's bounding box intersects with this widget's bounding box
     *
     * @param other         Reference to other widget
     *
     * @return true         Always
     *
     */
    bool get_intersection(BasicWidget *other) const override;

    /**
     * @brief               Propagate a press event down this widget's subtree, consuming it
     *
     * @note                The coordinates of the press event must be relative to its parent, not necessarily the screen
     * @note                This widget may consume the event itself (rather than passing it down) depending on its location and nature
     *
     * @param x             X-coordinate of the press event (offset from parent's left-edge)
     * @param y             Y-coordinate of the press event (offset from parent's top-edge)
     *
     * @return true         Always
     *
     */
    bool propagate_press(unsigned x, unsigned y) override;

    /**
     * @brief               Propagate a release event down this widget's subtree, consuming it
     *
     * @note                The coordinates of the release event must be relative to its parent, not necessarily the screen
     * @note                This widget may consume the event itself (rather than passing it down) depending on its location and nature
     *
     * @param x             X-coordinate of the release event(offset from parent's left-edge)
     * @param y             Y-coordinate of the release event(offset from parent's top-edge)
     *
     * @return true         Always
     *
     */
    bool propagate_release(unsigned x, unsigned y) override;

    /**
     * @brief               Report the current visibility of the widget
     *
     * @return true         Always
     *
     */
    bool get_visibility() const override;

    /**
     * @brief               Set the widget's visibility
     *
     * @note                This method does nothing when called on the app
     *
     * @param new_visibility Whether the widget is hidden (false) or visible (true)
     *
     */
    void set_visibility(bool new_visibility) override;

    // Frame overrides

    /**
     * @brief               Set the color of a pixel at a point
     *

     * @param x             X-coordinate of the pixel (offset from left-edge)
     * @param y             Y-coordiante of the pixel (offset from top-edge)
     * @param color         16-bit color of the pixel
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *set_at(unsigned x, unsigned y, uint16_t color) override;

    /**
     * @brief               Get the color of a pixel at a point
     *
     * @param x             X-coordinate of the pixel (offset from left-edge)
     * @param y             Y-coordinate of the pixel (offset from top-edge)
     *
     * @return uint16_t     16-bit color at the point
     *
     */
    uint16_t get_at(unsigned x, unsigned y) const override;

    /**
     * @brief               Draw a line between two points
     *
     *
     * @param x0            X-coordinate of the first point (offset from left-edge)
     * @param y0            Y-coordinate of the first point (offset from top-edge)
     * @param x1            X-coordinate of the second point (offset from left-edge)
     * @param y1            Y-coordinate of the second point (offset from top-edge)
     * @param color         16-bit color of the line
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *draw_line(unsigned x0, unsigned y0, unsigned x1, unsigned y1, uint16_t color) override;

    /**
     * @brief               Draw an empty rectangle
     *
     * @param x             X-coordinate of the top-left corner (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner (offset from top-edge)
     * @param w             Width of the rectangle
     * @param h             Height of the rectangle
     * @param color         16-bit color of the rectangle
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *draw_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) override;

    /**
     * @brief               Draw a rectangle and fill it
     *
     * @param x             X-coordinate of the top-left corner (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner (offset from top-edge)
     * @param w             Width of the rectangle
     * @param h             Height of the rectangle
     * @param color         16-bit color of the rectangle
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *fill_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) override;

    /**
     * @brief               Draw an empty rectangle with rounded corners
     *
     * @param x             X-coordinate of the top-left corner (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner (offset from top-edge)
     * @param w             Width of the rectangle
     * @param h             Height of the rectangle
     * @param r             Radius of the rounded-corners
     * @param color         16-bit color of the rectangle
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *draw_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;
    /**
     * @brief               Draw a rectangle with rounded corners and fill it
     *
     * @param x             X-coordinate of the top-left corner (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner (offset from top-edge)
     * @param w             Width of the rectangle
     * @param h             Height of the rectangle
     * @param r             Radius of the rounded-corners
     * @param color         16-bit color of the rectangle
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *fill_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;

    /**
     * @brief               Draw an empty circle
     *
     * @param x             X-coordinate of the center of the circle (offset from left-edge)
     * @param y             Y-coordiante of the center of the circle (offset from top-edge)
     * @param r             Radius of the circle
     * @param color         16-bit color of the circle
     *
     * @return              Pointer to the view (allows chaining method calls)
     */
    View *draw_circle(unsigned x, unsigned y, unsigned r, uint16_t color) override;

    /**
     * @brief               Draw a circle and fill it
     *
     * @param x             X-coordinate of the center of the circle (offset from left-edge)
     * @param y             Y-coordiante of the center of the circle (offset from top-edge)
     * @param r             Radius of the circle
     * @param color         16-bit color of the circle
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *fill_circle(unsigned x, unsigned y, unsigned r, uint16_t color) override;

    /**
     * @brief               Get the bounding box of a string when it is draw on the parent
     *
     * @param text          String to draw
     * @param text_size     Size of text
     * @param x             X-coordinate of the top-left corner of the textbox (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner of the textbox (offset from top-edge)
     * @param x1            Reference to variable where the top-left corner of the textbox will be stored
     * @param x1            Reference to variable where the top-left corner of the textbox will be stored
     * @param w             Reference to variable where the width of the textbox will be stored
     * @param h             Reference to variable where the height of the textbox will be stored
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *get_text_bounds(const char *text, unsigned text_size, unsigned x, unsigned y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) override;

    /**
     * @brief               Set the font face of the text that will be drawn
     *
     * @param f             Pointer to font which should be used henceforth
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *set_font(const GFXfont *f) override;

    /**
     * @brief               Print a string with a transparent background
     *
     * @param text          String to print
     * @param x             X-coordinate of the bottom-left corner of the text-box (offset from left-edge)
     * @param x             Y-coordinate of the bottom-left corner of the text-box (offset from top-edge)
     * @param text_size     Size of the text
     * @param fg_color      Color of the text
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *print(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color) override;

    /**
     * @brief               Print a string with a transparent background
     *
     * @deprecated          This method has been deprecated. Fill a rectangle behind the text to make it look opaque
     *
     * @param text          String to print
     * @param x             X-coordinate of the bottom-left corner of the text-box (offset from left-edge)
     * @param x             Y-coordinate of the bottom-left corner of the text-box (offset from top-edge)
     * @param text_size     Size of the text
     * @param fg_color      Color of the text
     * @param bg_color      Color of the background
     *
     * @return              Pointer to the view (allows chaining method calls)
     *
     */
    View *print_opaque(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color, uint16_t bg_color) override;

    /**
     * @brief               Draw an RGB Bitmap
     *
     * @param x             X-coordinate of the top-left corner of the bitmap (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner of the bitmap (offset from top-edge)
     * @param data          Pointer to array where the bitmap is stored
     * @param width         Number of columns in the bitmap
     * @param height        Number of rows in the bitmap
     *
     * @return              Pointer to the view (allows chaining method calls)
     */
    View *draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height) override;

    /**
     * @brief               Add a child to this view
     *
     * @note                The child widget will be positioned relative to this widget
     * @note                This method should not be called outside the child class
     *
     * @code{.cpp}
     *                      // Correct Usage
     *                      Button *button = Button::create(view, ...); // Button::create calls Frame::add_child(button)
     *
     *                      // Incorrect Usage
     *                      Button *button = Button::create();
     *                      view->add_child(button);
     * @endcode
     *
     * @param child         Reference to child that must be added
     * @return              Pointer to view (allows chaining method calls)
     *
     */
    View *add_child(BasicWidget *child) override;

    /**
     * @brief               Get the number of children that this view currently has
     *
     * @return              Number of children this view has
     *
     */
    unsigned get_children_count() const override;

    /**
     * @brief               Send a widget to a lower Z-index (higher up)
     *
     * @warning             If `child` is not a child of this view, then this function does nothing
     * @see                 View::send_back(BasicWidget*,unsigned)
     *
     * @param child         Reference to child whose needs to be moved up
     * @param amt           Number of positions by which to move the child up (set this to 0 to move the child all the way to the front)
     *
     * @return              Pointer to view (allows chaining method calls)
     *
     */
    View *send_front(BasicWidget *child, unsigned amt) override;

    /**
     * @brief               Send a widget to a higher Z-index (lower down)
     *
     * @warning             If `child` is not a child of this view, then this function does nothing
     * @see                 View::send_front(BasicWidget*,unsigned)
     *
     * @param child         Reference to child whose needs to be moved up
     * @param amt           Number of positions by which to move the child down (passing 0 moves the child all the way back)
     *
     * @return              Pointer to view (allows chaining method calls)
     *
     */
    View *send_back(BasicWidget *child, unsigned amt) override;

    /**
     * #brief               Enqueue all dirty widgets (that need to be redrawn/cleared) in the view's subtree from higher Z-index to lower
     *
     * @note                The traversal (and collection) must be from higher Z-index (further back) to lower (further front)
     * @note                This method should only be used from within the `App` class
     *
     * @param dirty_widgets Reference to queue onto which dirty widgets must be enqueued
     *
     */
    void collect_dirty_widgets(RingQueueInterface<BasicWidget *> *dirty_widgets) override;

    /**
     * @brief               Enqueue all widgets in the view's subtree that intersect with and lie above another widget
     *
     * @note                The traversal (and collection) must be from higher Z-index (further back) to lower (further front)
     * @note                This method should only be used from within the `App` class
     *
     * @param dirty         Reference to widget with whom the intersection condition is checked
     * @param child         The ancestor of `dirty` which is an immediate child of this frame
     * @param overlapping_widgets   Reference to queue onto which the overlapping widgets must be enqueued
     *
     */
    void collect_overlapped_widgets(BasicWidget *dirty, BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) override;

    /**
     * @brief               Enqueue all widgets in the view's subtree that intersect with another widget
     *
     * @note                The traversal (and collection) must be from higher Z-index (further back) to lower (further front)
     * @note                This method should only be used from within the `App` class
     *
     * @param child         Reference to widget with whom the intersection condition is checked
     * @param overlapping_widgets   Reference to queue onto which the overlapping widgets must be enqueued
     *
     */
    void collect_underlapped_widgets(BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) override;

protected:

    /**
     * @brief               Construct a new View object
     *
     * @param parent        Reference to the parent app
     *
     */
    View(App *parent);
};

#endif
