#ifndef __TOUCH_H__
#define __TOUCH_H__

void toDisplayMode();
void convertTouchCoors(uint16_t tx, uint16_t ty, uint16_t *xptr, uint16_t *yptr);
void getTouchCoors(uint16_t *xptr, uint16_t *yptr);

#endif
