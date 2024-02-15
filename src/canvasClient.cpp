#include "canvasClient.h"

bool RowCompressor::compress(uint8_t *codes, uint16_t count) {

    numSegments = 0;

    for (unsigned l = 0, r; l < count; ) {
        for (r = l; r < count; ++r) {
            if (codes[r] == codes[l]) {
                if (r == count || codes[l] != codes[r]) {
                    break;
                }
            }
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

uint16_t RowCompressor::getNumSegments() {
        return numSegments;
}

uint16_t* RowCompressor::getSegmentArray() {
    return segments;
}


CanvasClient::client_buffer_t CanvasClient::client_buffer;
CanvasClient::row_buffer_t CanvasClient::rowbuf;
RowCompressor CanvasClient::compressor;

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

            rowbuf.color[col] = code2color(client.read());
        }

        for (uint16_t j = 0; j < imageWidth; ++j) {
            canvas->writePixel(row + 1, j + 1, rowbuf.color[j]);
        }
    }

    while (client.connected()) {}
    client.stop();
}

void CanvasClient::saveCanvas(uint8_t id) {

    uint16_t imageHeight = canvas->height() - 2;
    uint16_t imageWidth = canvas->width() - 2;

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

    x = micros();
    if (client.connect(IPAddress(serverIP), serverPort) == 0) {
        Serial.println("Connection Failed");
        return;
    }
    connectionTime += micros() - x;

    x = micros();
    client_buffer.append((const uint8_t *)"\x01", 1);
    client_buffer.append(&id, 1);

    client_buffer.append((const uint8_t *)&imageHeight, 2);
    client_buffer.append((const uint8_t *)&imageWidth, 2);
    headerTime = micros() - x;

    for (uint16_t i = 0; i < imageHeight; ++i) {

        x = micros();
        for (unsigned j = 0; j < imageWidth; ++j) {
            rowbuf.code[j] = color2code(canvas->readPixel(i + 1, j + 1));
        }
        readTime += micros() - x;

        x = micros();
        bool compressable = compressor.compress(rowbuf.code, imageWidth);
        compressionTime += micros() - x;

        if (compressable) {

            x = micros();

            uint8_t numSegments = compressor.getNumSegments();
            unsigned size = numSegments * sizeof(uint16_t);

            if (!client_buffer.has_space(1 + size)) {
                if (!client_buffer.flush(&client)) {
                    client.stop();
                    Serial.println("error");
                    return;
                }
            }
            client_buffer.append(&numSegments, 1);
            client_buffer.append((const uint8_t *)compressor.getSegmentArray(), size);

            compressedTxTime += micros() - x;
        }
        else {

            x = micros();

            if (!client_buffer.has_space(1 + imageWidth)) {
                if (!client_buffer.flush(&client)) {
                    client.stop();
                    Serial.println("error");
                    return;
                }
            }
            client_buffer.append((const uint8_t *)"\x00", 1);
            client_buffer.append(rowbuf.code, imageWidth);

            rawTxTime += micros() - x;
        }
    }

    x = micros();
    if (!client_buffer.flush(&client)) {
        client.stop();
        Serial.println("error");
        return;
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
    Serial.print("Bytes sent: "); Serial.println(client_buffer.totalSent); client_buffer.totalSent = 0;
    Serial.println();
}
