/**
 * @file                    app.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   This file declares the `App` class, which is the top-level widget in the widget-tree
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_APP_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_APP_H__

#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

#include "vector"
#include "set"

#include "widget.h"
#include "frame.h"

class View;

/**
 * @brief                   The top-level widget in the widget tree and the only one with a reference to the display
 *
 *                          All events are passed from their originators to the app, who starts propagating it down the widget tree
 *
 *                          The app is responsible for collecting dirty widgets and redrawing them
 *
 *                          All drawing request are passed up the widget tree to the app, which then translates this to the display
 *
 */
class App : public BasicWidget, public DrawableWidget {

    friend class View;

protected:

#ifdef READ_PIXEL_ENABLED
    using display_t = MCUFRIEND_kbv;
#else
    using display_t = Adafruit_GFX;
#endif

    /** Reference to the display object */
    display_t *display {nullptr};

    /** The list of views that are owned by this app */
    std::vector<View *> views;

    /** Reference the view that is currently active */
    View *active_view {nullptr};

    /** Queue of events to execute (widgets have a reference to this queue ana post events to it) */
    RingQueue<InteractiveWidget::callback_event_t, 8> event_queue;

    /** Queue of dirty widgets to be re-drawn (a widget that is lower on the z-axis has a higher position in the queue) */
    RingQueue<BasicWidget *, 48> dirty_widgets;

public:

    /**
     * @brief               Default constructor disabled (use the `create` method)\
     *
     */
    App() = delete;

    /**
     * @brief               Dynamically create a new app instance
     *
     * @warning             This method returns a nullptr if an app instance could not be created
     *
     * @param display_ptr   Reference to the display object that the app uses
     *
     * @return App*         A pointer to the app instance (nullptr if the creation failed)
     *
     */
    static App *create(MCUFRIEND_kbv *display_ptr);

    /**
     * @brief               Set the active view
     *
     * @note                If the passed in view is not a child of the app instance, then this method call does nothing
     *
     * @param child         The view that must be made active (must be a child of this instance)
     *
     * @return              A pointer to the app (allows chaining method calls)
     *
     */
    App *make_active_view(View *child);

    /**
     * @brief               Get a reference to the active view
     *
     * @return              A pointer to the active view
     *
     */
    View *get_active_view() const;

    /**
     * @brief               Get a reference to the app's event queue
     *
     * @return              A pointer to the app's event queue
     *
     */
    RingQueueInterface<InteractiveWidget::callback_event_t> *get_event_queue();

    /**
     * @brief               Push an event onto the event queue
     *
     * @param event         Reference to the event that must be pushed to the event queue
     *
     * @return App*         A pointer to the app (allows chaining method calls)
     *
     */
    App *push_event(const InteractiveWidget::callback_event_t &event);

    /**
     * @brief               Execute all enqueued events and clear the queue
     *
     * @note                This must be called at every instance of the main loop
     *
     * @return App*         A pointer to the app (allows chaining method calls)
     *
     */
    App *execute_event_logic();

    /**
     * @brief               Collect all dirty widgets from the widget-tree, from lower Z-index to higher
     *
     * @return App*         A pointer to the app (allows chaining method calls)
     *
     */
    App *collect_dirty_widgets();

    /**
     * @brief               Update all enqueued dirty widgets (and recursively scan and update widgets that become dirty in the process)
     *
     * @return App*         A pointer to the app (allows chaining method calls)
     *
     */
    App *update_dirty_widgets();

    // BasicWidget overrides

    /**
     * @brief               Get the parent of this widget
     *
     * @return              nullptr (the app has no parent, it is the root of the widget-tree)
     *
     */
    BasicWidget *get_parent() override;

    /**
     * @brief               Get the X-coordinate of this widget within its parent's bounding box (offset from parent's left-edge)
     *
     * @return              0 (the app covers the entire screen)
     *
     */
    unsigned get_x() const override;

    /**
     * @brief               Get the Y-coordinate of this widget within its parent's bounding box (offset from parent's top-edge)
     *
     * @return              0 (the app covers the entire screen)
     *
     */
    unsigned get_y() const override;

    /**
     * @brief               Get the X-coordinate of this widget on the display (offset from screen's left-edge)
     *
     * @return              0 (the app covers the entire screen)
     *
     */
    unsigned get_absolute_x() const override;

    /**
     * @brief               Get the X-coordinate of this widget on the display (offset from screen's left-edge)
     *
     * @return              0 (the app covers the entire screen)
     *
     */
    unsigned get_absolute_y() const override;

    /**
     * @brief               Get the number of columns in this widget's bounding box
     *
     * @return              Width of the display
     *
     */
    unsigned get_width() const override;

    /**
     * @brief               Get the number of rows in this widget's bounding box
     *
     * @return              Height of the display
     *
     */
    unsigned get_height() const override;

    /**
     * @brief               Report if this widget is dirty and requires redrawing
     *
     * @note                The dirtyness of a wiget is not partial, i.e. it indicates that the whole widget must be re-drawn
     * @note                The widget can become dirty if its own state was updated or a widget overlapping with it has become dirty
     *
     * @return false        Always
     *
     */
    bool get_dirty() const override;

    /**
     * @brief               Report if this widget's visibility was changed and it requires to be cleared/redrawn
     *
     * @note                See also `BasicWidget::get_dirty`
     *
     * @return false        Always
     *
     */
    bool get_visibility_changed() const override;

