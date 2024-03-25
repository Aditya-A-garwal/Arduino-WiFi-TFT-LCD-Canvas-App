/**
 * @file                    widget.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   Declares Interfaces to be implemented by different types of widgets for compatibility with the app
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_WIDGET_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_WIDGET_H__

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "constants.h"

#include "functional"
#include "queue"

#include "Fonts/PlusJakartaSans6pt7b.h" // size 1
#include "Fonts/PlusJakartaSans9pt7b.h" // size 2
#include "Fonts/PlusJakartaSans15pt7b.h" // size 3
#include "Fonts/PlusJakartaSans21pt7b.h" // size 4

/** If defined, functions pointers are used to store callbacks rather than functors */
#define FUNCTION_PTR_CALLBACK
/** If defined, then the get_at method of the app is valid (this is not possible using the Adafruit_GFX class, and requires the MCUFRIEND_kbv subclass) */
#define READ_PIXEL_ENABLED

/** Font to use for text size 1 */
#define font_1 (&PlusJakartaSans_SemiBold6pt7b)
/** Font to use for text size 2 */
#define font_2 (&PlusJakartaSans_SemiBold9pt7b)
/** Font to use for text size 3 */
#define font_3 (&PlusJakartaSans_SemiBold15pt7b)
/** Font to use for text size 4 */
#define font_4 (&PlusJakartaSans_SemiBold21pt7b)

/**
 * @brief                   Interface for a Ring Queue data structure (template)
 *
 *                          This template class defines the interface for a ring queue.
 *                          The key benefit is that objects can interact with a ring queue through this interface
 *                          without needing to know the specific size (capacity) of the queue implementation.
 *
 *                          This enables sharing ring queue references between objects while keeping the
 *                          internal storage details (like capacity) encapsulated within the concrete implementation.
 *
 * @tparam T                Type of elements stored by the data structure
 *
 */
template <typename T>
class RingQueueInterface {

public:

    /**
     * @brief               Push an element to the back of the queue
     *
     * @param e             Element to push onto the queue
     *
     */
    virtual void push(const T &e) = 0;

    /**
     * @brief               Get an immutable reference to the element at the front of the queue
     *
     * @return              Immutable reference to the element currently at the front of the queue
     *
     */
    virtual const T &front() = 0;

    /**
     * @brief               Pop an element from the front of the queue without returning it
     *
     */
    virtual void pop() = 0;

    /**
     * @brief               Get the number of elements in the queue
     *
     * @return unsigned     Number of elements in the queue
     */
    virtual unsigned get_size() const = 0;
};

/**
 * @brief                   Concrete implementation of a Ring Queue data structure using an array (template)
 *
 *                          This template class provides a concrete implementation of a ring queue using an array.
 *                          The actual capacity of the queue is determined at compile time using the template parameter `CAPACITY`.
 *
 *                          However, objects interacting with this queue only need to be aware of the data type (`T`) being stored.
 *                          They can use the provided interface methods (push, pop, front, get_size) to manipulate the queue
 *                          without knowing the specific size of the underlying array.
 *
 *                          This approach promotes loose coupling and improves code maintainability.
 *
 * @tparam T                Type of elements stored by the data structure
 * @tparam CAPACITY         Maximum number of elements that the data structure can hold at a time
 */
template <typename T, unsigned CAPACITY>
class RingQueue : public RingQueueInterface<T> {

protected:

    /** Array to hold elements */
    T arr[CAPACITY];

    /** Position of the back of the queue in `arr` array */
    unsigned head = 0;
    /** Position of the front of the queue in `arr` array */
    unsigned tail = 0;
    /** Number of elements in the queue */
    unsigned size = 0;

public:

    /**
     * @brief               Push an eleement to the back of the queue
     *
     *                      The queue uses the object's operator= function to store it within an internal array
     *
     * @warning             If this function was called when the queue was full, the queue will go into an invalid state
     *
     * @param e             Element to push to the back of the queue
     */
    void push(const T &e) override {
        arr[head] = e;
        (head += 1) %= CAPACITY;
        ++size;
    }

    /**
     * @brief               Get an immutable reference to the element at the front of the queue
     *
     * @warning             If this function was called when the queue was empty, it will return an invalid reference
     * @warning             The reference returned by this method becomes invalid after calling the `pop` method
     *
     * @return              Immutable reference to the element currently at the front of the queue
     */
    const T &front() override {
        return arr[tail];
    }

