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

    if (!client.connect(IPAddress(serverIP), serverPort)) {
        Serial.println("Connection Failed");
        return;
    }

    uint16_t imageHeight = canvas->height() - 2;
    uint16_t imageWidth = canvas->width() - 2;

    client.write("\x02", 1);
    client.write(&id, 1);
    client.write((const char *)&imageHeight, sizeof(imageHeight));
    client.write((const char *)&imageWidth, sizeof(imageWidth));

    client.flush();

    for (int row = 0; row < imageHeight; ++row) {

        for (int col = 0; col < imageWidth; ++col) {

            while (client.connected() && !client.available());

            if (!client.connected()) {
                Serial.print("Communication with client failed at row ");
                Serial.println(row);

                return;
            }

            rowbuf.color[col] = code2color(client.read());
        }

        for (uint16_t j = 0; j < imageWidth; ++j) {
            canvas->writePixel(row + 1, j + 1, rowbuf.color[j]);
        }

        for (uint16_t j = 0; j < imageWidth; ++j) {
            rowbuf.code[j] = color2code(rowbuf.color[j]); //! this is very risky code
        }

        compressed[row].compress(rowbuf.code, imageWidth); //! this code can be optimized
    }

    while (client.connected()) {}
    client.stop();
}

void CanvasClient::saveCanvas(uint8_t id, Compressor<CANVAS_BUFFER_MAX_SEGMENTS> *compressed) {

    uint16_t imageHeight = canvas->height() - 2;
    uint16_t imageWidth = canvas->width() - 2;

    if (client.connect(IPAddress(serverIP), serverPort) == 0) {
        Serial.println("Connection Failed");
        return;
    }

    client_buffer.append((const uint8_t *)"\x01", 1);
    client_buffer.append(&id, 1);

    client_buffer.append((const uint8_t *)&imageHeight, 2);
    client_buffer.append((const uint8_t *)&imageWidth, 2);

    for (unsigned i = 0; i < imageHeight; ++i) {

        bool compressable = true;

        if (compressed[i].getPrefixSize() == imageWidth) {
            compressor.transfer(compressed);
        }
        else {
            for (unsigned j = compressed[i].uncompress(rowbuf.code); j < imageWidth; ++j) {
                rowbuf.code[j] = color2code(canvas->readPixel(i + 1, j + 1));
            }
            compressable = compressor.compress(rowbuf.code, imageWidth);
        }

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
