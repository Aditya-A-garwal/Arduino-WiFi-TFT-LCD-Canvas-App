#include "Arduino.h"
#include "WiFiS3.h"

#include "arduino_secrets.h"

#include "touchscreen_driver.h"
#include "touchscreen_constants.h"

#include "Adafruit_GFX.h"

#include "widgets/app.h"
#include "widgets/view.h"
#include "widgets/drawablecanvas.h"
#include "widgets/colorselector.h"
#include "widgets/pensizeselector.h"
#include "widgets/keyboard.h"
#include "widgets/window.h"
#include "widgets/button.h"
#include "widgets/label.h"
#include "widgets/textbox.h"
#include "widgets/bitmap.h"

#include "bitmaps.h"

MCUFRIEND_kbv tft;

Touchscreen ts(XP, YP, XM, YM);

App *app;

// startup view

View *startup_view;

Label *startup_title;

Window *apps_window;
Bitmap *canvas_app_icon;
Bitmap *settings_app_icon;
Bitmap *information_app_icon;

// main view

View *main_view;

Button *main_back_button;
Label *main_title;

DrawableCanvas *canvas;

Window *tools_window;

ColorSelector *color_selector;
PenSizeSelector *size_selector;

Button *save_button;
Button *load_button;

Button *connection_button;
Button *information_button;

Button *clear_button;

Window *slot_selection_window;
Button *slot_exit_button;
Button *slot_buttons[6];

TextBox *communication_status_label;

struct slot_button_args_t {
    unsigned number;
    Button *caller;
};

// connection view

View *connection_view;

Button *connection_back_button;
Label *connection_title;

Window *connection_form_window;

Label *ssid_label;
TextBox *ssid_box;

Label *pass_label;
TextBox *pass_box;

Label *addr_label;
TextBox *addr_box;

Button *connect_button;
Label *status_label;

Keyboard *keyboard;

// information view

View *information_view;

Button *information_back_button;
Label *information_title;

Window *information_form_window;

Label *ssid_readonly_label;
Label *ssid_readonly_box;

Label *ip_addr_label;
Label *ip_addr_box;

Bitmap *dumblebots_qrcode;
Label *dumblebots_website_label;


[[noreturn]] void err(const char msg[]);


void init_startup_view();
void init_main_view();
void init_connection_view();
void init_information_view();


// main view

void switch_to_startup(unsigned *args);
void switch_to_connection(unsigned *args);
void switch_to_information(unsigned *args);
void switch_to_main(unsigned *args);

void select_color_cb(unsigned *args);
void select_pen_size_cb(unsigned *args);

void clear_button_cb(unsigned *args);

void open_slot_selection(unsigned *args);
void exit_slot_selection(unsigned *args);
void slot_selection_cb(unsigned *args);

void server_connection_failure_cb(unsigned *args);
void server_communication_failure_cb(unsigned *args);
void server_success_cb(unsigned *args);

// connection view

void open_keyboard(unsigned *args);

void ssid_box_edit_cb(unsigned *args);
void pass_box_edit_cb(unsigned *args);
void addr_box_edit_cb(unsigned *args);

void connect_cb(unsigned *args);
void try_connect(unsigned *args);
bool verify_server();


void setup() {

    Serial.begin(9600);

    tft.begin(0x9486);

    ts.set_dimensions(tft.width(), tft.height());
    ts.set_pressure_bounds(PRESSURE_LO, PRESSURE_RIGHT);

    app = App::create(&tft);
    if (app == nullptr) {
        err("Error while creating app");
    }

    init_startup_view();
    init_main_view();
    init_connection_view();
    init_information_view();

    app->make_active_view(startup_view);
    app->draw();
}

void loop() {

    bool press, release;
    unsigned px, py, rx, ry;

    ts.read_screen();

    press = ts.get_press(&px, &py);
    release = ts.get_release(&rx, &ry);

    if (release) {
        app->propagate_release(rx, ry);
    }
    if (press) {
        app->propagate_press(px, py);
    }

    app
    ->collect_dirty_widgets()
    ->update_dirty_widgets()
    ->execute_event_logic();

    {
        if (app->get_active_view() == main_view && ts.get_stylus_position(&px, &py)) {
            canvas->draw_at(px, py);
        }
    }

    delay(1);
}

/* ----- Start of Init functions ----- */

