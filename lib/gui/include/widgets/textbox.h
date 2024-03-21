#ifndef __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_TEXTBOX_H__
#define __ARDUINO_WIFI_TFT_LCD_CANVAS_APP_WIDGETS_TEXTBOX_H__

#include "label.h"

class TextBox : public Label, public InteractiveWidget {

protected:

    constexpr static unsigned DEBOUNCE_THRESH = 200;

    RingQueueInterface<callback_event_t> *event_queue;

    bool pressed {false};
    bool enabled {true};

    unsigned *args {nullptr};

    callback_t on_press {};
    callback_t on_release {};

    unsigned last_press_epoch {0};

public:

    TextBox() = delete;

    static TextBox *create(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);

    // Label overrides

    bool propagate_press(unsigned x, unsigned y) override;
    bool propagate_release(unsigned x, unsigned y) override;

    // InteractiveWidget overrides

    TextBox *set_onpress(callback_t callback)  override;
    TextBox *reset_onpress()  override;

    TextBox *set_onrelease(callback_t callback)  override;
    TextBox *reset_onrelease()  override;

    TextBox *set_event_queue(RingQueueInterface<callback_event_t> *new_event_queue) override;
    TextBox *reset_event_queue() override;

    TextBox *set_args(unsigned *new_args) override;
    TextBox *reset_args() override;
    unsigned *get_args() override;

    bool is_pressed() const override;

    TextBox *set_interactable(bool new_state) override;
    bool get_interactable() const override;

protected:

    TextBox(Frame *parent, unsigned x, unsigned y, unsigned width, unsigned height);
};

#endif
