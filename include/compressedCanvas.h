#ifndef __COMPRESSED_CANVAS_H__
#define __COMPRESSED_CANVAS_H__

#include "Arduino.h"
#include "WiFiS3.h"

#include "constants.h"
#include "helper.h"

#include "widgets/canvas.h"

#include "stdint.h"

class CompressedCanvas {

    constexpr static unsigned NUM_SEGMENTS = 18;

    struct segment {
        uint16_t code: 4;
        uint16_t size: 9;
        uint16_t _: 3;
    };
    static_assert(sizeof(segment) == sizeof(uint16_t));

    uint16_t segments[NUM_SEGMENTS];
    uint16_t numSegments;

public:

    bool compress(uint8_t *codes, uint16_t count);
    bool uncompress(uint8_t *codes);

    uint16_t getNumSegments();
    uint16_t *getSegmentArray();

private:
};

#endif
