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

    struct segment_t {
        uint16_t code: 4;
        uint16_t size: 9;
        uint16_t _: 3;
    };
    static_assert(sizeof(segment_t) == sizeof(uint16_t));

    struct count_info_t {
        uint16_t segmentCount: 6;
        uint16_t pixelCount: 10;
    };
    static_assert(sizeof(count_info_t) == sizeof(uint16_t));

    uint16_t segments[NUM_SEGMENTS];
    count_info_t segmentInfo;

public:

    unsigned compress(uint8_t *codes, uint16_t count);
    unsigned uncompress(uint8_t *codes) const;

    uint16_t segmentCount() const;
    uint16_t pixelCount() const;
    uint16_t *getSegmentArray();

private:
};

#endif
