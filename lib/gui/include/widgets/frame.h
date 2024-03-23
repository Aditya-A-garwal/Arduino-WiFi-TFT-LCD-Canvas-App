/**
 * @file                    frame.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   Declares Frame interface, which must be implemented by all widgets that may are not terminal widgets in the widget-tree
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_FRAME_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_FRAME_H__

#include "widget.h"

/**
 * @brief                   Interface that should be implemented by all widgets that are not terminal-widgets, i.e. they have their own subtree of widgets
 *
 */
class Frame : public BasicWidget, public DrawableWidget {

public:

    /**
     * @brief               Add a child to this widget's list of children
     *
     * @note                The child widget will be positioned relative to this widget
     * @note                This method should not be called outside the child class
     *
     * @code{.cpp}
     *                      // Correct Usage
     *                      Button *button = Button::create(frame, ...); // Button::create calls Frame::add_child(button)
     *
     *                      // Incorrect Usage
     *                      Button *button = Button::create();
     *                      frame->add_child(button);
     * @endcode
     *
     * @param child         Reference to child that must be added
     * @return              Pointer to frame (allows chaining method calls)
     *
     */
    virtual Frame *add_child(BasicWidget *child) = 0;

    /**
     * @brief               Get the number of children that this widget currently has
     *
     * @return              Number of children this widget has
     *
     */
    virtual unsigned get_children_count() const = 0;


    /**
     * @brief               Send the widget to a lower Z-index (higher up)
     *
     * @warning             If `child` is not a child of this widget, then this function does nothing
     * @see                 Frame::send_back(BasicWidget*,unsigned)
     *
     * @param child         Reference to child whose needs to be moved up
     * @param amt           Number of positions by which to move the child up (set this to 0 to move the child all the way to the front)
     *
     * @return              Pointer to frame (allows chaining method calls)
     *
     */
    virtual Frame *send_front(BasicWidget *child, unsigned amt) = 0;

    /**
     * @brief               Send the widget to a higher Z-index (lower down)
     *
     * @warning             If `child` is not a child of this widget, then this function does nothing
     * @see                 Frame::send_front(BasicWidget*,unsigned)
     *
     * @param child         Reference to child whose needs to be moved up
     * @param amt           Number of positions by which to move the child down (passing 0 moves the child all the way back)
     *
     * @return              Pointer to frame (allows chaining method calls)
     *
     */
    virtual Frame *send_back(BasicWidget *child, unsigned amt) = 0;

    /**
     * #brief               Enqueue all dirty widgets (that need to be redrawn/cleared) in the frame's subtree from higher Z-index to lower
     *
     * @note                The traversal (and collection) must be from higher Z-index (further back) to lower (further front)
     * @note                This method should only be used from within the `App` class
     *
     * @param dirty_widgets Reference to queue onto which dirty widgets must be enqueued
     *
     */
    virtual void collect_dirty_widgets(RingQueueInterface<BasicWidget *> *dirty_widgets) = 0;

    /**
     * @brief               Enqueue all widgets in the frame's subtree that intersect with and lie above another widget
     *
     * @note                The traversal (and collection) must be from higher Z-index (further back) to lower (further front)
     * @note                This method should only be used from within the `App` class
     *
     * @param dirty         Reference to widget with whom the intersection condition is checked
     * @param child         The ancestor of `dirty` which is an immediate child of this frame
     * @param overlapping_widgets   Reference to queue onto which the overlapping widgets must be enqueued
     *
     */
    virtual void collect_overlapped_widgets(BasicWidget *dirty, BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) = 0; //! this method should only be used by the app/view

    /**
     * @brief               Enqueue all widgets in the frame's subtree that intersect with another widget
     *
     * @note                The traversal (and collection) must be from higher Z-index (further back) to lower (further front)
     * @note                This method should only be used from within the `App` class
     *
     * @param child         Reference to widget with whom the intersection condition is checked
     * @param overlapping_widgets   Reference to queue onto which the overlapping widgets must be enqueued
     *
     */
    virtual void collect_underlapped_widgets(BasicWidget *child, RingQueueInterface<BasicWidget *> *overlapping_widgets) = 0; //! this method should only be used by the app/view

    // BasicWidget overrides

    /**
     * @brief               Get whether the current widget is a frame or not (required because RTTI is disabled)
     *
     * @return true         This widget is a frame
     *
     */
    virtual bool is_frame() const override { return true; }

};

#endif