void init_startup_view() {

    startup_view = View::create(app);
    if (startup_view == nullptr) {
        err("Error while creating startup view");
    }

    startup_title = Label::create(startup_view, 30, 1, app->get_width() - 30 - 10, 27);
    if (startup_title == nullptr) {
        err("Error while creating startup title");
    }

    apps_window = Window::create(startup_view, 5, 30, app->get_width() - 10, app->get_height() - 45);
    if (apps_window == nullptr) {
        err("Error while creating apps window");
    }

    canvas_app_icon = Bitmap::create(apps_window, main_app_bmp2, 30, 30, 64, 92);
    if (canvas_app_icon == nullptr) {
        err("Error while creating canvas app icon");
    }

    settings_app_icon = Bitmap::create(apps_window, settings_app_bmp, canvas_app_icon->get_x() + canvas_app_icon->get_width() + 30, canvas_app_icon->get_y(), 64, 92);
    if (settings_app_icon == nullptr) {
        err("Error while creating settings app icon");
    }

    information_app_icon = Bitmap::create(apps_window, information_app_bmp, settings_app_icon->get_x() + settings_app_icon->get_width() + 30, settings_app_icon->get_y(), 64, 92);
    if (information_app_icon == nullptr) {
        err("Error while creating information app icon");
    }

    startup_title
    ->set_message("Apps")
    ->get_style()
    ->set_text_size(2)
    ->set_border_width(0)
    ->set_border_radius(0);

    apps_window
    ->get_style()
    ->set_border_radius(3);

    canvas_app_icon
    ->set_event_queue(app->get_event_queue())
    ->set_onrelease(switch_to_main);

    settings_app_icon
    ->set_event_queue(app->get_event_queue())
    ->set_onrelease(switch_to_connection);

    information_app_icon
    ->set_event_queue(app->get_event_queue())
    ->set_onrelease(switch_to_information);
}