    /**
     * @brief               Pop an element from the front of the queue without returning it
     *
     * @warning             Calling this function invalidates the reference returned by the latest call to the `front` method
     * @warning             This method does not perform any garbage collection for the popped object
     *
     */
    void pop() override {
        (tail += 1) %= CAPACITY;
        --size;
    }

    /**
     * @brief               Get the number of elements present in the queue
     *
     * @return unsigned     Number of elements in the queue
     *
     */
    unsigned get_size() const override {
        return size;
    }
};

/**
 * @brief                   Interface that should be implemented by all widgets
 *
 *                          Declares the necessary behaviour for all widgets. Implementing this widget is required
 *                          for compatibility with the `App` class,
 *
 */
class BasicWidget {

public:

    /**
     * @brief               Get a reference to the parent of this widget
     *
     * @return              Reference to the parent of this widget
     *
     */
    virtual BasicWidget *get_parent() = 0;

    /**
     * @brief               Get the X-coordinate of this widget within its parent's bounding box (offset from parent's left-edge)
     *
     * @return              X-coordinate of this widget within its parent
     *
     */
    virtual unsigned get_x() const = 0;

    /**
     * @brief               Get the Y-coordinate of this widget within its parent's bounding box (offset from parent's top-edge)
     *
     * @return              Y-coordinate of this widget within its parent
     *
     */
    virtual unsigned get_y() const = 0;

    /**
     * @brief               Get the X-coordinate of this widget on the display (offset from screen's left-edge)
     *
     * @return              X-coordinate of this widget on the display
     *
     */
    virtual unsigned get_absolute_x() const = 0;

    /**
     * @brief               Get the Y-coordinate of this widget on the display (offset from screen's top-edge)
     *
     * @return              Y-coordinate of this widget on the display
     *
     */
    virtual unsigned get_absolute_y() const = 0;

    /**
     * @brief               Get the number of columns in this widget's bounding box
     *
     * @return              Number of columns in this widget's bounding box
     *
     */
    virtual unsigned get_width() const = 0;

    /**
     * @brief               Get the number of rows in this widget's bounding box
     *
     * @return              Number of rows in this widget's bounding box
     *
     */
    virtual unsigned get_height() const = 0;

    /**
     * @brief               Report if this widget is dirty and requires redrawing
     *
     * @note                The dirtyness of a wiget is not partial, i.e. it indicates that the whole widget must be re-drawn
     * @note                The widget can become dirty if its own state was updated or a widget overlapping with it has become dirty
     *
     * @return false        If the widget does not need to be redrawn
     * @return true         If the widget needs to be redraw
     *
     */
    virtual bool get_dirty() const = 0;

    /**
     * @brief               Report if this widget's visibility was changed and it requires to be cleared/redrawn
     *
     * @note                See also `BasicWidget::get_dirty`
     *
     * @return false        If the widget's visibility was not changed
     * @return true         If the widget's visibility was changed
     *
     */
    virtual bool get_visibility_changed() const = 0;

    /**
     * @brief               Change the widget's status to dirty (requiring redrawing), irrespective of its current state
     *
     */
    virtual void set_dirty() = 0;

    /**
     * @brief               Change the widget's status to having its visibility changed (required clearing/redrawing), irrespective of its current state
     *
     */
    virtual void set_visibility_changed() = 0;

    /**
     * @brief               Draw the widget on its parent, and reset its dirty/visibility changed status
     *
     */
    virtual void draw() = 0;

    /**
     * @brief               Clear the widget on its parent, and reset its dirty/visibility changed status
     *
     */
    virtual void clear() = 0;

    /**
     * @brief               Check whether a point intersects with this widget's bounding box
     *
     * @note                The coordinates of the point must be relative to its parent, not the screen
     *
     * @param x             X-coordinate of point (offset from parent's left-edge)
     * @param y             Y-coordiante of point (offset from parents' top-edge)
     *
     * @return false        If the point intersects with the widget's bounding box
     * @return true         If the point does not intersect with the widget's bounding box
     *
     */
    virtual bool get_intersection(unsigned x, unsigned y) const = 0;

