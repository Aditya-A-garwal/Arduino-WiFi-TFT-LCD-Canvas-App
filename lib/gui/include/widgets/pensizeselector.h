/**
 * @file                    pensizeselector.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   File that declares the `PenSizeSelector` class, which is used to create row of pen-sizes to pick from from
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_PEN_SIZE_SELECTOR_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_PEN_SIZE_SELECTOR_H__

#include "widget.h"
#include "frame.h"

/**
 * @brief                   Class that implements a widget which is used to create row of pen-sizes to pick from from
 *
 */
class PenSizeSelector : public BasicWidget, public InteractiveWidget {

protected:

    constexpr static unsigned H_PAD = 17;

    constexpr static unsigned WIDTH = 8 * H_PAD;
    constexpr static unsigned HEIGHT = 29;

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

    /** Color of the spots */
    uint16_t color {};
    /** Radii of the available options */
    uint16_t sizes[4] {};

    /** Selected size */
    unsigned selected_size {};

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
    PenSizeSelector() = delete;

    /**
     * @brief               Dynamically create a new pen size selector instance
     *
     * @warning             This method returns a nullptr if a color selector instance could not be created
     *
     * @param parent        The frame that should own this pen size selector
     * @param x             X-coordinate of the pen size selector, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the pen size selector, within `parent` (offset from top-edge)
     *
     * @return
     *
     */
    static PenSizeSelector *create(Frame *parent, unsigned x, unsigned y);

    /**
     * @brief               Set the size at a position
     *
     * @param pos           Position of the new size (from left to right in increasing order)
     * @param new_size      Radius
     *
     * @return              Pointer to the pen size selector (allows chaining method calls)
     *
     */
    PenSizeSelector *set_size(unsigned pos, uint16_t new_size);

    /**
     * @brief               Get the size at a position
     *
     * @param pos           Position to get from (from left to right in increasing order)
     *
     * @return              Radius
     *
     */
    uint16_t get_size(unsigned pos) const;

    /**
     * @brief               Set the selected size
     *
     * @param pos           Position of the size which should be selected
     *
     * @return              Pointer to the pen size selector (allows chaining method calls)
     *
     */
    PenSizeSelector *set_selected_size(unsigned pos);

    /**
     * @brief               Get the selected size
     *
     * @return              Radius
     */
    uint16_t get_selected_size() const;

    /**
     * @brief               Set the color of the size selection spots
     *
     * @param new_color     16-bit color
     *
     * @return              Pointer to the pen size selector (allows for chaining method calls)
     *
     */
    PenSizeSelector *set_color(uint16_t new_color);

    /**
     * @brief               Get the color of the size selection spots
     *
     * @return              Pointer to the pen size selector (allows for chaining method calls)
     */
    uint16_t get_color() const;

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

    PenSizeSelector *set_onpress(callback_t callback)  override;
    PenSizeSelector *reset_onpress()  override;

    PenSizeSelector *set_onrelease(callback_t callback)  override;
    PenSizeSelector *reset_onrelease()  override;

    PenSizeSelector *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue)  override;
    PenSizeSelector *reset_event_queue()  override;

    PenSizeSelector *set_args(unsigned *new_args)  override;
    PenSizeSelector *reset_args()  override;
    unsigned *get_args()  override;

    bool is_pressed() const override;

    PenSizeSelector *set_interactable(bool new_state) override;
    bool get_interactable() const override;

protected:

    /**
     * @brief               Construct a new Pen Size Selector object
     *
     * @param parent        The frame that should own this pen size selector
     * @param x             X-coordinate of the pen size selector, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the pen size selector, within `parent` (offset from top-edge)
     *
     */
    PenSizeSelector(Frame *parent, unsigned x, unsigned y);
};

#endif
