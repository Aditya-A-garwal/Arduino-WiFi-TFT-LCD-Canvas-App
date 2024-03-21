#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_DRAWABLE_CANVAS_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_DRAWABLE_CANVAS_H__

#include "widget.h"
#include "frame.h"

#include "WiFiS3.h"
#include "cstring"

class BufferedWiFiWriter {

friend class DrawableCanvas;

protected:

    constexpr static unsigned BUFFER_CAPACITY = 1024;

    WiFiClient *client;
    uint8_t buf[BUFFER_CAPACITY];

    unsigned used {0};
    bool flag {true};

public:

    signed connect(WiFiClient *ptr, const char *server_ip, const uint16_t server_port) {
        client = ptr;
        return client->connect(IPAddress(server_ip), server_port);
    }

    void write(const uint8_t *bytes, unsigned len) {

        unsigned n;

        while (len > 0) {

            if (used == BUFFER_CAPACITY) {
                flush();
            }

            n = min(len, BUFFER_CAPACITY - used);
            for (const uint8_t *ptr = bytes; ptr != &bytes[n]; ++ptr) {
                buf[used++] = *ptr;
            }
            len -= n;
            bytes += n;
        }
    }

    void flush() {

        unsigned n;

        n = client->write(buf, used);
        if (n != used) {
            flag = false;
        }
        used = 0;
    }

    void stop() {
        flush();
        client->flush();
        client->stop();

        client = nullptr;
    }
};

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

    static unsigned compress(canvas_row_t *row, unsigned max_segments, uint8_t *raw_data, unsigned raw_data_len) {

        unsigned finished = 0;

        for (unsigned l = 0, r; l < raw_data_len; l = r) {
            for (r = 1 + l; r <= raw_data_len; ++r) {
                if (r == raw_data_len || raw_data[l] != raw_data[r]) {
                    break;
                }
            }

            if (++finished > max_segments) {
                row->pixel_count = l;
                row->segment_count = finished - 1;

                return l;
            }

            row->segments[finished - 1].code = raw_data[l];
            row->segments[finished - 1].size = r - l;
        }

        row->pixel_count = raw_data_len;
        row->segment_count = finished;

        return raw_data_len;
    }

    static unsigned uncompress(canvas_row_t *row, uint8_t *raw_data, unsigned raw_data_len) {

        uint8_t code;
        unsigned size;

        if (row->segment_count == 0) {
            return 0;
        }

        for (unsigned s = 0, idx = 0; s < row->segment_count; ++s) {

            code = row->segments[s].code;
            size = row->segments[s].size;

            while (size--) {
                raw_data[idx++] = code;
            }
        }

        return row->pixel_count;
    }
};

class DrawableCanvas : public BasicWidget {

public:

    constexpr static unsigned WIDTH = 312;
    constexpr static unsigned HEIGHT = 312;

    constexpr static uint16_t DRAWABLE_W = WIDTH - 2;
    constexpr static uint16_t DRAWABLE_H = HEIGHT - 2;
    constexpr static unsigned MAX_INLINE_COMPRESSED_SEGMENTS = 7;

protected:

    constexpr static unsigned BUFFER_WIDTH = 28;
    constexpr static unsigned BUFFER_HEIGHT = 28;

    Frame *parent {nullptr};

    bool dirty {false};
    bool visibility_changed {false};

    bool visible {true};

    unsigned widget_x {0};
    unsigned widget_y {0};

    unsigned widget_absolute_x {0};
    unsigned widget_absolute_y {0};

    uint16_t pen_color {0};
    unsigned pen_size {0};

    char server_ip[16] {0};
    uint16_t server_port {0};

    GFXcanvas1 drawing_buffer;

    Compressor::canvas_row_t compressed_rows[DRAWABLE_H];
    Compressor::canvas_row_t cur_row;

    InteractiveWidget::callback_t on_success {nullptr};
    InteractiveWidget::callback_t on_connection_failure {nullptr};
    InteractiveWidget::callback_t on_communication_failure {nullptr};

    unsigned *args {nullptr};

    RingQueueInterface<InteractiveWidget::callback_event_t> *event_queue {nullptr};

public:

    DrawableCanvas() = delete;

    static DrawableCanvas *create(Frame *parent, unsigned x, unsigned y);

    DrawableCanvas *set_pen_color(uint16_t new_color);
    uint16_t get_pen_color() const;

    DrawableCanvas *set_pen_size(uint16_t new_size);
    unsigned get_pen_size() const;

    DrawableCanvas *set_server_addr(const char *new_server_ip, const uint16_t new_server_port);

    DrawableCanvas *draw_at(unsigned x, unsigned y);
    DrawableCanvas *clear_canvas();

    DrawableCanvas *set_success_callback(InteractiveWidget::callback_t cb);
    DrawableCanvas *reset_success_callback();

    DrawableCanvas *set_connection_failure_callback(InteractiveWidget::callback_t cb);
    DrawableCanvas *reset_connection_failure_callback();

    DrawableCanvas *set_communication_failure_callback(InteractiveWidget::callback_t cb);
    DrawableCanvas *reset_communication_failure_callback();

    DrawableCanvas *set_args(unsigned *new_args);
    DrawableCanvas *reset_args();
    unsigned *get_args() const;

    DrawableCanvas *set_event_queue(RingQueueInterface<InteractiveWidget::callback_event_t> *new_event_queue);
    DrawableCanvas *reset_event_queue();

    bool save_to_server(uint8_t slot);
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

    DrawableCanvas(Frame *parent, unsigned x, unsigned y);

    void reset_compressed();
};

#endif