    /**
     * @brief               Check whether another widget's bounding box intersects with this widget's bounding box
     *
     * @param other         Reference to other widget
     *
     * @return false        If the other widget's bounding box does not partially or completely intersect with this widget's bounding box
     * @return true         If atleast one of the corners of the other widget's bounding box lies in this widget's bounding box or vice-versa
     *
     */
    virtual bool get_intersection(BasicWidget *other) const = 0;

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
    virtual bool propagate_press(unsigned x, unsigned y) = 0;

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
    virtual bool propagate_release(unsigned x, unsigned y) = 0;

    /**
     * @brief               Report the current visibility of the widget
     *
     * @return false        If the widget is set to be hidden
     * @return true         If the widget is set to be visibile
     *
     */
    virtual bool get_visibility() const = 0;

    /**
     * @brief               Set the widget's visibility
     *
     * @param new_visibility Whether the widget is hidden (false) or visible (true)
     *
     */
    virtual void set_visibility(bool new_visibility) = 0;

    /**
     * @brief               Is the widget a frame or not
     *
     * @return              Whether the widget is a frame or not
     */
    virtual bool is_frame() const { return false; }
};

/**
 * @brief                   Interface that should be implemented by all widgets that must report when they are interacted with (pressed/released)
 *
 *                          Declares the necessary behaviour for registering/de-registering callbacks and arguments
 *
 */
class InteractiveWidget {

public:

#ifdef FUNCTION_PTR_CALLBACK
    /** The data-type of the callback, either a functor or a function pointer (depending on if `FUNCTION_PTR_CALLBACK` macro is defined or not)*/
    using callback_t = void (*)(unsigned *);
#else
    /** The data-type of the callback, either a functor or a function pointer (depending on if `FUNCTION_PTR_CALLBACK` macro is defined or not)*/
    using callback_t = std::function<void (unsigned *)>;
#endif

    /** Structure that holds the details necessary to execute a single-event, namely a pointer to the callback and a pointer to the arguments */
    struct callback_event_t {
        callback_t cb;
        unsigned *args;
    };

    /**
     * @brief               Set the function to be called when the widget is pressed
     *
     * @param cb            Reference to the callback function
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual InteractiveWidget *set_onpress(callback_t new_onpress) = 0;

    /**
     * @brief               Reset the function to be called when the widget is pressed
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual InteractiveWidget *reset_onpress() = 0;

    /**
     * @brief               Set the function to be called when the widget is released
     *
     * @param cb            Reference to the callback function
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual InteractiveWidget *set_onrelease(callback_t new_onrelease) = 0;

    /**
     * @brief               Reset the function to be called when the widget is released
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual InteractiveWidget *reset_onrelease() = 0;

    /**
     * @brief               Set the queue on which the widget can enqueue events
     *
     * @param new_event_queue   Mutable reference to queue which should be used
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual InteractiveWidget *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) = 0;

    /**
     * @brief               Reset the event queue so that the widget may not enqueue any events
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual InteractiveWidget *reset_event_queue() = 0;

    /**
     * @brief               Set the reference to the arguments which must be passed to callbacks
     *
     * @param new_args      Reference to the new arguments
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual InteractiveWidget *set_args(unsigned *new_args) = 0;

    /**
     * @brief               Remove the reference to the arguments which are passed to callbacks
     *
     * @warning             This method does not explicitly perform any garbage collection for the old arguments
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
    */
    virtual InteractiveWidget *reset_args() = 0;

    /**
     * @brief               Get a reference to the arguments which are passed to callbacks (as set be calls to `set_args`)
     *
     * @return              Pointer to arguments
     *
     */
    virtual unsigned *get_args() = 0;

    /**
     * @brief               Report whether the widget is currently being pressed or not
     *
     * @return false        If the widget is not currently being pressed
     * @return true         If the widget is currently being pressed
     *
     */
    virtual bool is_pressed() const = 0;

    /**
     * @brief               Set whether the widget should ignore press and release events (or not)
     *
     * @param new_state     Whether the widget should ignore press and release events (or not)
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual InteractiveWidget *set_interactable(bool new_state) = 0;

    /**
     * @brief               Reports whether the widget is set to ignore (or not) press and release events (or not)
     *
     * @return false        If the widget ignores press and release events
     * @return true         If the widget does not ignore press and release events
     *
     */
    virtual bool get_interactable() const = 0;
};

