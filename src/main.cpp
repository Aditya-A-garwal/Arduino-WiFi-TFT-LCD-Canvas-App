#include "Arduino.h"

#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

#include "constants.h"
#include "helper.h"
#include "arduino_secrets.h"

#include "touch.h"

#include "widgets/logo.h"
#include "widgets/canvas.h"
#include "widgets/colorSelector.h"
#include "widgets/thicknessSelector.h"
#include "widgets/buttonGrid.h"
#include "widgets/button.h"

#include "canvasClient.h"
#include "compressedCanvas.h"


MCUFRIEND_kbv tft;

Logo logo(LOGO_X, LOGO_Y, LOGO_C, &tft);

Canvas canvas(CANVAS_X, CANVAS_Y, CANVAS_W, CANVAS_H, &tft);
ColorSelector colorSelector(COLOR_SELECTOR_X, COLOR_SELECTOR_Y, COLOR_SELECTOR_HPAD, COLOR_SELECTOR_VPAD, PAINT_RADIUS, &tft);
ThicknessSelector thicknessSelector(THICKNESS_SELECTOR_X, THICKNESS_SELECTOR_Y, THICKNESS_SELECTOR_PAD, &tft);
ButtonGrid slotSelector(SLOT_MENU_X, SLOT_MENU_Y, SLOT_MENU_W, SLOT_MENU_H, 2, 3, &tft);

Button loadButton(LOAD_X, LOAD_Y, LOAD_W, LOAD_H, LOAD_C, "LOAD", &tft);
Button saveButton(SAVE_X, SAVE_Y, SAVE_W, SAVE_H, SAVE_C, "SAVE", &tft);
Button clearButton(CLEAR_X, CLEAR_Y, CLEAR_W, CLEAR_H, CLEAR_C, "CLEAR", &tft);
Button infoButton(INFO_X, INFO_Y, INFO_W, INFO_H, INFO_C, "INFO", &tft);

CanvasClient client(&canvas);

GFXcanvas1 canvasBuffer(CANVAS_BUFFER_W, CANVAS_BUFFER_H);

CompressedCanvas compressed[CANVAS_H - 2];
uint8_t rawCode[CANVAS_W - 2] {};

void clearCompressedCanvas();

void executeSlotSelection(CanvasClient *client, void (CanvasClient::*method)(uint8_t, CompressedCanvas *));
void drawLowerWidgets();

void setup(void) {

    // Use Serial Monitor for debugging
    Serial.begin(9600);
    tft.begin(0x9486);

    client.setSSID(SECRET_SSID);
    client.setPass(SECRET_PASS);
    client.setServer(SERVER_IP, 5005);
    client.connect(8);

    tft.fillScreen(BLACK);

    logo.draw();
    canvas.draw();
    drawLowerWidgets();

    clearCompressedCanvas();
}

void loop(void) {

    uint16_t x, y;
    unsigned canvasUpdateTime = 0;
    getTouchCoors(&x, &y);

    // update each widget
    canvasUpdateTime = micros();
    if (canvas.update(x, y, thicknessSelector.getThickness(), colorSelector.getColor())) {

        const uint16_t cx = x - CANVAS_X - 1;
        const uint16_t cy = y - CANVAS_Y - 1;

        const uint16_t thickness = thicknessSelector.getThickness();
        const uint16_t color = colorSelector.getColor();
        const uint8_t code = color2code(color);

        const uint16_t row_origin = cy - thickness;
        const uint16_t col_origin = cx - thickness;

        canvasBuffer.fillCircle(thickness, thickness, thickness, 1);

        for (unsigned i = 0; i <= thickness*2; ++i) {

            if (!compressed[i + row_origin].uncompress(rawCode)) { //! it is possible for only a prefix to be compressed, dont ignore that case
                continue;
            }

            for (unsigned j = 0; j <= thickness*2; ++j) {

                if (canvasBuffer.getPixel(j, i)) {
                    rawCode[j + col_origin] = code;
                }
            }

            compressed[i + row_origin].compress(rawCode, CANVAS_W - 2);
        }
    }
    canvasUpdateTime = (micros() - canvasUpdateTime) / 1000;

    if (colorSelector.update(x, y)) {

        thicknessSelector.setColor(colorSelector.getColor());
        thicknessSelector.draw();
    }

    if (thicknessSelector.update(x, y)) {
        thicknessSelector.draw();
    }

    if (loadButton.update(x, y)) {

        slotSelector.draw();

        delay(500);

        for (;;) {

            getTouchCoors(&x, &y);

            if (slotSelector.update(x, y)) {

                executeSlotSelection(&client, &CanvasClient::loadCanvas);
                break;
            }

            delay(5);
        }
    }

    if (saveButton.update(x, y)) {

        slotSelector.draw();

        delay(500);

        for (;;) {

            getTouchCoors(&x, &y);

            if (slotSelector.update(x, y)) {

                executeSlotSelection(&client, &CanvasClient::saveCanvas);
                break;
            }

            delay(5);
        }
    }

    if (clearButton.update(x, y)) {
        canvas.clear();
        clearCompressedCanvas();
    }

    if (infoButton.update(x, y)) {

    }

    if (canvasUpdateTime < 5) {
        delay(5 - canvasUpdateTime);
    }
}

void clearCompressedCanvas() {
    memset(rawCode, 8, sizeof(rawCode));
    for (unsigned i = 0; i < CANVAS_H-2; ++i) {
        compressed[i].compress(rawCode, CANVAS_W-2);
    }
}

void executeSlotSelection(CanvasClient *client, void (CanvasClient::*method)(uint8_t, CompressedCanvas *)) {

    uint8_t selectedSlot = slotSelector.getSlot();

    if (selectedSlot != 0) {
        (client->*method)(selectedSlot, compressed);
    }

    slotSelector.clear();
    drawLowerWidgets();
}

void drawLowerWidgets() {

    colorSelector.draw();
    thicknessSelector.draw();

    loadButton.draw();
    saveButton.draw();
    clearButton.draw();
    infoButton.draw();
}
