#include "compressedCanvas.h"

unsigned CompressedCanvas::compress(uint8_t *codes, uint16_t count) {

    unsigned segmentCount = 0;

    for (unsigned l = 0, r; l < count; l = r) {
        for (r = l; r <= count; ++r) {
            if (r == count || codes[l] != codes[r]) {
                break;
            }
        }

        if (++segmentCount >= NUM_SEGMENTS) { // this segment can not be added

            segmentInfo.pixelCount = l;
            segmentInfo.segmentCount = segmentCount - 1;

            return l;
        }

        segment_t *curSegment = (segment_t *)&segments[segmentCount - 1];
        curSegment->code = codes[l];
        curSegment->size = r - l;
    }

    segmentInfo.pixelCount = count;
    segmentInfo.segmentCount = segmentCount;

    return count;
}

unsigned CompressedCanvas::uncompress(uint8_t *codes) const {

    const unsigned segmentCount = segmentInfo.segmentCount;
    const unsigned pixelCount = segmentInfo.pixelCount;

    if (segmentCount == 0) {
        return 0;
    }

    for (unsigned s = 0, idx = 0; s < segmentCount; ++s) {

        unsigned code = ((segment_t *)&segments[s])->code;
        unsigned size = ((segment_t *)&segments[s])->size;

        while (size--) {
            codes[idx++] = code;
        }
    }

    return pixelCount;
}

uint16_t CompressedCanvas::segmentCount() const {
    return segmentInfo.segmentCount;
}

uint16_t CompressedCanvas::pixelCount() const {
    return segmentInfo.pixelCount;
}

uint16_t* CompressedCanvas::getSegmentArray() {
    return segments;
}