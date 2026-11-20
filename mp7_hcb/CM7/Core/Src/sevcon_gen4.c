/*
 * sevcon_gen4.c
 *
 * Library for the sevcon Gen4 motor controller
 * 
 *  Last Modified: Oct 21, 2026
 *      By Author: montemayorm
 *
 *  Created on: Oct 21, 2026
 *      Author: montemayorm
 */
#include "mp7_fdcan.h"
#include "sevcon_gen4.h"

 /**
 * @brief Initializes the Sevcon controller and brings it into Operational state.
 *
 * @param[in,out] handle   Pointer to Sevcon instance handle.
 * @param[in]     hfdcan   Pointer to initialized STM32 FDCAN handle.
 * @param[in]     node_id  CANopen Node ID of Sevcon (typically 0x01).
 * @param[in]     limits   Pointer to safety and operational limit settings.
 * @return Sevcon_Status_t SEVCON_OK on successful initialization sequence.
 *
 */
Sevcon_Status_t Sevcon_Init(Sevcon_Handle_t *handle,
                            FDCAN_HandleTypeDef *hfdcan,
                            uint8_t node_id,
                            const Sevcon_SafetyLimits_t *limits)
{

    /* CAN Messages Sent:
    * 1. NMT Reset Communication / Node:
    *    - COB-ID:  0x000 (Standard 11-bit)
    *    - DLC:     2
    *    - Payload: [0x82, NodeID] or [0x81, NodeID]
    * 2. NMT Start Remote Node (Enter Operational mode, enables PDOs):
    *    - COB-ID:  0x000
    *    - DLC:     2
    *    - Payload: [0x01, NodeID]
    * 3. CiA 402 State Machine Bring-Up via RPDO1 / SDO (Object 0x6040):
    *    - Step A: Shutdown -> Controlword = 0x0006
    *      Payload: [0x06, 0x00, ...] -> transitions to "Ready to Switch On"
    *    - Step B: Switch On -> Controlword = 0x0007
    *      Payload: [0x07, 0x00, ...] -> transitions to "Switched On"
    *    - Step C: Enable Operation -> Controlword = 0x000F
    *      Payload: [0x0F, 0x00, ...] -> transitions to "Operation Enabled"
    */

    // setup the canfd peripheral
//    if(MP7_FDCAN_InitAndStart(hfdcan) != HAL_OK){
//        MP7_FDCAN_HandleError(HAL_ERROR);
//    }

    // Initialize the Sevcon handle with provided parameters
    handle->hfdcan = hfdcan;
    handle->node_id = node_id;
    handle->limits = *limits;

    return SEVCON_OK;
}
