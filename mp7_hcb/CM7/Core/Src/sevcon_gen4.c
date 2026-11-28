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

#include <CO_app_STM32.h>
#include "mp7_fdcan.h"
#include "sevcon_gen4.h"
#include "CANOpen.h"

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
                            CO_t *stack,
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

    // Initialize the Sevcon handle with provided parameters
    handle->stack = stack;
    handle->node_id = node_id;
    handle->limits = *limits;

    // PUT IN PREOP MODE, ONLY TIME WHERE SDO'S CAN BE USED
    // add checks to these
    CO_NMT_sendCommand(handle->stack->NMT, CO_NMT_RESET_COMMUNICATION, handle->node_id);   // reset first
    CO_NMT_sendCommand(handle->stack->NMT, CO_NMT_ENTER_PRE_OPERATIONAL, handle->node_id); // preop mode

    CO_SDOclient_t *SDO_C = handle->stack->SDOclient; // Get the SDO client from the stack
    // sdo time :)

    return SEVCON_OK;
}

/**
 * @brief Write one 1–4 byte value to a remote CANopen object and wait for completion.
 * @param[in,out] SDO_C    Initialized SDO client available for this transfer.
 * @param[in] nodeId       Remote node ID, from 1 through 127.
 * @param[in] index        Object dictionary index.
 * @param[in] subIndex     Object dictionary subindex.
 * @param[in] data         Value encoded in CANopen wire byte order.
 * @param[in] dataSize     Number of value bytes required by the object.
 * @return CO_SDO_AB_NONE on confirmed completion, or an abort code on failure.
 */
CO_SDO_abortCode_t
write_SDO(CO_SDOclient_t *SDO_C, uint8_t node_id, uint16_t index, uint8_t sub_index, uint8_t *data, size_t data_size)
{

    if (SDO_C == NULL || data == NULL || data_size > 4) || data_size ==0) // CHECK FOR INCORRECT PARAMETERS
        // data size being incorrect for over 4, assuming we don't have SDO's larger than that
        {
            return CO_SDO_AB_GENERAL; // Return a general abort code for invalid parameters
        }

    // SDO flag setup
    CO_SDO_return_t SDO_ret;

    SDO_ret = CO_SDOclient_setup(SDO_C, CO_CAN_ID_SDO_CLI + node_id, CO_CAN_ID_SDO_SRV + node_id, node_id);
    if (SDO_ret != CO_SDO_RT_ok_communicationEnd)
    {
        return CO_SDO_AB_GENERAL; // Return a general abort code if setup fails
    }

    // initiate download
    SDO_ret = CO_SDOclientDownloadInitiate(SDO_C, index, sub_index,
                                           data_size, 1000, false);
    if (SDO_ret != CO_SDO_RT_ok_communicationEnd)
    {
        return CO_SDO_AB_GENERAL;
    }

    if (CO_SDOclientDownloadBufWrite(SDO_C, data, data_size) != data_size)
    {
        CO_SDOclientClose(SDO_C);
        return CO_SDO_AB_GENERAL;
    }
    // download data
    do
    {
        uint32_t timeDifference_us = 10000;
        CO_SDO_abortCode_t abortCode = CO_SDO_AB_NONE;

        SDO_ret = CO_SDOclientDownload(SDO_C, timeDifference_us, false, bufferPartial, &abortCode, NULL, NULL);
        if (SDO_ret < 0)
        {
            return abortCode;
        }

        sleep_us(timeDifference_us); // This is blocking, but it's fine. No heartbeats are on ATP.
    } while (SDO_ret > 0);

    return CO_SDO_AB_NONE;
}
