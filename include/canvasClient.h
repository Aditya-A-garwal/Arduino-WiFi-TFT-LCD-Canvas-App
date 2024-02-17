#ifndef __CANVAS_CLIENT_H__
#define __CANVAS_CLIENT_H__

#include "Arduino.h"
#include "WiFiS3.h"

#include "constants.h"
#include "helper.h"

#include "widgets/canvas.h"
#include "compressor.h"

#include "stdint.h"

class CanvasClient {

    constexpr static unsigned CLIENT_BUFFER_LEN = 4096;
    constexpr static unsigned ROW_BUFFER_LEN = 512;

    constexpr static unsigned MAX_SSID_LEN = 64;
    constexpr static unsigned MAX_PASS_LEN = 64;

    constexpr static unsigned IP_LEN = 15;

    struct client_buffer_t {

        uint8_t bytes[CLIENT_BUFFER_LEN];
        unsigned size = 0;
        unsigned totalSent = 0;

        bool has_space(unsigned insertsize) {

            return (size + insertsize) <= CLIENT_BUFFER_LEN;
        }

        void append(const uint8_t *insertbytes, unsigned insertsize) {

            memcpy(&bytes[size], insertbytes, insertsize);
            size += insertsize;
        }

        bool flush(WiFiClient *client) {

            if (size == 0) {
                return true;
            }

            unsigned sent = client->write(bytes, size);
            if (sent != size) {
                return false;
            }
            totalSent += sent;

            size = 0;
            return true;
        }
    };

    union row_buffer_t {
        uint16_t color[ROW_BUFFER_LEN];
        uint8_t code[ROW_BUFFER_LEN];
    };

    static client_buffer_t client_buffer;
    static row_buffer_t rowbuf;
    static Compressor<CLIENT_BUFFER_MAX_SEGMENTS> compressor;

    Canvas *canvas;

    char ssid[MAX_SSID_LEN];
    char pass[MAX_PASS_LEN];

    char serverIP[IP_LEN];
    uint16_t serverPort;

    WiFiClient client;

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

    void loadCanvas(uint8_t id, Compressor<CANVAS_BUFFER_MAX_SEGMENTS> *compressed);
    void saveCanvas(uint8_t id, Compressor<CANVAS_BUFFER_MAX_SEGMENTS> *compressed);

private:

};

void handleConnection();
void transmitCanvas();
void loadCanvas();

#endif
