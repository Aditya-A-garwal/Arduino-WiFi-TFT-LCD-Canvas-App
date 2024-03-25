/**
 * @file                    drawablecanvas.h
 * @author                  Aditya Agarwal (aditya.agarwal@dumblebots.com)
 * @brief                   File that declares the `DrawableCanvas` class, which provides a canvas to draw on
 *
 */

#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_DRAWABLE_CANVAS_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_DRAWABLE_CANVAS_H__

#include "widget.h"
#include "frame.h"

#include "WiFiS3.h"
#include "cstring"

/**
 * @brief                   Class that provides a canvas to draw on
 *
 */
class DrawableCanvas : public BasicWidget {

public:

    constexpr static unsigned WIDTH = 312;
    constexpr static unsigned HEIGHT = 312;

    constexpr static uint16_t DRAWABLE_W = WIDTH - 2;
    constexpr static uint16_t DRAWABLE_H = HEIGHT - 2;
    constexpr static unsigned MAX_INLINE_COMPRESSED_SEGMENTS = 9;

    /**
     * @brief               Class that provides a buffered TCP Stream to write to
     *
     */
    class BufferedTCPStream {

        friend class DrawableCanvas;

    protected:

        constexpr static unsigned BUFFER_CAPACITY = 1024;

        /** Pointer to underlying TCP socket */
        WiFiClient *client {nullptr};

        /** Buffer to store bytes in */
        uint8_t buf[BUFFER_CAPACITY];

        /** Current size of buffer (number of bytes used up) */
        unsigned size {0};
        /** Whether the object is in a valid state or not */
        bool flag {true};

    public:

        /**
         * @brief               Connect to a server
         *
         * @param ptr           Reference to TCP Socket to use (discarded after calling `stop`)
         * @param server_ip     IP Address of server
         * @param server_port   Port of server
         *
         * @return              Return value of `WiFiClient::connect()` on `ptr`
         *
         */
        signed connect(WiFiClient *ptr, const char *server_ip, const uint16_t server_port);

        /**
         * @brief               Write a buffer
         *
         * @note                It is necessary to check `flag` after calling this method to ensure that the object is in a valid state
         *
         * @param bytes         Pointer to buffer
         * @param len           Number of bytes to pick from the buffer
         *
         */
        void write(const uint8_t *bytes, unsigned len);

        /**
         * @brief               Flush the stream
         *
         * @note                It is necessary to check `flag` after calling this method to ensure that the object is in a valid state
         *
         */
        void flush();

        /**
         * @brief               Flush all data and disconnect from the server
         *
         */
        void stop();
    };


    /**
     * @brief                   Utilities to compress arrays
     *
     */
    class Compressor {

    public:

        struct segment_t {
            uint16_t code: 4;
            uint16_t size: 11;
            uint16_t flag: 1;
        };
        static_assert(sizeof(segment_t) == sizeof(uint16_t));

        struct canvas_row_t {
            uint16_t segment_count: 6;
            uint16_t pixel_count: 10;
            segment_t *segments;
        };

        /**
         * @brief               Compress a single row of values into its segment representation
         *
         * @param row           Pointer to destination, where compressed data will be stored
         * @param max_segments  Maximum number of segments that can be accomodated in the destination
         * @param raw_data      Pointer to source data, a raw buffer of bytes
         * @param raw_data_len  Length of raw data
         *
         * @return              The size of the largest prefix of `raw_data` that could be compressed
         *
         */
        static unsigned compress(canvas_row_t *row, unsigned max_segments, uint8_t *raw_data, unsigned raw_data_len);

        /**
         * @brief               Decompress segments to a single row
         *
         * @param row           Pointer to source, whether the compressed data is stored
         * @param raw_data      Pointer to destination, where raw data will be stored
         * @param raw_data_len  Maximum number of bytes that can be accomodated in the destination
         *
         * @return              The number of bytes in raw data after decompressing
         *
         */
        static unsigned decompress(canvas_row_t *row, uint8_t *raw_data, unsigned raw_data_len);
    };


protected:

    constexpr static unsigned BUFFER_WIDTH = 28;
    constexpr static unsigned BUFFER_HEIGHT = 28;

    /** Reference to parent frame */
    Frame *parent {nullptr};

    /** Flag that indicates if the bitmap is dirty or not */
    bool dirty {false};
    /** Flag that indicates if the bitmap's visibility has been changed or not */
    bool visibility_changed {false};
    /** Flag to indicate if the bitmap is hidden or visible */
    bool visible {true};

    /** X-coordinate of the canvas relative to its parent (offset from left-edge) */
    unsigned widget_x;
    /** Y-coordinate of the canvas relative to its parent (offset from top-edge) */
    unsigned widget_y;

    /** X-coordinate of the canvas relative to the display (offset from left-edge) */
    unsigned widget_absolute_x;
    /** Y-coordinate of the canvas relative to the display (offset from top-edge) */
    unsigned widget_absolute_y;

    /** 16-bit color of the pen */
    uint16_t pen_color {0};
    /** Radius of the pen stroke */
    unsigned pen_size {0};

    /** IP Address of the server */
    char server_ip[16] {0};
    /** Port of the server */
    uint16_t server_port {0};

    /**
     * @brief           In-memory buffer to draw-on
     *
     *                  Each stroke from the pen is first drawn on the canvas and then this buffer,
     *                  The contents of the buffer are used while updating the compressed representation of the canvas.
     *                  The entire contents of the canvas will not fit in memory, and therefore a compressed
     *                  representation is stored. Each time the pen creates a new stroke, the compressed
     *                  representation needs to be updated after reading the updated area of the screen.
     *                  Since this is a slow operation, the drawing is also carried onto this in-memory buffer,
     *                  which is used to (after offsetting the pixels) update the compressed representation
     *
     */
    GFXcanvas1 drawing_buffer;