/**
 * @brief                   Interface that must be implemented by all widgets onto which graphics can be drawn
 *
 *                          Declares the contract that all widgets must follow onto whom other widgets can draw themselves
 *
 */
class DrawableWidget {

public:

    /**
     * @brief               Set the color of a pixel at a point
     *

     * @param x             X-coordinate of the pixel (offset from left-edge)
     * @param y             Y-coordiante of the pixel (offset from top-edge)
     * @param color         16-bit color of the pixel
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *set_at(unsigned x, unsigned y, uint16_t color) = 0;

    /**
     * @brief               Get the color of a pixel at a point
     *
     * @param x             X-coordinate of the pixel (offset from left-edge)
     * @param y             Y-coordinate of the pixel (offset from top-edge)
     *
     * @return uint16_t     16-bit color at the point
     *
     */
    virtual uint16_t get_at(unsigned x, unsigned y) const = 0;

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
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *draw_line(unsigned x0, unsigned y0, unsigned x1, unsigned y1, uint16_t color) = 0;

    /**
     * @brief               Draw an empty rectangle
     *
     * @param x             X-coordinate of the top-left corner (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner (offset from top-edge)
     * @param w             Width of the rectangle
     * @param h             Height of the rectangle
     * @param color         16-bit color of the rectangle
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *draw_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) = 0;

    /**
     * @brief               Draw a rectangle and fill it
     *
     * @param x             X-coordinate of the top-left corner (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner (offset from top-edge)
     * @param w             Width of the rectangle
     * @param h             Height of the rectangle
     * @param color         16-bit color of the rectangle
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *fill_rect(unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color) = 0;

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
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *draw_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) = 0;
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
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *fill_round_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned r, uint16_t color) = 0;

    /**
     * @brief               Draw an empty circle
     *
     * @param x             X-coordinate of the center of the circle (offset from left-edge)
     * @param y             Y-coordiante of the center of the circle (offset from top-edge)
     * @param r             Radius of the circle
     * @param color         16-bit color of the circle
     *
     * @return              Pointer to the widget (allows chaining method calls)
     */
    virtual DrawableWidget *draw_circle(unsigned x, unsigned y, unsigned r, uint16_t color) = 0;

    /**
     * @brief               Draw a circle and fill it
     *
     * @param x             X-coordinate of the center of the circle (offset from left-edge)
     * @param y             Y-coordiante of the center of the circle (offset from top-edge)
     * @param r             Radius of the circle
     * @param color         16-bit color of the circle
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *fill_circle(unsigned x, unsigned y, unsigned r, uint16_t color) = 0;

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
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *get_text_bounds(const char *text, unsigned text_size, unsigned x, unsigned y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) = 0;

    /**
     * @brief               Set the font face of the text that will be drawn
     *
     * @param f             Pointer to font which should be used henceforth
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *set_font(const GFXfont *f) = 0;

    /**
     * @brief               Print a string with a transparent background
     *
     * @param text          String to print
     * @param x             X-coordinate of the bottom-left corner of the text-box (offset from left-edge)
     * @param x             Y-coordinate of the bottom-left corner of the text-box (offset from top-edge)
     * @param text_size     Size of the text
     * @param fg_color      Color of the text
     *
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *print(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color) = 0;

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
     * @return              Pointer to the widget (allows chaining method calls)
     *
     */
    virtual DrawableWidget *print_opaque(const char *text, unsigned x, unsigned y, unsigned text_size, uint16_t fg_color, uint16_t bg_color) = 0;

    /**
     * @brief               Draw an RGB Bitmap
     *
     * @param x             X-coordinate of the top-left corner of the bitmap (offset from left-edge)
     * @param y             Y-coordinate of the top-left corner of the bitmap (offset from top-edge)
     * @param data          Pointer to array where the bitmap is stored
     * @param width         Number of columns in the bitmap
     * @param height        Number of rows in the bitmap
     *
     * @return              Pointer to the widget (allows chaining method calls)
     */
    virtual DrawableWidget *draw_rgb_bitmap(unsigned x, unsigned y, const uint16_t *data, unsigned width, unsigned height) = 0;
};

#endif