void init_main_view() {

    main_view = View::create(app);
    if (main_view == nullptr) {
        err("Error while creating main view");
    }

    main_back_button = Button::create(main_view, 3, 15 - 12, 24, 24);
    if (main_back_button == nullptr) {
        err("Error while creating main back button");
    }

    main_title = Label::create(main_view, 30, 1, app->get_width() - 30 - 10, 27);
    if (main_title == nullptr) {
        err("Error while creating main title");
    }

    canvas = DrawableCanvas::create(main_view, 4, 30);
    if (canvas == nullptr) {
        err("Error while creating canvas");
    }

    tools_window = Window::create(main_view, 5, 345, app->get_width() - 10, app->get_height() - 345 - 5);
    if (tools_window == nullptr) {
        err("Error while creating tools window");
    }

    color_selector = ColorSelector::create(tools_window, 2, 2);
    if (color_selector == nullptr) {
        err("Error while creating color selector");
    }

    size_selector = PenSizeSelector::create(tools_window, color_selector->get_x() + color_selector->get_width() + 18, 11);
    if (size_selector == nullptr) {
        err("Error while creating pen size selector");
    }

    save_button = Button::create(tools_window, color_selector->get_x() + color_selector->get_width() + 3, size_selector->get_y() + size_selector->get_height() + 3, 64, 30);
    if (save_button == nullptr) {
        err("Error while creating save button");
    }

    load_button = Button::create(tools_window, save_button->get_x() + save_button->get_width() + 3, save_button->get_y(), 64, 30);
    if (load_button == nullptr) {
        err("Error while creating load button");
    }

    connection_button = Button::create(tools_window, color_selector->get_x() + color_selector->get_width() + 3, save_button->get_y() + save_button->get_height() + 3, 64, 30);
    if (connection_button == nullptr) {
        err("Error while creating connection button");
    }

    information_button = Button::create(tools_window, connection_button->get_x() + connection_button->get_width() + 3, connection_button->get_y(), 64, 30);
    if (information_button == nullptr) {
        err("Error while creating information button");
    }

    clear_button = Button::create(tools_window, load_button->get_x() + load_button->get_width() + 3, load_button->get_y(), 40, 3 + (30 * 2));
    if (clear_button == nullptr) {
        err("Error while creating clear button");
    }

    slot_selection_window = Window::create(tools_window, 16, 16, 280, 100);
    if (slot_selection_window == nullptr) {
        err("Error while creating slot selection window");
    }

    slot_exit_button = Button::create(slot_selection_window, 2, 2, 13, 13);
    if (slot_exit_button == nullptr) {
        err("Error while creating slot exit button");
    }

    for (unsigned c = 0, i = 0; c < 3; ++c) {
        for (unsigned r = 0; r < 2; ++r, ++i) {
            slot_buttons[i] = Button::create(slot_selection_window, (80 * c) + (5 * c) + 16, (34 * r) + (6 * r) + 16, 80, 32);
            if (slot_buttons[i] == nullptr) {
                err("Error while creating slot buttons");
            }
        }
    }

    communication_status_label = TextBox::create(slot_selection_window, 14, 12, slot_selection_window->get_width() - 28, slot_selection_window->get_height() - 24);
    if (communication_status_label == nullptr) {
        err("Error while creating communication status label");
    }

    main_back_button
    ->set_onrelease(switch_to_startup)
    ->set_event_queue(app->get_event_queue())
    ->set_message("x")
    ->get_style()
    ->set_text_size(2)
    ->set_bg_color(RED)
    ->set_fg_color(BLACK)
    ->set_pressed_bg_color(blend_color(RED, BLACK, 160))
    ->set_pressed_fg_color(WHITE)
    ->set_border_width(0)
    ->set_border_radius(14);

    main_title
    ->set_message("Canvas App")
    ->get_style()
    ->set_text_size(2)
    ->set_border_width(0)
    ->set_border_radius(0);

    canvas
    ->set_event_queue(app->get_event_queue())
    ->set_connection_failure_callback(server_connection_failure_cb)
    ->set_communication_failure_callback(server_communication_failure_cb)
    ->set_success_callback(server_success_cb);

    tools_window
    ->get_style()
    ->set_border_radius(3);

    color_selector
    ->set_color(0, RED)->set_color(1, GREEN)->set_color(2, BLUE)
    ->set_color(3, CYAN)->set_color(4, MAGENTA)->set_color(5, YELLOW)
    ->set_color(6, WHITE)->set_color(7, GRAY)->set_color(8, BLACK)
    ->set_event_queue(app->get_event_queue())
    ->set_onrelease(select_color_cb);

    size_selector
    ->set_size(0, 3)->set_size(1, 5)->set_size(2, 7)->set_size(3, 9)
    ->set_event_queue(app->get_event_queue())
    ->set_onrelease(select_pen_size_cb);

    save_button
    ->set_message("Save")
    ->set_args((unsigned *)save_button)
    ->set_onrelease(open_slot_selection)
    ->set_event_queue(app->get_event_queue())
    ->get_style()
    ->set_text_size(2)
    ->set_bg_color(GREEN)
    ->set_pressed_bg_color(blend_color(GREEN, BLACK, 128))
    ->set_fg_color(BLACK)
    ->set_pressed_fg_color(WHITE)
    ->set_border_color(GREEN)
    ->set_pressed_border_color(blend_color(GREEN, BLACK, 128));

    load_button
    ->set_message("Load")
    ->set_args((unsigned *)load_button)
    ->set_onrelease(open_slot_selection)
    ->set_event_queue(app->get_event_queue())
    ->get_style()
    ->set_text_size(2)
    ->set_bg_color(blend_color(BLUE, CYAN, 64))
    ->set_pressed_bg_color(blend_color(BLUE, BLACK, 128))
    ->set_fg_color(BLACK)
    ->set_pressed_fg_color(WHITE)
    ->set_border_color(blend_color(BLUE, CYAN, 64))
    ->set_pressed_border_color(blend_color(BLUE, BLACK, 128));

    connection_button
    ->set_message("WiFi")
    ->set_onrelease(switch_to_connection)
    ->set_event_queue(app->get_event_queue())
    ->get_style()
    ->set_text_size(2);

    information_button
    ->set_message("Info")
    ->set_onrelease(switch_to_information)
    ->set_event_queue(app->get_event_queue())
    ->get_style()
    ->set_text_size(2);

    clear_button
    ->set_message("C")
    ->set_onrelease(clear_button_cb)
    ->set_event_queue(app->get_event_queue())
    ->get_style()
    ->set_text_size(4)
    ->set_bg_color(RED)
    ->set_pressed_bg_color(blend_color(RED, BLACK, 160))
    ->set_fg_color(BLACK)
    ->set_pressed_fg_color(WHITE)
    ->set_border_color(RED)
    ->set_pressed_border_color(blend_color(RED, BLACK, 160))
    ->set_border_radius(7);

    slot_selection_window
    ->get_style()
    ->set_border_radius(3);

    slot_exit_button
    ->set_onrelease(exit_slot_selection)
    ->set_event_queue(app->get_event_queue())
    ->get_style()
    ->set_border_width(0)
    ->set_border_radius(6)
    ->set_bg_color(RED)
    ->set_pressed_bg_color(blend_color(RED, BLACK, 160));

    for (unsigned c = 0, i = 0; c < 3; ++c) {
        for (unsigned r = 0; r < 2; ++r, ++i) {

            char buffer[] = "Slot ""\x00\x00";
            itoa((r * 3) + c + 1, &buffer[5], 10);

            slot_buttons[i]
            ->set_message(buffer)
            ->set_onrelease(slot_selection_cb)
            ->set_event_queue(app->get_event_queue())
            ->get_style()
            ->set_text_size(2)
            ->set_border_width(0);

            unsigned *args = (unsigned *)new (std::nothrow) slot_button_args_t {
                .number = (r * 3) + c + 1,
                .caller = nullptr
            };
            if (args == nullptr) {
                err("Error while creatings args for slot buttons");
            }
            slot_buttons[i]->set_args(args);
        }
    }
    slot_buttons[0]->get_style()->set_bg_color(blend_color(RED, YELLOW, 128))->set_pressed_bg_color(blend_color(blend_color(RED, YELLOW, 128), BLACK, 128));
    slot_buttons[2]->get_style()->set_bg_color(blend_color(GREEN, CYAN, 128))->set_pressed_bg_color(blend_color(blend_color(GREEN, CYAN, 128), BLACK, 128));
    slot_buttons[4]->get_style()->set_bg_color(blend_color(BLUE, MAGENTA, 128))->set_pressed_bg_color(blend_color(blend_color(BLUE, MAGENTA, 128), BLACK, 128));

    slot_buttons[1]->get_style()->set_bg_color(blend_color(RED, MAGENTA, 128))->set_pressed_bg_color(blend_color(blend_color(RED, MAGENTA, 128), BLACK, 128));
    slot_buttons[3]->get_style()->set_bg_color(blend_color(GREEN, YELLOW, 128))->set_pressed_bg_color(blend_color(blend_color(GREEN, YELLOW, 128), BLACK, 128));
    slot_buttons[5]->get_style()->set_bg_color(blend_color(BLUE, CYAN, 128))->set_pressed_bg_color(blend_color(blend_color(BLUE, CYAN, 128), BLACK, 128));

    communication_status_label
    ->set_event_queue(app->get_event_queue())
    ->set_onrelease(exit_slot_selection)
    ->get_style()
    ->set_text_size(2)
    ->set_border_radius(0)
    ->set_border_width(0);

    tools_window->send_front(slot_selection_window, 0);

    slot_selection_window->send_front(communication_status_label, 0);

    communication_status_label->set_visibility(false);
    slot_selection_window->set_visibility(false);
    communication_status_label->clear();
    slot_selection_window->clear();
}

