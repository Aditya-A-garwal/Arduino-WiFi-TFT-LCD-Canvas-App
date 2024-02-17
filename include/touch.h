#ifndef __TOUCH_H__
#define __TOUCH_H__

void toDisplayMode();
void convertTouchCoors(unsigned tx, unsigned ty, unsigned *xptr, unsigned *yptr);
void getTouchCoors(unsigned *xptr, unsigned *yptr);

#endif
