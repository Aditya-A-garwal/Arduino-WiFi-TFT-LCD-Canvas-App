#include "widgets/drawablecanvas.h"

static Compressor::segment_t segments1[(DrawableCanvas::DRAWABLE_W + 1) / 2];
static Compressor::segment_t segments2[DrawableCanvas::DRAWABLE_H][DrawableCanvas::MAX_INLINE_COMPRESSED_SEGMENTS];
static WiFiClient sock;

DrawableCanvas::DrawableCanvas(Frame *parent, unsigned x, unsigned y)
    : parent {parent}
    , pen_color {BLACK}
    , pen_size {3}
    , widget_x {x}
    , widget_y {y}
    , widget_absolute_x { x + parent->get_absolute_x() }
    , widget_absolute_y { y + parent->get_absolute_y() }
    , drawing_buffer(BUFFER_WIDTH, BUFFER_HEIGHT)
{}

DrawableCanvas *DrawableCanvas::create(Frame *parent, unsigned x, unsigned y) {

    DrawableCanvas *canvas = new (std::nothrow) DrawableCanvas(parent, x, y);
    if (canvas == nullptr) {
        return canvas;
    }

    canvas->cur_row.segments = segments1;
    for (unsigned r = 0, offset = 0; r < DRAWABLE_H; ++r, offset += MAX_INLINE_COMPRESSED_SEGMENTS) {
        canvas->compressed_rows[r].segments = segments2[r];

        canvas->compressed_rows[r].pixel_count = DRAWABLE_W;
        canvas->compressed_rows[r].segment_count = 1;
        canvas->compressed_rows[r].segments[0].code = color_2_code(BLACK);
        canvas->compressed_rows[r].segments[0].size = DRAWABLE_W;
    }

    sock.setTimeout(8'000);

    parent->add_child(canvas);
    return canvas;
}

DrawableCanvas *DrawableCanvas::set_pen_color(uint16_t new_color) {
    pen_color = new_color;
    return this;
}
uint16_t DrawableCanvas::get_pen_color() const { return pen_color; }

DrawableCanvas *DrawableCanvas::set_server_addr(const char *new_server_ip, const uint16_t new_server_port) {

    std::memset(server_ip, 0, sizeof(server_ip));
    std::strncpy(server_ip, new_server_ip, 16);

    server_port = new_server_port;

    return this;
}

DrawableCanvas *DrawableCanvas::set_pen_size(uint16_t new_size) {
    pen_size = new_size;
    return this;
}
unsigned DrawableCanvas::get_pen_size() const { return pen_size; }

DrawableCanvas *DrawableCanvas::draw_at(unsigned x, unsigned y) {

    unsigned col_l, col_h, row_l, row_h;
    uint8_t code;
    uint8_t raw_pixels[DRAWABLE_W];

    if (((x - pen_size - 1) <= widget_x)
        || ((x + pen_size + 1) >= (widget_x + WIDTH))) {
        return this;
    }

    if (((y - pen_size - 1) <= widget_y)
        || ((y + pen_size + 1) >= (widget_y + HEIGHT))) {
        return this;
    }

    parent->fill_circle(x, y, pen_size, pen_color);

    drawing_buffer.fillRect(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT, 0);
    drawing_buffer.fillCircle(pen_size, pen_size, pen_size, 1);

    x -= widget_x;
    y -= widget_y;

    col_l = x - pen_size;
    col_h = x + pen_size;

    row_l = y - pen_size;
    row_h = y + pen_size;

    code = color_2_code(pen_color);

    for (unsigned r = row_l, final_pixel; r <= row_h; ++r) {

        if (compressed_rows[r].pixel_count < col_l) {
            continue;
        }
        Compressor::uncompress(&compressed_rows[r], raw_pixels, DRAWABLE_W);
        final_pixel = compressed_rows[r].pixel_count - 1;

        for (unsigned c = col_l; c <= col_h; ++c) {

            if (!drawing_buffer.getPixel(c - col_l, r - row_l)) {
                continue;
            }
            raw_pixels[c] = code;
            final_pixel = max(final_pixel, c);
        }
        Compressor::compress(&compressed_rows[r], MAX_INLINE_COMPRESSED_SEGMENTS, raw_pixels, final_pixel + 1);
    }

    return this;
}

DrawableCanvas *DrawableCanvas::clear_canvas() {
    parent->fill_rect(widget_x + 1, widget_y + 1, WIDTH - 2, HEIGHT - 2, BLACK);
    reset_compressed();
    return this;
}

DrawableCanvas *DrawableCanvas::set_success_callback(InteractiveWidget::callback_t cb) {
    on_success = cb;
    return this;
}
DrawableCanvas *DrawableCanvas::reset_success_callback() {
    on_success = nullptr;
    return this;
}

DrawableCanvas *DrawableCanvas::set_connection_failure_callback(InteractiveWidget::callback_t cb) {
    on_connection_failure = cb;
    return this;
}
DrawableCanvas *DrawableCanvas::reset_connection_failure_callback() {
    on_connection_failure = nullptr;
    return this;
}

DrawableCanvas *DrawableCanvas::set_communication_failure_callback(InteractiveWidget::callback_t cb) {
    on_communication_failure = cb;
    return this;
}
DrawableCanvas *DrawableCanvas::reset_communication_failure_callback() {
    on_communication_failure = nullptr;
    return this;
}

DrawableCanvas *DrawableCanvas::set_args(unsigned *new_args) {
    args = new_args;
    return this;
}
DrawableCanvas *DrawableCanvas::reset_args() {
    args = nullptr;
    return this;
}
unsigned *DrawableCanvas::get_args() const { return args; }

DrawableCanvas *DrawableCanvas::set_event_queue(RingQueueInterface<InteractiveWidget::callback_event_t> *new_event_queue) {
    event_queue = new_event_queue;
    return this;
}
DrawableCanvas *DrawableCanvas::reset_event_queue() {
    event_queue = nullptr;
    return this;
}

bool DrawableCanvas::save_to_server(uint8_t slot) {

    constexpr static unsigned MAX_SEGMENTS = (DRAWABLE_W + 1) / 2;
    uint8_t codes[DRAWABLE_W];

    BufferedWiFiWriter client;

    if (strnlen(server_ip, 16) == 0 || !client.connect(&sock, server_ip, server_port)) {

        if (event_queue != nullptr && on_connection_failure != nullptr) {
            event_queue->push({on_connection_failure, args});
        }
        return false;
    }

    client.write((uint8_t *)"\x01", 1);
    client.write((uint8_t *)&slot, 1);
    client.write((uint8_t *)&DRAWABLE_H, 2);
    client.write((uint8_t *)&DRAWABLE_W, 2);

    if (!client.flag) {
        if (event_queue != nullptr && on_communication_failure != nullptr) {
            event_queue->push({on_communication_failure, args});
        }
        client.stop();
        return false;
    }

    for (unsigned r = 0; r < DRAWABLE_H; ++r) {

        Compressor::uncompress(&compressed_rows[r], codes, DRAWABLE_W);
        for (unsigned c = compressed_rows[r].pixel_count; c < DRAWABLE_W; ++c) {
            codes[c] = color_2_code(parent->get_at(widget_x + 1 + c, widget_y + 1 + r));
        }
        Compressor::compress(&cur_row, MAX_SEGMENTS, codes, DRAWABLE_W);

        if (cur_row.pixel_count == DRAWABLE_W) {

            uint8_t segment_count = cur_row.segment_count;
            client.write(&segment_count, 1);
            client.write((uint8_t *)(cur_row.segments), sizeof(Compressor::segment_t) * segment_count);
        }
        else {

            client.write((uint8_t *)"\x00", 1);
            client.write(codes, DRAWABLE_W);
        }

        if (!client.flag) {
            if (event_queue != nullptr && on_communication_failure != nullptr) {
                event_queue->push({on_communication_failure, args});
            }
            client.stop();
            return false;
        }
    }

    if (event_queue != nullptr && on_success != nullptr) {
        event_queue->push({on_success, args});
    }

    client.stop();
    return true;
}

bool DrawableCanvas::load_from_server(uint8_t slot) {

    uint8_t codes[DRAWABLE_W];

    if (strnlen(server_ip, 16) == 0 || !sock.connect(IPAddress(server_ip), server_port)) {
        if (event_queue != nullptr && on_connection_failure != nullptr) {
            event_queue->push({on_connection_failure, args});
        }
        sock.stop();
        return false;
    }

    sock.write((uint8_t *)"\x02", 1);
    sock.write(&slot, 1);
    sock.write((uint8_t *)&DRAWABLE_H, 2);
    sock.write((uint8_t *)&DRAWABLE_W, 2);
    sock.write((uint8_t *)&DRAWABLE_W, 2);

    for (unsigned r = 0; r < DRAWABLE_H; ++r) {

        signed status = sock.readBytes(codes, DRAWABLE_W);

        if (status != DRAWABLE_W) {
            if (event_queue != nullptr && on_communication_failure != nullptr) {
                event_queue->push({on_communication_failure, args});
            }

            sock.stop();
            return false;
        }

        if (r % 10 == 0) {
            sock.write("\x00", 1);
        }

        for (unsigned c = 0; c < DRAWABLE_W; ++c) {
            parent->set_at(widget_x + 1 + c, widget_y + 1 + r, code_2_color(codes[c]));
        }
        Compressor::compress(&compressed_rows[r], MAX_INLINE_COMPRESSED_SEGMENTS, codes, DRAWABLE_W);
    }

    if (event_queue != nullptr && on_success != nullptr) {
        event_queue->push({on_success, args});
    }

    sock.write("\x00", 1);
    sock.flush();

    sock.stop();
    return true;
}

// BasicWidget overrides

Frame *DrawableCanvas::get_parent() { return parent; }

unsigned DrawableCanvas::get_x() const { return widget_x; }
unsigned DrawableCanvas::get_y() const { return widget_y; }

unsigned DrawableCanvas::get_absolute_x() const { return widget_absolute_x; }
unsigned DrawableCanvas::get_absolute_y() const { return widget_absolute_y; }

unsigned DrawableCanvas::get_width() const { return WIDTH; }
unsigned DrawableCanvas::get_height() const { return HEIGHT; }

bool DrawableCanvas::get_dirty() const { return dirty; }
bool DrawableCanvas::get_visibility_changed() const { return visibility_changed; }

void DrawableCanvas::set_dirty() { dirty = true; }
void DrawableCanvas::set_visibility_changed() { visibility_changed = true; }

void DrawableCanvas::draw() {

    dirty = false;
    visibility_changed = false;

    reset_compressed();

    parent->fill_rect(widget_x, widget_y, WIDTH, HEIGHT, BLACK);
    parent->draw_rect(widget_x, widget_y, WIDTH, HEIGHT, WHITE);
}
void DrawableCanvas::clear() {

    dirty = false;
    visibility_changed = false;

    parent->fill_rect(widget_x, widget_y, WIDTH, HEIGHT, BLACK);
}

bool DrawableCanvas::get_intersection(unsigned x, unsigned y) const {
    return (widget_x <= x && x <= (widget_x + WIDTH))
        && (widget_y <= y && y <= (widget_y + HEIGHT));
}

bool DrawableCanvas::get_intersection(BasicWidget *other) const {

    unsigned x0 = other->get_absolute_x();
    unsigned y0 = other->get_absolute_y();

    unsigned x1 = x0 + other->get_width();
    unsigned y1 = y0 + other->get_height();

    if ((
        (widget_absolute_x <= x0 && x0 <= (widget_absolute_x + WIDTH))
        || (widget_absolute_x <= x1 && x1 <= (widget_absolute_x + WIDTH))
    )
    && (
        (widget_absolute_y <= y0 && y0 <= (widget_absolute_y + HEIGHT))
        || (widget_absolute_y <= y1 && y1 <= (widget_absolute_y + HEIGHT))
    )) {
        return true;
    }

    if ((
        (x0 <= widget_absolute_x && widget_absolute_x <= x1)
        || (x0 <= (widget_absolute_x + WIDTH) && (widget_absolute_x + WIDTH) <= x1)
    )
    && (
        (y0 <= widget_absolute_y && widget_absolute_y <= y1)
        || (y0 <= (widget_absolute_y + HEIGHT) && (widget_absolute_y + HEIGHT) <= y1)
    )) {
        return true;
    }

    return false;
}

bool DrawableCanvas::propagate_press(unsigned x, unsigned y) {

    if (!get_intersection(x, y)) {
        return false;
    }

    return true;
}
bool DrawableCanvas::propagate_release(unsigned x, unsigned y) {

    if (!get_intersection(x, y)) {
        return false;
    }

    return true;
}

bool DrawableCanvas::get_visibility() const { return visible; }
void DrawableCanvas::set_visibility(bool new_visibility) {
    if (new_visibility == visible) {
        return;
    }

    dirty = true;
    visibility_changed = true;
    visible = new_visibility;
}

void DrawableCanvas::reset_compressed() { //todo this is to be used in many places, not just clear
    for (unsigned r = 0; r < DRAWABLE_H; ++r) {
        compressed_rows[r].pixel_count = DRAWABLE_W;
        compressed_rows[r].segment_count = 1;
        compressed_rows[r].segments[0].code = color_2_code(BLACK);
        compressed_rows[r].segments[0].size = DRAWABLE_W;
    }
}
