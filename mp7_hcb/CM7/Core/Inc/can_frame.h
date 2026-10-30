#ifndef MP7_CAN_FRAME_H
#define MP7_CAN_FRAME_H
#include <stdint.h>

/* Transport boundary: standard Classical CAN DATA frames only.
 * Reject extended, FD and remote frames before constructing this type.
 * length is an ordinary byte count, not a HAL DLC constant.
 */
typedef struct {
    uint16_t id;
    uint8_t length;
    uint8_t data[8];
} CanFrame;
#endif