void init_connection_view() {

    connection_view = View::create(app);
    if (connection_view == nullptr) {
        err("Error while creating connection view");
    }

    connection_back_button = Button::create(connection_view, 3, 15 - 12, 24, 24);
    if (connection_back_button == nullptr) {
        err("Error while creating connection back button");
    }

    connection_title = Label::create(connection_view, 30, 1, app->get_width() - 30 - 10, 27);
    if (connection_title == nullptr) {
        err("Error while creating connection title");
    }

    keyboard = Keyboard::create(connection_view);
    if (keyboard == nullptr) {
        err("ERror while creating keyboard");
    }

    connection_form_window = Window::create(connection_view, 5, 30, app->get_width() - 10, app->get_height() - 45);
    if (connection_form_window == nullptr) {
        err("Error while creating connection form window");
    }

    ssid_label = Label::create(connection_form_window, 10, 10, connection_form_window->get_width() - 20, 30);
    if (ssid_label == nullptr) {
        err("Error while creating ssid label");
    }
    ssid_box = TextBox::create(connection_form_window, 10, 47 - 4, connection_form_window->get_width() - 20, 34);
    if (ssid_box == nullptr) {
        err("Error while creating ssid box");
    }

    pass_label = Label::create(connection_form_window, 10, 90, connection_form_window->get_width() - 20, 30);
    if (pass_label == nullptr) {
        err("Error while creating pass_label");
    }
    pass_box = TextBox::create(connection_form_window, 10, 127 - 4, connection_form_window->get_width() - 20, 34);
    if (pass_box == nullptr) {
        err("Error while creating pass box");
    }

    addr_label = Label::create(connection_form_window, 10, 170, connection_form_window->get_width() - 20, 30);
    if (addr_label == nullptr) {
        err("Error while creating addr label");
    }
    addr_box = TextBox::create(connection_form_window, 10, 207 - 4, connection_form_window->get_width() - 20, 34);
    if (addr_box == nullptr) {
        err("Error while creating addr box");
    }

    connect_button = Button::create(connection_form_window, 10, 260, connection_form_window->get_width() - 20, 40);
    if (connect_button == nullptr) {
        err("Error while creating connect button");
    }

    status_label = Label::create(connection_form_window, 10, connection_form_window->get_height() - 30 - 5, connection_form_window->get_width() - 20, 30);
    if (status_label == nullptr) {
        err("Error while creating status label");
    }

    connection_back_button
    ->set_onrelease(switch_to_startup)
    ->set_event_queue(app->get_event_queue())
    ->set_message("x")
    ->get_style()
    ->set_text_size(2)
    ->set_bg_color(RED)
    ->set_fg_color(BLACK)
    ->set_pressed_bg_color(blend_color(RED, BLACK, 160))
    ->set_pressed_fg_color(WHITE)
    ->set_border_width(0)
    ->set_border_radius(14);

    connection_title
    ->set_message("Manage Connection")
    ->get_style()
    ->set_text_size(2)
    ->set_border_width(0)
    ->set_border_radius(0);

    keyboard
    ->set_event_queue(app->get_event_queue())
    ->set_visibility(false);
    keyboard->clear();

    connection_form_window
    ->get_style()
    ->set_border_radius(3);

    ssid_label
    ->set_message("WiFi SSID:")
    ->get_style()
    ->set_text_size(2)
    ->set_horizontal_alignment(LabelStyle::HorizontalAlignment::LEFT_ALIGN)
    ->set_border_radius(0)
    ->set_border_width(0);

    ssid_box
    ->set_event_queue(app->get_event_queue())
    ->set_onrelease(open_keyboard)
    ->set_args((unsigned *)ssid_box)
#ifdef DEFAULT_SECRET_SSID
    ->set_message(DEFAULT_SECRET_SSID)
#endif
    ->get_style()
    ->set_text_size(2)
    ->set_horizontal_alignment(LabelStyle::HorizontalAlignment::LEFT_ALIGN)
    ->set_border_radius(2)
    ->set_border_width(2)
    ->set_bg_color(blend_color(BLACK, BLUE, 18));

    pass_label
    ->set_message("WiFi Password:")
#ifdef DEFAULT_SERVER_ADDR
    ->set_message(DEFAULT_SERVER_ADDR)
#endif
    ->get_style()
    ->set_text_size(2)
    ->set_horizontal_alignment(LabelStyle::HorizontalAlignment::LEFT_ALIGN)
    ->set_border_radius(0)
    ->set_border_width(0);

    pass_box
    ->set_event_queue(app->get_event_queue())
    ->set_onrelease(open_keyboard)
    ->set_args((unsigned *)pass_box)
#ifdef DEFAULT_SECRET_PASS
    ->set_message(DEFAULT_SECRET_PASS)
#endif
    ->get_style()
    ->set_text_size(2)
    ->set_horizontal_alignment(LabelStyle::HorizontalAlignment::LEFT_ALIGN)
    ->set_border_radius(2)
    ->set_border_width(2)
    ->set_bg_color(blend_color(BLACK, BLUE, 18));

    addr_label
    ->set_message("Server IP:port")
    ->get_style()
    ->set_text_size(2)
    ->set_horizontal_alignment(LabelStyle::HorizontalAlignment::LEFT_ALIGN)
    ->set_border_radius(0)
    ->set_border_width(0);

    addr_box
    ->set_event_queue(app->get_event_queue())
    ->set_onrelease(open_keyboard)
    ->set_args((unsigned *)addr_box)
    ->get_style()
    ->set_text_size(2)
    ->set_horizontal_alignment(LabelStyle::HorizontalAlignment::LEFT_ALIGN)
    ->set_border_radius(2)
    ->set_border_width(2)
    ->set_bg_color(blend_color(BLACK, BLUE, 18));

    connect_button
    ->set_message("Connect")
    ->set_event_queue(app->get_event_queue())
    ->set_onrelease(connect_cb)
    ->get_style()
    ->set_text_size(2)
    ->set_border_radius(20)
    ->set_border_width(0)
    ->set_bg_color(blend_color(BLUE, CYAN, 192))
    ->set_fg_color(BLACK)
    ->set_pressed_bg_color(blend_color(BLUE, CYAN, 128))
    ->set_pressed_fg_color(WHITE);

    status_label
    ->set_message("NOT CONNECTED")
    ->get_style()
    ->set_text_size(2)
    ->set_border_radius(0)
    ->set_border_width(0)
    ->set_fg_color(GRAY);

    connection_view->send_front(keyboard, 0);

    { // optimization
        signed status = WiFi.status();
        if (status == WL_NO_MODULE) {
            status_label
            ->set_message("NO WIFI MODULE")
            ->get_style()
            ->set_fg_color(RED);
        }
    }
}

