#ifndef HCB_CAN_CONFIG_H
#define HCB_CAN_CONFIG_H

/* Peripheral assignments, not CAN message IDs.
 * Existing vehicle traffic uses FDCAN1. SEVCON wiring is currently unknown:
 * leave 0 until identified, then select 1 or 2 and assign the vehicle bus to
 * the OTHER peripheral. These macros may also be supplied as build defines.
 * No automatic bus detection or SYNC producer is enabled.
 */
#ifndef HCB_VEHICLE_FDCAN_INDEX
#define HCB_VEHICLE_FDCAN_INDEX 1
#endif

#ifndef HCB_SEVCON_FDCAN_INDEX
#define HCB_SEVCON_FDCAN_INDEX 0
#endif

#if HCB_VEHICLE_FDCAN_INDEX != 1 && HCB_VEHICLE_FDCAN_INDEX != 2
#error "Vehicle CAN must select FDCAN1 or FDCAN2"
#endif
#if HCB_SEVCON_FDCAN_INDEX < 0 || HCB_SEVCON_FDCAN_INDEX > 2
#error "SEVCON CAN must select 0 (unassigned), 1 or 2"
#endif
#if HCB_SEVCON_FDCAN_INDEX != 0 && HCB_SEVCON_FDCAN_INDEX == HCB_VEHICLE_FDCAN_INDEX
#error "Vehicle and SEVCON protocols must use separate configured buses"
#endif
#endif
