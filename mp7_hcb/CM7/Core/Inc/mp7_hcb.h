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
#include "sevcon_pdo.h"

void HCB_Init(void);
void FDCAN_Init(void);

/* Poll both protocols on their explicitly selected buses. These functions and
 * getters run in the main-loop context; do not read the cache concurrently in an ISR.
 */
FDCAN_HandleTypeDef *HCB_GetVehicleCan(void);
eCAN_Error HCB_ProcessCANRx(uint32_t now_ms);
const SevconPdoFeedback *HCB_GetSevconFeedback(void);
eCAN_Error HCB_HandleCANRxMessage(const sCAN_Message *message);
eCAN_Error HCB_RegisterR2DPeriodicTransmit(void);

#endif // HCB_H
