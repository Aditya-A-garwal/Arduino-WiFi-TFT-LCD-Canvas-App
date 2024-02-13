#ifndef __CANVAS_CLIENT_H__
#define __CANVAS_CLIENT_H__

#include "Arduino.h"
#include "WiFiS3.h"

#include "constants.h"
#include "helper.h"

#include "widgets/canvas.h"

#include "stdint.h"

class CompressedRow {

    constexpr static unsigned NUM_SEGMENTS = 24;

    struct segment {
        uint16_t code: 4;
        uint16_t size: 9;
        uint16_t _: 3;
    };
    static_assert(sizeof(segment) == sizeof(uint16_t));

    uint16_t segments[NUM_SEGMENTS];
    uint16_t numSegments;

public:

    CompressedRow() {}

    bool compress(uint8_t *codes, uint16_t count) {

        numSegments = 0;

        for (unsigned l = 0, r; l < count; ) {
            for (r = l; r < count; ++r) {
                if (codes[r] == codes[l]) {
                    if (r < count-1) {
                        continue;
                    }
                    r = count;
                }
                break;
            }

            if (++numSegments >= NUM_SEGMENTS) {
                numSegments = 0;
                return false;
            }

            segment *curSegment = (segment *)(&segments[numSegments - 1]);
            curSegment->code = codes[l];
            curSegment->size = r - l;

            l = r;
        }

        return true;
    }

    uint16_t getNumSegments() {
        return numSegments;
    }

    uint16_t getSegment(uint16_t i) {
        return segments[i];
    }

    uint16_t *getSegmentArray() {
        return segments;
    }

private:
};

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
    uint8_t codeBuffer[512];

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

#endif