void init_information_view() {

    information_view = View::create(app);
    if (information_view == nullptr) {
        err("Error while creating information view");
    }

    information_back_button = Button::create(information_view, 3, 15 - 12, 24, 24);
    if (information_back_button == nullptr) {
        err("Error while creating information back button");
    }

    information_title = Label::create(information_view, 30, 3, app->get_width() - 30 - 10, 25);
    if (information_title == nullptr) {
        err("Error while creating information title");
    }

    information_form_window = Window::create(information_view, 5, 30, app->get_width() - 10, app->get_height() - 45);
    if (information_form_window == nullptr) {
        err("Error while creating information form window");
    }

    ssid_readonly_label = Label::create(information_form_window, 12, 20, (information_form_window->get_width() - 24) / 2 - 30, 35);
    if (ssid_readonly_label == nullptr) {
        err("Error while creating ssid readonly label");
    }

    ssid_readonly_box = Label::create(information_form_window, ssid_readonly_label->get_x() + ssid_readonly_label->get_width() - 1, ssid_readonly_label->get_y(), information_form_window->get_width() - ssid_readonly_label->get_x() - ssid_readonly_label->get_width() - 12, 35);
    if (ssid_readonly_box == nullptr) {
        err("Error while creating ssid readonly label");
    }

    ip_addr_label = Label::create(information_form_window, ssid_readonly_label->get_x(), ssid_readonly_box->get_y() + ssid_readonly_box->get_height() - 1, ssid_readonly_label->get_width(), ssid_readonly_label->get_height());
    if (ip_addr_label == nullptr) {
        err("Error while creating ssid readonly label");
    }

    ip_addr_box = Label::create(information_form_window, ssid_readonly_box->get_x(), ip_addr_label->get_y(), ssid_readonly_box->get_width(), ssid_readonly_box->get_height());
    if (ip_addr_box == nullptr) {
        err("Error while creating ssid readonly label");
    }

    dumblebots_qrcode = Bitmap::create(information_form_window, qrcode_bmp, (information_form_window->get_width() - 135) / 2, information_form_window->get_height() - 135 - 30, 135, 135);
    if (dumblebots_qrcode == nullptr) {
        err("Error while creating dumblebots qrcode");
    }
    dumblebots_website_label = Label::create(information_form_window, 10, information_form_window->get_height() - 25, information_form_window->get_width() - 20, 20);
    if (dumblebots_website_label == nullptr) {
        err("Error while creating dumblebots website label");
    }

    information_back_button
    ->set_onrelease(switch_to_startup)
    ->set_event_queue(app->get_event_queue())
    ->set_message("x")
    ->get_style()
    ->set_text_size(2)
    ->set_bg_color(RED)
    ->set_fg_color(BLACK)
    ->set_pressed_bg_color(blend_color(RED, BLACK, 160))
    ->set_pressed_fg_color(WHITE)
    ->set_border_width(0)
    ->set_border_radius(14);

    information_title
    ->set_message("Information")
    ->get_style()
    ->set_text_size(2)
    ->set_border_width(0)
    ->set_border_radius(0);

    information_form_window
    ->get_style()
    ->set_border_radius(3);

    ssid_readonly_label
    ->set_message("WiFi SSID ")
    ->get_style()
    ->set_text_size(2)
    ->set_horizontal_alignment(LabelStyle::HorizontalAlignment::RIGHT_ALIGN)
    ->set_border_radius(0)
    ->set_border_width(1);

    ssid_readonly_box
    ->get_style()
    ->set_text_size(2)
    ->set_horizontal_alignment(LabelStyle::HorizontalAlignment::CENTER_ALIGN)
    ->set_border_radius(0)
    ->set_border_width(1);

    ip_addr_label
    ->set_message("IP Address ")
    ->get_style()
    ->set_text_size(2)
    ->set_horizontal_alignment(LabelStyle::HorizontalAlignment::RIGHT_ALIGN)
    ->set_border_radius(0)
    ->set_border_width(1);

    ip_addr_box
    ->get_style()
    ->set_text_size(2)
    ->set_horizontal_alignment(LabelStyle::HorizontalAlignment::CENTER_ALIGN)
    ->set_border_radius(0)
    ->set_border_width(1);

    dumblebots_website_label
    ->set_message("Powered by https://dumblebots.com")
    ->get_style()
    ->set_text_size(1)
    ->set_border_radius(0)
    ->set_border_width(0);

    information_form_window->send_back(dumblebots_qrcode, 100);
}

