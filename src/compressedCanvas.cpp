#include "compressedCanvas.h"

bool CompressedCanvas::compress(uint8_t *codes, uint16_t count) {

    numSegments = 0;

    for (unsigned l = 0, r; l < count; l = r) {

        for (r = l; r <= count; ++r) {

            if (r == count || codes[l] != codes[r]) {
                break;
            }
        }

        if (++numSegments >= NUM_SEGMENTS) {
            numSegments = 0;
            return false;
        }

        segment *curSegment = (segment *)&segments[numSegments - 1];
        curSegment->code = codes[l];
        curSegment->size = r - l;
    }

    return true;
}

bool CompressedCanvas::uncompress(uint8_t *codes) {

    if (numSegments == 0) {
        return false;
    }

    for (unsigned s = 0, idx = 0; s < numSegments; ++s) {

        unsigned code = ((segment *)&segments[s])->code;
        unsigned size = ((segment *)&segments[s])->size;

        while (size--) {
            codes[idx++] = code;
        }
    }

    return true;
}

uint16_t CompressedCanvas::getNumSegments() {
    return numSegments;
}

uint16_t* CompressedCanvas::getSegmentArray() {
    return segments;
}