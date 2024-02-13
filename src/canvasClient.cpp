#include "canvasClient.h"

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

void CanvasClient::loadCanvas(uint8_t id) {

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

            colorBuffer[col] = code2color(client.read());
        }

        for (uint16_t j = 0; j < imageWidth; ++j) {
            canvas->writePixel(row + 1, j + 1, colorBuffer[j]);
        }
    }

    while (client.connected()) {}
    client.stop();
}

void CanvasClient::saveCanvas(uint8_t id) {

    if (client.connect(IPAddress(serverIP), serverPort) == 0) {
        Serial.println("Connection Failed");
        return;
    }

    uint16_t imageHeight = canvas->height() - 2;
    uint16_t imageWidth = canvas->width() - 2;

    CompressedRow compressed;

    client.write("\x01", 1);
    client.write(&id, 1);
    client.write((const char *)&imageHeight, sizeof(imageHeight));
    client.write((const char *)&imageWidth, sizeof(imageWidth));

    for (uint16_t i = 0; i < imageHeight; ++i) {

        for (uint16_t j = 0; j < imageWidth; ++j) {
            codeBuffer[j] = color2code(canvas->readPixel(i + 1, j + 1));
        }

        if (compressed.compress(codeBuffer, imageWidth)) {

            uint8_t numSegments = compressed.getNumSegments();
            unsigned size = numSegments * sizeof(uint16_t);

            int _ = client.write(&numSegments, 1);
            _ += client.write((uint8_t *)(compressed.getSegmentArray()), size);

            if (_ != size+1 || !client.connected()) {

                Serial.print("Communication with server failed at row ");
                Serial.println(i);
                return;
            }
        }
        else {

            int _ = client.write('\x00');
            _ += client.write(codeBuffer, imageWidth);

            if (_ != imageWidth+1 || !client.connected()) {

                Serial.print("Communication with server failed at row ");
                Serial.println(i);
                return;
            }
        }
    }
    client.flush();

    while (client.connected()) {}
    client.stop();
}