/* ----- Start of Callback functions ----- */

void switch_to_startup(unsigned *args) {
    connection_back_button->set_onrelease(switch_to_startup);
    information_back_button->set_onrelease(switch_to_startup);
    app->make_active_view(startup_view);
}

void switch_to_connection(unsigned *args) {

    signed status = WiFi.status();

    if (status == WL_CONNECTED) {
        status_label
        ->set_message("CONNECTED")
        ->get_style()
        ->set_fg_color(GREEN);
    }
    else {
        status_label
        ->set_message("NOT CONNECTED")
        ->get_style()
        ->set_fg_color(GRAY);
    }

    keyboard->set_visibility(false);
    app->make_active_view(connection_view);
}

void switch_to_information(unsigned *args) {

    signed status = WiFi.status();
    char buffer[64];

    if (status == WL_CONNECTED) {

        ssid_box->get_message().toCharArray(buffer, sizeof(buffer) / sizeof(buffer[0]), 0);

        ssid_readonly_box
        ->set_message(buffer)
        ->get_style()
        ->set_fg_color(WHITE);

        WiFi.localIP().toString().toCharArray(buffer, sizeof(buffer) / sizeof(buffer[0]), 0);

        ip_addr_box
        ->set_message(buffer)
        ->get_style()
        ->set_fg_color(WHITE);
    }
    else {

        ssid_readonly_box
        ->set_message("Not Connected")
        ->get_style()
        ->set_fg_color(RED);

        ip_addr_box
        ->set_message("-")
        ->get_style()
        ->set_fg_color(RED);
    }

    app->make_active_view(information_view);
}

