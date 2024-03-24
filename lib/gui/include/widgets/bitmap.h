/**
 * @file                    bitmap.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   File that declares the `Bitmap` class, which is used to show 16-bit color bitmap images
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_BITMAP_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_BITMAP_H__

#include "widget.h"
#include "frame.h"

/**
 * @brief                   Class to show 16-bit color bitmaps stored in memory
 *
 */
class Bitmap : public BasicWidget, public InteractiveWidget {

protected:

    /** The cooldown period between two presses (used to prevent "bouncing", where a single press is detected as multiple) */
    constexpr static unsigned DEBOUNCE_THRESH = 200;

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

    /** X-coordinate of the bitmap relative to its parent (offset from left-edge) */
    unsigned widget_x;
    /** Y-coordinate of the bitmap relative to its parent (offset from top-edge) */
    unsigned widget_y;

    /** X-coordinate of the widget relative to the display (offset from left-edge) */
    unsigned widget_absolute_x;
    /** Y-coordinate of the widget relative to the display (offset from top-edge) */
    unsigned widget_absolute_y;

    /** Width of widget (number of columns occupied) */
    unsigned widget_w;
    /** Height of widget (number of rows occupied) */
    unsigned widget_h;

    /** Pointer to the bitmap array */
    const uint16_t *data;

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
    Bitmap() = delete;

    /**
     * @brief               Dynamically create a new bitmap instance
     *
     * @warning             This method returns a nullptr if a bitmap instance could not be created
     *
     * @param parent        The frame that should own this bitmap
     * @param data          Pointer to the Bitmap array
     * @param x             X-coordinate of the bitmap, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the bitmap, within `parent` (offset from top-edge)
     * @param width         Number of columns occupied by the bitmap
     * @param height        Number of rows occupied by the bitmap
     *
     * @return              A pointer to the bitmap instance (nullptr if the creation failed)
     *
     */
    static Bitmap *create(Frame *parent, const uint16_t *data, unsigned x, unsigned y,  unsigned width, unsigned height);

    // BasicWidget overrides

    Frame *get_parent();

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

    Bitmap *set_onpress(callback_t callback) override;
    Bitmap *reset_onpress() override;

    Bitmap *set_onrelease(callback_t callback) override;
    Bitmap *reset_onrelease() override;

    Bitmap *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) override;
    Bitmap *reset_event_queue() override;

    Bitmap *set_args(unsigned *new_args) override;
    Bitmap *reset_args() override;
    unsigned *get_args() override;

    bool is_pressed() const override;

    Bitmap *set_interactable(bool new_state) override;
    bool get_interactable() const override;

protected:

    /**
     * @brief               Construct a new Bitmap object
     *
     * @param parent        The frame that should own this bitmap
     * @param data          Pointer to the Bitmap array
     * @param x             X-coordinate of the bitmap, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the bitmap, within `parent` (offset from top-edge)
     * @param width         Number of columns occupied by the bitmap
     * @param height        Number of rows occupied by the bitmap
     *
     */
    Bitmap(Frame *parent, const uint16_t *data, unsigned x, unsigned y, unsigned width, unsigned height);
};

#endif