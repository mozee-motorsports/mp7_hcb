/*
 * hcb.h
 *
 * Library for the hybrid control board
 * 
 *  Last Modified: Oct 19, 2026
 *      By Author: montemayorm
 *
 *  Created on: Oct 10, 2023
 *      Author: montemayorm
 */

#ifndef HCB_H
#define HCB_H

#include "mp7_fdcan.h"

void HCB_Init(void);
void FDCAN_Init(void);
eCAN_Error HCB_HandleCANRxMessage(const sCAN_Message *message);
eCAN_Error HCB_RegisterR2DPeriodicTransmit(void);

#endif // HCB_H