void switch_to_main(unsigned *args) {
    canvas->set_pen_color(RED);
    size_selector->set_color(RED);
    connection_back_button->set_onrelease(switch_to_main);
    information_back_button->set_onrelease(switch_to_main);
    app->make_active_view(main_view);
}

void select_color_cb(unsigned *args) {
    size_selector->set_color(*(uint16_t *)args);
    canvas->set_pen_color(*(uint16_t *)args);
}

void select_pen_size_cb(unsigned *args) {
    canvas->set_pen_size(*args);
}

void clear_button_cb(unsigned *args) {
    canvas->clear_canvas();
}

void open_slot_selection(unsigned *args) {

    slot_button_args_t *button_args;

    for (auto &button : slot_buttons) {
        button_args = (slot_button_args_t *)button->get_args();
        button_args->caller = (Button *)args;
    }

    slot_selection_window->set_visibility(true);
    communication_status_label->set_visibility(false);
}

void exit_slot_selection(unsigned *args) {
    slot_selection_window->set_visibility(false);
}

void slot_selection_cb(unsigned *args) {

    slot_button_args_t *info = (slot_button_args_t *)args;

    if (info->caller == save_button) {
        canvas->set_args((unsigned *)info->caller);
        canvas->save_to_server(info->number);
    }
    else {
        canvas->set_args((unsigned *)info->caller);
        canvas->load_from_server(info->number);
    }
}

void server_connection_failure_cb(unsigned *args) {

    communication_status_label
    ->set_message("FAILED TO CONNECT")
    ->get_style()
    ->set_fg_color(RED);

    communication_status_label->set_visibility(true);
}

void server_communication_failure_cb(unsigned *args) {

    communication_status_label
    ->set_message("COMMUNICATION INTERRUPTED")
    ->get_style()
    ->set_fg_color(RED);

    communication_status_label->set_visibility(true);
}

void server_success_cb(unsigned *args) {

    communication_status_label
    ->set_message((args == (unsigned *)save_button) ? "IMAGE SAVED" : "IMAGE LOADED")
    ->get_style()
    ->set_fg_color(GREEN);

    communication_status_label->set_visibility(true);
}

void open_keyboard(unsigned *args) {

    keyboard->set_visibility(true);
    keyboard->set_shift_index(0);

    if (args == (unsigned *)ssid_box) {
        keyboard->set_onrelease(ssid_box_edit_cb);
    }
    else if (args == (unsigned *)pass_box) {
        keyboard->set_onrelease(pass_box_edit_cb);
    }
    else if (args == (unsigned *)addr_box) {
        keyboard->set_onrelease(addr_box_edit_cb);
    }
}

void ssid_box_edit_cb(unsigned *args) {

    Keyboard::keyboard_state_t *info = (Keyboard::keyboard_state_t *)args;

    if (info->special != 0) {
        if (info->backspace != 0) {
            ssid_box->pop_from_message();
        }
        return;
    }
    if (ssid_box->get_message_len() < 16) {
        ssid_box->append_to_message((char)(info->key));
    }
}