    /**
     * @brief               Change the widget's status to dirty (requiring redrawing), irrespective of its current state
     *
     * @note                This method does not do anything when called on an app
     *
     */
    void set_dirty() override;

    /**
     * @brief               Change the widget's status to having its visibility changed (required clearing/redrawing), irrespective of its current state
     *
     * @note                This method does not do anything when called on an app
     *
     */
    void set_visibility_changed() override;

    /**
     * @brief               Draw the widget on its parent, and reset its dirty/visibility changed status
     *
     * @note                This method draws the active view when called on the app
     *
     */
    void draw() override;

    /**
     * @brief               Clear the widget on its parent, and reset its dirty/visibility changed status
     *
     * @note                This method clears the screen when called on the app
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
     * @return false        If the location of this press event does not lie within this widget
     * @return true         If the location of this press event lies within this widget
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
     * @return false        If the location of this release event does not lie within this widget
     * @return true         If the location of this release event lies within this widget
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

    // Drawable overrides

    /**
     * @brief               Set the color of a pixel at a point
     *

     * @param x             X-coordinate of the pixel (offset from left-edge)
     * @param y             Y-coordiante of the pixel (offset from top-edge)
     * @param color         16-bit color of the pixel
     *
     * @return              Pointer to the app (allows for chaining method calls)
     *
     */
    App *set_at(unsigned x, unsigned y, uint16_t color) override;

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
     * @return              Pointer to the widget (allows for chaining method calls)
     *
     */
    App *draw_line(unsigned x0, unsigned y0, unsigned x1, unsigned y1, uint16_t color) override;

    /**
     * @brief               Draw an empty rectangle
     *
     * @param x             X-coordinate of the top-left corner (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner (offset from top-edge)
     * @param w             Width of the rectangle
     * @param h             Height of the rectangle
     * @param color         16-bit color of the rectangle
     *
     * @return              Pointer to the widget (allows for chaining method calls)
     *
     */
    App *draw_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) override;

    /**
     * @brief               Draw a rectangle and fill it
     *
     * @param x             X-coordinate of the top-left corner (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner (offset from top-edge)
     * @param w             Width of the rectangle
     * @param h             Height of the rectangle
     * @param color         16-bit color of the rectangle
     *
     * @return              Pointer to the widget (allows for chaining method calls)
     *
     */
    App *fill_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) override;

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
     * @return              Pointer to the widget (allows for chaining method calls)
     *
     */
    App *draw_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;
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
     * @return              Pointer to the widget (allows for chaining method calls)
     *
     */
    App *fill_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) override;

    /**
     * @brief               Draw an empty circle
     *
     * @param x             X-coordinate of the center of the circle (offset from left-edge)
     * @param y             Y-coordiante of the center of the circle (offset from top-edge)
     * @param r             Radius of the circle
     * @param color         16-bit color of the circle
     *
     * @return              Pointer to the widget (allows for chaining method calls)
     */
    App *draw_circle(unsigned x, unsigned y, unsigned r, uint16_t color) override;

    /**
     * @brief               Draw a circle and fill it
     *
     * @param x             X-coordinate of the center of the circle (offset from left-edge)
     * @param y             Y-coordiante of the center of the circle (offset from top-edge)
     * @param r             Radius of the circle
     * @param color         16-bit color of the circle
     *
     * @return              Pointer to the widget (allows for chaining method calls)
     *
     */
    App *fill_circle(unsigned x, unsigned y, unsigned r, uint16_t color) override;

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
     * @return              Pointer to the widget (allows for chaining method calls)
     *
     */
    App *get_text_bounds(const char *text, unsigned text_size, unsigned x, unsigned y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) override;

    /**
     * @brief               Set the font face of the text that will be drawn
     *
     * @param f             Pointer to font which should be used henceforth
     *
     * @return              Pointer to the widget (allows for chaining method calls)
     *
     */
    App *set_font(const GFXfont *f) override;

    /**
     * @brief               Print a string with a transparent background
     *
     * @param text          String to print
     * @param x             X-coordinate of the bottom-left corner of the text-box (offset from left-edge)
     * @param x             Y-coordinate of the bottom-left corner of the text-box (offset from top-edge)
     * @param text_size     Size of the text
     * @param fg_color      Color of the text
     *
     * @return              Pointer to the widget (allows for chaining method calls)
     *
     */
    App *print(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color) override;

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
     * @return              Pointer to the widget (allows for chaining method calls)
     *
     */
    App *print_opaque(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color, uint16_t bg_color) override;

    /**
     * @brief               Draw an RGB Bitmap
     *
     * @param x             X-coordinate of the top-left corner of the bitmap (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner of the bitmap (offset from top-edge)
     * @param data          Pointer to array where the bitmap is stored
     * @param width         Number of columns in the bitmap
     * @param height        Number of rows in the bitmap
     *
     * @return              Pointer to the widget (allows for chaining method calls)
     */
    App *draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height) override;

protected:

    /**
     * @brief               Construct a new App object
     *
     * @param display       Reference to the display object that the app uses
     *
     */
    App(MCUFRIEND_kbv *display);

    /**
     * @brief               Add a view to the app
     *
     * @param child         The view which must be added to the app
     *
     * @return App*         Pointer to the app (allows chaining method calls)
     *
     */
    App *add_view(View *child);
};

#endif