    /** Compressed representation of the canvas */
    Compressor::canvas_row_t compressed_rows[DRAWABLE_H];
    /** Compressed representation of a single row (used by member functions as buffer) */
    Compressor::canvas_row_t cur_row;

    /** Function to call when a drawing could successfully be saved/loaded */
    InteractiveWidget::callback_t on_success {nullptr};
    /** Function to call when a connection could not be established with the server */
    InteractiveWidget::callback_t on_connection_failure {nullptr};
    /** Function to call when the communication with the server failed midway */
    InteractiveWidget::callback_t on_communication_failure {nullptr};

    /** Pointer to arguments passed to callbacks */
    unsigned *args {nullptr};

    /** Reference to event queue for posting events */
    RingQueueInterface<InteractiveWidget::callback_event_t> *event_queue {nullptr};

public:

    /**
     * @brief               Default constructor disabled (use the `create` method)
     *
     */
    DrawableCanvas() = delete;

    /**
     * @brief               Dynamically create a new canvas instance
     *
     * @warning             This method returns a nullptr if a canvas instance could not be created
     *
     * @param parent        The frame that should own this canvas
     * @param x             X-coordinate of the canvas, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the canvas, within `parent` (offset from top-edge)
     *
     * @return              A pointer to the canvas instance (nullptr if the creation fails)
     *
     */
    static DrawableCanvas *create(Frame *parent, unsigned x, unsigned y);

    /**
     * @brief               Set the color of the stroke
     *
     * @param new_color     16-bit color to use
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *set_pen_color(uint16_t new_color);

    /**
     * @brief               Get the color of the stroke
     *
     * @return              16-bit color of the stroke
     *
     */
    uint16_t get_pen_color() const;

    /**
     * @brief               Set the radius of the stroke
     *
     * @param new_size      Radius to use
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *set_pen_size(uint16_t new_size);

    /**
     * @brief               Get the radius of the stroke
     *
     * @return              Radius of the stroke
     *
     */
    unsigned get_pen_size() const;

    /**
     * @brief                   Set the address of the server
     *
     * @param new_server_ip     IP address of the server (must be a valid IP address that is reachable)
     * @param new_server_port   Port on which the server is listening
     *
     * @return                  Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *set_server_addr(const char *new_server_ip, const uint16_t new_server_port);

    /**
     * @brief               Draw a stroke at a point
     *
     * @param x             X-coordinate of the stroke (offset from left-edge)
     * @param y             Y-coordinate of the stroke (offset from top-edge)
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *draw_at(unsigned x, unsigned y);

    /**
     * @brief               Reset the canvas to its original state
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *clear_canvas();

    /**
     * @brief               Set the function to be called when a drawing was successfully saved/loaded
     *
     * @param cb            Reference to the callback function
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *set_success_callback(InteractiveWidget::callback_t cb);

    /**
     * @brief               Reset the function to be called when a drawing was successfully saved/loaded
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *reset_success_callback();

    /**
     * @brief               Set the function to be called when communication could not be established with server
     *
     * @param cb            Reference to the callback function
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *set_connection_failure_callback(InteractiveWidget::callback_t cb);

    /**
     * @brief               Reset the function to be called when communication could not be established with server
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *reset_connection_failure_callback();

    /**
     * @brief               Set the function to be called when communication with the server broke midway
     *
     * @param cb            Reference to the callback function
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *set_communication_failure_callback(InteractiveWidget::callback_t cb);

    /**
     * @brief               Reset the function to be called when communication with the server broke midway
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *reset_communication_failure_callback();

    /**
     * @brief               Set the reference to the arguments which must be passed to callbacks
     *
     * @param new_args      Reference to the new arguments
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *set_args(unsigned *new_args);

    /**
     * @brief               Remove the reference to the arguments which are passed to callbacks
     *
     * @warning             This method does not explicitly perform any garbage collection for the old arguments
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *reset_args();

    /**
     * @brief               Get a reference to the arguments which are passed to callbacks (as set be calls to `set_args`)
     *
     * @return              Pointer to arguments
     *
     */
    unsigned *get_args() const;

    /**
     * @brief               Set the queue on which the canvas can enqueue events
     *
     * @param new_event_queue   Mutable reference to queue which should be used
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *set_event_queue(RingQueueInterface<InteractiveWidget::callback_event_t> *new_event_queue);

    /**
     * @brief               Reset the event queue so that the widget may not enqueue any events
     *
     * @return              Pointer to the canvas (allows chaining method calls)
     *
     */
    DrawableCanvas *reset_event_queue();

    /**
     * @brief               Save the current drawing to the server
     *
     * @param slot          The slot to save the drawing to (number between 0 and 255 inclusive)
     *
     * @return true         If the drawing could successfully be saved
     * @return false        If the drawing could not be saved
     *
     */
    bool save_to_server(uint8_t slot);

    /**
     * @brief               Load a drawing from the server to the canvas, overwriting its contents
     *
     * @param slot          The slot to load the drawing from (number between 0 and 255)
     *
     * @return true         If the drawing could succesfully be loaded
     * @return false        If the drawing oculd not be loaded
     *
     */
    bool load_from_server(uint8_t slot);

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
     * @brief               Construct a new Drawable Canvas object
     *
     * @param parent        The frame that should own this canvas
     * @param x             X-coordinate of the canvas, within `parent` (offset from left-edge)
     * @param y             Y-coordinate of the canvas, within `parent` (offset from top-edge)
     *
     */
    DrawableCanvas(Frame *parent, unsigned x, unsigned y);

    /**
     * @brief               Reset the compressed representation of the canvas
     *
     */
    void reset_compressed();
};

#endif
