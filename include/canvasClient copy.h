#ifndef __CANVAS_CLIENT_H__
#define __CANVAS_CLIENT_H__

#include "Arduino.h"
#include "WiFiS3.h"

#include "constants.h"
#include "helper.h"

#include "widgets/canvas.h"

#include "stdint.h"

class CanvasClient {

    constexpr static int DISPLAYBUF_LEN = 512;

    constexpr static uint16_t MAX_SSID_LEN = 64;
    constexpr static uint16_t MAX_PASS_LEN = 64;

    constexpr static uint16_t IP_LEN = 15;

    Canvas *canvas;

    char ssid[MAX_SSID_LEN];
    char pass[MAX_PASS_LEN];

    char serverIP[IP_LEN];
    uint16_t serverPort;

    int status = WL_IDLE_STATUS;

    WiFiClient client;

    uint16_t colorBuffer[512];
    uint8_t displayBuffer[DISPLAYBUF_LEN];

public:

    enum ConnectionStatus {
        NO_MODULE,
        NO_CONNECTION,
        CONNECTION
    };

    CanvasClient(Canvas *canvas);

    void setSSID(const char SSID[]);
    void setPass(const char PASS[]);
    void setServer(const char serverIp[], const uint16_t port);

    ConnectionStatus connect(uint16_t maxAttempt);
    void disconnect();

    void loadCanvas(uint8_t id);
    void saveCanvas(uint8_t id);

private:

};

void handleConnection();
void transmitCanvas();
void loadCanvas();
// void transmitCanvas() {

//     volatile uint32_t readPixelTime = 0;
//     volatile uint32_t txTime = 0;
//     volatile uint32_t x;

//     if (client.connect(IPAddress(serverIp), serverPort) == 0) {
//         Serial.println("Connection Failed");
//         return;
//     }

//     x = micros();
//     client.write("\x01", 1);
//     client.write(&selectedSlot, 1);
//     client.write((const char *)&CANVAS_H, sizeof(CANVAS_H));
//     client.write((const char *)&CANVAS_W, sizeof(CANVAS_W));

//     client.flush();

//     txTime += micros() - x;

//     for (int row = 0; row < CANVAS_H; ++row) {

//         x = micros();
//         for (int col = 0; col < CANVAS_W; ++col) {
//             replyBuffer[col] = color2code(tft.readPixel(col + CANVAS_X, row + CANVAS_Y));
//         }
//         readPixelTime += micros() - x;

//         x = micros();
//         volatile int _ = client.write(replyBuffer, CANVAS_W);
//         txTime += micros() - x;

//         if (!client.connected()) {
//             Serial.print("Communication with server failed at row ");
//             Serial.println(row);

//             return;
//         }

//         if (_ != CANVAS_W) {
//             --row;
//         }
//     }

//     x = micros();
//     client.flush();
//     txTime += micros() - x;

//     while (client.connected()) {}
//     client.stop();

//     Serial.print("\nRead Time ");
//     Serial.print(readPixelTime / 1000);
//     Serial.println(" ms");

//     Serial.print("Tx Time ");
//     Serial.print(txTime / 1000);
//     Serial.println(" ms");
// }

#endif
