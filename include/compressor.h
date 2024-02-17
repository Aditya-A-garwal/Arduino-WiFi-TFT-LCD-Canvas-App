#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__

#include "Arduino.h"
#include "WiFiS3.h"

#include "constants.h"
#include "helper.h"

#include "stdint.h"

struct segment_t {
    uint16_t code: 4;
    uint16_t size: 11;
    uint16_t flag: 1; //todo
};

struct count_info_t {
    uint16_t segmentCount: 6;
    uint16_t pixelCount: 10;
};

static_assert(sizeof(segment_t) == sizeof(uint16_t));
static_assert(sizeof(count_info_t) == sizeof(uint16_t));

template <unsigned MAX_SEGMENTS>
class Compressor {

    segment_t segments[MAX_SEGMENTS];
    count_info_t segmentInfo;

public:

    unsigned compress(uint8_t *codes, unsigned count);
    unsigned uncompress(uint8_t *codes);

    unsigned getPrefixSize() const;
    unsigned getSegmentCount() const;

    template <unsigned NUM_SEGMENTS_OTHER>
    void transfer(Compressor<NUM_SEGMENTS_OTHER> *other);

    const uint16_t *getRawSegmentAr() const;

private:

};

template <unsigned MAX_SEGMENTS>
unsigned Compressor<MAX_SEGMENTS>::compress(uint8_t *codes, unsigned count) {

    unsigned finished = 0;

    for (unsigned l = 0, r; l < count; l = r) {
        for (r = 1 + l; r <= count; ++r) {
            if (r == count || codes[l] != codes[r]) {
                break;
            }
        }

        if (++finished > MAX_SEGMENTS) {

            segmentInfo.pixelCount = l;
            segmentInfo.segmentCount = finished - 1;

            return l;
        }

        segments[finished - 1].code = codes[l];
        segments[finished - 1].size = r - l;
    }

    segmentInfo.pixelCount = count;
    segmentInfo.segmentCount = finished;

    return count;
}

template <unsigned MAX_SEGMENTS>
unsigned Compressor<MAX_SEGMENTS>::uncompress(uint8_t *codes) {

    const unsigned segmentCount = segmentInfo.segmentCount;
    const unsigned pixelCount = segmentInfo.pixelCount;

    if (segmentCount == 0) {
        return 0;
    }

    for (unsigned s = 0, idx = 0; s < segmentCount; ++s) {

        uint8_t code = segments[s].code;
        unsigned size = segments[s].size;

        while (size--) {
            codes[idx++] = code;
        }
    }

    return pixelCount;
}

template <unsigned MAX_SEGMENTS>
unsigned Compressor<MAX_SEGMENTS>::getPrefixSize() const {
    return segmentInfo.pixelCount;
}

template <unsigned MAX_SEGMENTS>
unsigned Compressor<MAX_SEGMENTS>::getSegmentCount() const {
    return segmentInfo.segmentCount;
}

template <unsigned MAX_SEGMENTS>
template <unsigned NUM_SEGMENTS_OTHER>
void Compressor<MAX_SEGMENTS>::transfer(Compressor<NUM_SEGMENTS_OTHER> *other) {

    segmentInfo.pixelCount = other->getPrefixSize();
    segmentInfo.segmentCount = other->getSegmentCount();

    memcpy(segments, other->getRawSegmentAr(), sizeof(uint16_t) * segmentInfo.segmentCount);
}

template <unsigned MAX_SEGMENTS>
const uint16_t *Compressor<MAX_SEGMENTS>::getRawSegmentAr() const {
    return (const uint16_t *)segments;
}

#endif
