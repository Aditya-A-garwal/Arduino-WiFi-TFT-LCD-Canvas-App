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

    uint16_t imageHeight = canvas->height() - 2;
    uint16_t imageWidth = canvas->width() - 2;

    CompressedRow compressed;

    unsigned x = 0;

    unsigned readTime = 0;

    unsigned compressionTime = 0;

    unsigned connectionTime = 0;
    unsigned stopTime = 0;

    unsigned headerTime = 0;
    unsigned compressedTxTime = 0;
    unsigned rawTxTime = 0;
    unsigned flushTime = 0;

    unsigned totalTime = 0;
    unsigned sendAmt = 0;

    x = micros();
    if (client.connect(IPAddress(serverIP), serverPort) == 0) {
        Serial.println("Connection Failed");
        return;
    }
    connectionTime += micros() - x;

    x = micros();
    buf.bytes[0] = '\x01';
    buf.bytes[1] = id;

    buf.bytes[2] = imageHeight & 0xFF;
    buf.bytes[3] = (imageHeight >> 8) & 0xFF;

    buf.bytes[4] = imageWidth & 0xFF;
    buf.bytes[5] = (imageWidth >> 8) & 0xFF;

    buf.used = 6;

    headerTime = micros() - x;

    for (uint16_t i = 0; i < imageHeight; ++i) {

        x = micros();
        for (unsigned j = 0; j < imageWidth; ++j) {
            codeBuffer[j] = color2code(canvas->readPixel(i + 1, j + 1));
        }
        readTime += micros() - x;

        x = micros();
        bool status = compressed.compress(codeBuffer, imageWidth);
        compressionTime += micros() - x;

        if (status) {

            x = micros();

            uint8_t numSegments = compressed.getNumSegments();
            unsigned size = numSegments * sizeof(uint16_t);

            if (buf.used+size+1 >= BUFFER_LEN) {

                int _ = client.write(buf.bytes, buf.used);
                if (_ != buf.used) {
                    client.stop();
                    Serial.println("Error");
                    return;
                }
                buf.used = 0;

                sendAmt += _;
            }

            buf.bytes[buf.used++] = numSegments;
            memcpy(&(buf.bytes[buf.used]), (uint8_t *)(compressed.getSegmentArray()), size);
            buf.used += size;

            compressedTxTime += micros() - x;
        }
        else {

            x = micros();

            if (imageWidth+buf.used+1 >= BUFFER_LEN) {

                int _ = client.write(buf.bytes, buf.used);
                if (_ != buf.used) {
                    client.stop();
                    Serial.println("Error");
                    return;
                }
                buf.used = 0;

                sendAmt += _;
            }

            buf.bytes[buf.used++] = '\x00';
            memcpy(&(buf.bytes[buf.used]), codeBuffer, imageWidth);
            buf.used += imageWidth;

            rawTxTime += micros() - x;
        }
    }

    x = micros();
    if (buf.used != 0) {
        int _ = client.write(buf.bytes, buf.used);
        if (_ != buf.used) {
            client.stop();
            Serial.println("Error");
            return;
        }
        sendAmt += buf.used;
    }
    rawTxTime += micros() - x;

    x = micros();
    client.flush();
    flushTime += micros() - x;

    x = micros();
    client.stop();
    stopTime += micros() - x;

    Serial.println();
    Serial.println();
    Serial.print("Connection Time: "); Serial.print(connectionTime / 1000); Serial.println(" ms"); totalTime += connectionTime;
    Serial.print("Stop Time: "); Serial.print(stopTime / 1000); Serial.println(" ms"); totalTime += stopTime;
    Serial.println();
    Serial.print("Read Time: "); Serial.print(readTime / 1000); Serial.println(" ms"); totalTime += readTime;
    Serial.print("Compression Time: "); Serial.print(compressionTime / 1000); Serial.println(" ms"); totalTime += compressionTime;
    Serial.println();
    Serial.print("Header Tx Time: "); Serial.print(headerTime / 1000); Serial.println(" ms"); totalTime += headerTime;
    Serial.print("Compressed Tx Time: "); Serial.print(compressedTxTime / 1000); Serial.println(" ms"); totalTime += compressedTxTime;
    Serial.print("Raw Tx Time: "); Serial.print(rawTxTime / 1000); Serial.println(" ms"); totalTime += rawTxTime;
    Serial.print("Flush Tx Time: "); Serial.print(flushTime / 1000); Serial.println(" ms"); totalTime += flushTime;
    Serial.println();
    Serial.print("Total Time: "); Serial.print(totalTime / 1000); Serial.println(" ms");
    Serial.println();
    Serial.print("Bytes sent: "); Serial.println(sendAmt);
    Serial.println();
}
