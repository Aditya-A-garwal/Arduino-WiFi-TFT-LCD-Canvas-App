#include "canvasClient.h"

CanvasClient::client_buffer_t CanvasClient::client_buffer;
CanvasClient::row_buffer_t CanvasClient::rowbuf;

Compressor<CLIENT_BUFFER_MAX_SEGMENTS> CanvasClient::compressor;

CanvasClient::CanvasClient(Canvas *canvas)
    : canvas {canvas}
{}

void CanvasClient::setSSID(const char SSID[]) {

    strncpy(ssid, SSID, CanvasClient::MAX_SSID_LEN);
}

void CanvasClient::setPass(const char PASS[]) {

    strncpy(pass, PASS, CanvasClient::MAX_PASS_LEN);
}

void CanvasClient::setServer(const char serverIp[], const uint16_t port) {

    strncpy(serverIP, serverIp, IP_LEN);
    serverPort = port;
}

CanvasClient::ConnectionStatus CanvasClient::connect(uint16_t maxAttempt) {

    signed status = WL_IDLE_STATUS;

    if (WiFi.status() == WL_NO_MODULE) {
        return NO_MODULE;
    }

    while (maxAttempt--) {

        status = WiFi.begin(ssid, pass);

        delay(2500);

        if (status == WL_CONNECTED) {
            return CONNECTION;
        }
    }

    return NO_CONNECTION;
}

void CanvasClient::disconnect() {

    WiFi.end();
}

void CanvasClient::loadCanvas(uint8_t id, Compressor<CANVAS_BUFFER_MAX_SEGMENTS> *compressed) {

    uint16_t imageHeight = canvas->heightInternal();
    uint16_t imageWidth = canvas->widthInternal();

    if (!client.connect(IPAddress(serverIP), serverPort)) {
        Serial.println("Connection Failed");
        return;
    }

    client.write("\x02", 1);
    client.write(&id, 1);
    client.write((const char *)&imageHeight, sizeof(imageHeight));
    client.write((const char *)&imageWidth, sizeof(imageWidth));

    client.flush();

    for (unsigned row = 0; row < imageHeight; ++row) {

        while (!client.available());
        uint8_t mode = client.read();

        if (mode == 0) {

            for (unsigned col = 0; col < imageWidth; ++col) {

                while (!client.available());
                rowbuf.code[col] = client.read();
            }
        }
        else {

            uint8_t lo, hi, code;
            uint16_t segment, size;

            compressor.clear();
            for (unsigned s = 0, idx = 0; s < mode; ++s) {

                while (!client.available());
                lo = client.read();

                while (!client.available());
                hi = client.read();

                compressor.pushSegment(((uint16_t)hi << 8) | (uint16_t)lo);

                segment = ((uint16_t)hi << 8) | (uint16_t)lo;
                code = ((segment_t *)&segment)->code;
                size = ((segment_t *)&segment)->size;

                while (size--) {
                    rowbuf.code[idx++] = code;
                }
            }
        }

        for (unsigned j = 0; j < imageWidth; ++j) {
            canvas->writePixel(row, j, code2color(rowbuf.code[j]));
        }
        compressed[row].compress(rowbuf.code, imageWidth);
    }

    client.stop();
}

void CanvasClient::saveCanvas(uint8_t id, Compressor<CANVAS_BUFFER_MAX_SEGMENTS> *compressed) {

    uint16_t imageHeight = canvas->heightInternal();
    uint16_t imageWidth = canvas->widthInternal();

    if (client.connect(IPAddress(serverIP), serverPort) == 0) {
        Serial.println("Connection Failed");
        return;
    }

    client_buffer.append((const uint8_t *)"\x01", 1);
    client_buffer.append(&id, 1);

    client_buffer.append((const uint8_t *)&imageHeight, 2);
    client_buffer.append((const uint8_t *)&imageWidth, 2);

    for (unsigned i = 0; i < imageHeight; ++i) {

        bool compressable;

        for (unsigned j = compressed[i].uncompress(rowbuf.code); j < imageWidth; ++j) {
            rowbuf.code[j] = color2code(canvas->readPixel(i, j));
        }
        compressable = compressor.compress(rowbuf.code, imageWidth);

        if (compressable) {

            uint8_t numSegments = compressor.getSegmentCount();
            unsigned size = numSegments * sizeof(uint16_t);

            if (!client_buffer.has_space(1 + size)) {
                if (!client_buffer.flush(&client)) {
                    client.stop();
                    Serial.println("error");
                    return;
                }
            }
            client_buffer.append(&numSegments, 1);
            client_buffer.append((const uint8_t *)compressor.getRawSegmentAr(), size);
        }
        else {

            if (!client_buffer.has_space(1 + imageWidth)) {
                if (!client_buffer.flush(&client)) {
                    client.stop();
                    Serial.println("error");
                    return;
                }
            }
            client_buffer.append((const uint8_t *)"\x00", 1);
            client_buffer.append(rowbuf.code, imageWidth);
        }
    }

    if (!client_buffer.flush(&client)) {
        client.stop();
        Serial.println("error");
        return;
    }
    client.flush();
    client.stop();
}
