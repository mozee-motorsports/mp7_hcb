# SEVCON current telemetry: five TPDO slots

The active interface is one receive-only TPDO, using the configuration supplied by the user for Gen4 Size6 80 V 550 A / firmware 0705.0014. The previous torque-command/status/speed facade has been replaced. No RPDO command path is defined by this facade; no SYNC, NMT or motor-enable transmission was added.

The older `sevcon_gen4.h` declarations and fixed-layout message helpers remain legacy material; they are not the active five-slot PDO profile.

## Configuration to edit

- **Bus assignment:** `CM7/Core/Inc/hcb_can_config.h`.
- **PDO field lists and five slots:** `CM7/Core/Src/sevcon_pdo.c`, in `tpdo1_fields` and `SevconPdo_DefaultLayouts`.
- **Public receive/accessor API:** `CM7/Core/Inc/sevcon_pdo.h`.
- **Raw HAL receive path:** `CM7/Core/Src/sevcon_rx.c`.

| Slot | Current configuration |
|---|---|
| TPDO1 | Enabled, ID `0x410`, eight bytes, four fields below |
| TPDO2 | Disabled, empty; fill from the actual DVT map |
| TPDO3 | Disabled, empty; fill from the actual DVT map |
| TPDO4 | Disabled, empty; fill from the actual DVT map |
| TPDO5 | Disabled, empty; fill from the actual DVT map |

| Bytes | Dictionary object | DVT name | Interpretation |
|---|---|---|---|
| 0-1 | `4600:05` | Target Id | INTEGER16, signed; multiply raw by `0.0625` |
| 2-3 | `4600:06` | Target Iq | 16 raw bits pending dictionary metadata |
| 4-5 | `4600:07` | Id | 16 raw bits pending dictionary metadata |
| 6-7 | `4600:08` | Iq | 16 raw bits pending dictionary metadata |

All multibyte fields are little endian. Target Id is read-only according to the supplied row. Its scaled accessor is named `target_id_value`: the engineering unit has not been supplied, so the code does not label it amperes. The remaining `uint16_t` fields preserve bits; they do not assert that the physical currents are unsigned. Confirm each row separately before adding signed interpretation/scaling.

## Select the bus when its wiring is known

The vehicle defaults to FDCAN1. SEVCON defaults to **0 (unassigned)** because the physical connection is unknown. Until assigned, HCB does not poll a SEVCON bus and the current samples remain unseen. Both existing peripherals are still initialized as before.

For a confirmed SEVCON connection on FDCAN2, set:

```c
#define HCB_VEHICLE_FDCAN_INDEX 1
#define HCB_SEVCON_FDCAN_INDEX 2
```

If the confirmed roles are reversed, select vehicle 2 and SEVCON 1. The vehicle's existing periodic messages follow that explicit vehicle assignment. The configuration rejects assigning both protocols to the same peripheral. Do not guess the bus from a matching numeric CAN ID.

## Receive flow

```text
main loop (also while waiting for R2D)
  -> HCB_ProcessCANRx(now_ms)
     -> selected vehicle bus: existing MP7 receiver and handler
     -> assigned SEVCON bus: SevconRx_Process
        -> raw HAL header/data, preserving the CAN ID and DLC
        -> SevconStm32_RxFrame validates standard Classical CAN data frame
        -> SevconPdo_Receive matches one enabled slot and exact payload length
        -> only that slot's values, seen flag and receive timestamp update
```

SEVCON polling does bounded work (at most eight frames per call). Invalid frames do not refresh samples. Other CANopen service IDs are currently ignored by this receive module; heartbeat, EMCY, startup/drive-state supervision and communication-loss actions are not implemented by it. The current TPDO carries currents, not drive readiness or speed.

## Read named TPDO1 feedback

```c
/* Use in the main-loop context that owns RX, after HCB_Init(). */
const SevconPdoFeedback *feedback = HCB_GetSevconFeedback();
SevconCurrentFeedback currents;

if (SevconPdo_GetCurrents(feedback, &currents)) {
    /* currents.target_id_raw: signed dictionary value
     * currents.target_id_value: raw * 0.0625, dictionary engineering unit
     * currents.target_iq_bits, currents.id_bits, currents.iq_bits: raw patterns
     * currents.updated_ms: local receive time
     */
}
```

`SevconPdo_IsFresh(feedback, slot, now_ms, max_age_ms)` checks whether a seen sample is within a caller-chosen age. It chooses no timeout action. Use the API from one main-loop owner; protect shared cache access if later moving reception or consumers into interrupts/tasks.

## Fill TPDO2-5

For each actual new DVT map:

1. Add a `static const PdoField` array near the top of `sevcon_pdo.c`, in wire order, with verified types.
2. Fill the corresponding disabled slot's `PdoMap`: full CAN ID, field array and field count. Set `confirmed` after comparing it with the intended controller map, then enable the slot. Confirmation is a local decoding guard, not controller programming.
3. Read a generic value using `SevconPdo_GetValue(feedback, SEVCON_TPDO2, index, subindex)` (or the corresponding slot). The result is NULL until that slot has a sample containing that object. Add named accessors as the meanings become known.
4. Read back the actual controller settings and compare captured frames when hardware becomes available. Clear feedback whenever replacing a live layout so values from an older map cannot be mistaken for new ones.

Every enabled slot must fit within eight bytes and have a distinct CAN ID. TPDO1 also checks the exact object/type set required by its named accessor; reordered fields are supported. Slots 2-5 accept any field set supported by the generic codec. Each slot has its own `seen` flag and timestamp; reception of one never refreshes another.

## Synchronous timing

The supplied TPDO1 is cyclic synchronous, one transmission per SYNC. At a 20 ms SYNC period, expected feedback is 50 Hz. This is a documented expectation, not a timer that this receiver starts. Identify the existing producer before enabling an HCB SYNC generator; none was added here. [CiA PDO triggering reference](https://www.can-cia.org/can-knowledge/pdo-protocol).

## Offline checks

From the repository root:

```sh
sh tests/sevcon_pdo/run_tests.sh
```

The tests use an explicitly marked HAL test double outside the firmware include paths. They check five slots and independent ages, all signed-16 Target Id values and scaling, malformed/unrelated/unconfirmed frames, duplicate IDs, generic additional maps, HAL FIFO selection and failures, bounded draining, and bus routing for unassigned/normal/swapped assignments. They assert that the receive path sends no CAN messages. C++ header/C linkage checks are included.

Full firmware and hardware verification still require the actual vendor HAL/CMSIS dependencies, project build configuration and connected inverter. These are not present in this checkout. The portable codec/facade can also be compiled independently for Cortex-M7.
