#include "Arduino.h"
#include "TouchScreen.h"

#include "constants.h"
#include "helper.h"

#include "touch.h"

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void toDisplayMode() {

    pinMode(XM, OUTPUT);
    pinMode(XP, OUTPUT);
    pinMode(YM, OUTPUT);
    pinMode(YP, OUTPUT);
}

void convertTouchCoors(unsigned tx, unsigned ty, unsigned *xptr, unsigned *yptr) {

    tx = constrain(tx, XBEGIN, XEND);
    ty = constrain(ty, YBEGIN, YEND);

    tx = map(tx, XBEGIN, XEND, 0, 319);
    ty = map(ty, YBEGIN, YEND, 479, 0);

    *xptr = tx;
    *yptr = ty;
}

void getTouchCoors(unsigned *xptr, unsigned *yptr) {

    TSPoint p;

    for (;;) {

        p = ts.getPoint();
        if (inRange(p.z, PRESSURE_LEFT, PRESSURE_RIGHT)) {
            break;
        }
    }
    convertTouchCoors(p.x, p.y, xptr, yptr);
    toDisplayMode();
}