void pass_box_edit_cb(unsigned *args) {

    Keyboard::keyboard_state_t *info = (Keyboard::keyboard_state_t *)args;

    if (info->special != 0) {
        if (info->backspace != 0) {
            pass_box->pop_from_message();
        }
        return;
    }
    if (pass_box->get_message_len() < 16) {
        pass_box->append_to_message((char)(info->key));
    }
}

void addr_box_edit_cb(unsigned *args) {

    Keyboard::keyboard_state_t *info = (Keyboard::keyboard_state_t *)args;

    if (info->special != 0) {
        if (info->backspace != 0) {
            addr_box->pop_from_message();
        }
        return;
    }

    if ((addr_box->get_message_len() < (15 + 1 + 5))
        && (info->key == '.'
            || info->key == ':'
            || ('0' <= info->key && info->key <= '9'))) {
        addr_box->append_to_message((char)(info->key));
    }
}

void connect_cb(unsigned *args) {

    signed status = WL_IDLE_STATUS;

    if (WiFi.status() == WL_NO_MODULE) {
        status_label
        ->set_message("NO WIFI MODULE")
        ->get_style()
        ->set_fg_color(RED);

        return;
    }
    if (!verify_server()) {
        status_label
        ->set_message("INVALID IP/PORT")
        ->get_style()
        ->set_fg_color(RED);

        canvas->set_server_addr("", 0);
        return;
    }

    status_label
    ->set_message("CONNECTING...")
    ->get_style()
    ->set_fg_color(BLUE);

    keyboard->set_visibility(false);

    //!
    app
    ->collect_dirty_widgets()
    ->update_dirty_widgets();

    app->push_event({try_connect, nullptr});
}

bool verify_server() {

    char addr[15 + 1 + 5 + 1];
    unsigned addr_len {0};

    char *ip {nullptr};
    char *port {nullptr};
    char *delim {nullptr};

    unsigned server_port;

    addr_box->get_message().toCharArray(addr, sizeof(addr) / sizeof(addr[0]));
    addr_len = addr_box->get_message_len();

    if (addr_len == 0) {
        return false;
    }

    delim = strchr(addr, ':');
    if (delim == NULL
        || delim == addr
        || delim == &addr[addr_len - 1]) {
        return false;
    }


    ip = addr;
    port = delim + 1;
    *delim = NULL;

    {
        signed temp;
        temp = atoi(port);

        if (temp < 0 || temp > 65535) {
            return false;
        }

        server_port = temp;
    }
    {
        char *saveptr {nullptr};
        unsigned num_dots {0};
        unsigned len {0};
        signed temp {0};

        for (char *ptr = ip; *ptr != NULL; ++ptr) {
            if (*ptr == '.') {
                ++num_dots;
            }
        }


        if (num_dots != 3) {
            return false;
        }


        for (char *ptr = strtok_r(ip, ".", &saveptr); ptr != nullptr; ptr = strtok_r(NULL, ".", &saveptr)) {

            if (ptr != ip) {
                *(ptr - 1) = '.';
            }

            len = strlen(ptr);
            temp = atoi(ptr);

            if (len == 0 || temp < 0 || temp > 255) {
                return false;
            }

            if (len > 1 && ptr[0] == '0') {
                return false;
            }

            for (char *subptr = ptr; subptr != &ptr[len]; ++subptr) {
                if (*subptr < '0' || *subptr > '9') {
                    return false;
                }
            }
        }

    }

    canvas->set_server_addr(ip, server_port);
    return true;
}

void try_connect(unsigned *args) {

    constexpr static unsigned MAX_WIFI_RETRY = 2;

    char ssid[17];
    char pass[17];

    ssid_box->get_message().toCharArray(ssid, sizeof(ssid) / sizeof(ssid[0]));
    pass_box->get_message().toCharArray(pass, sizeof(pass) / sizeof(pass[0]));

    signed status = WL_IDLE_STATUS;
    unsigned attempts_left = MAX_WIFI_RETRY;

    while (attempts_left--) {

        status = WiFi.begin(ssid, pass);

        delay(250);

        if (status == WL_CONNECTED) {

            status_label
            ->set_message("CONNECTED")
            ->get_style()
            ->set_fg_color(GREEN);

            return;
        }
    }

    status_label
    ->set_message("FAILED TO CONNECT")
    ->get_style()
    ->set_fg_color(RED);
}

/* ----- ----- */

[[noreturn]]
void err(const char msg[]) {
    Serial.println(msg);
    for (;;);
}
