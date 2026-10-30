#ifndef SEVCON_PDO_H
#define SEVCON_PDO_H

#include "canopen_pdo.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Five receive slots on the HCB, corresponding to the inverter's TPDO1-5.
 * Slot numbers below are zero-based C array indices. No RPDOs are defined here.
 */
#define SEVCON_TPDO_COUNT 5U
#define SEVCON_TPDO1_EXPECTED_PERIOD_MS 20U

typedef enum {
    SEVCON_TPDO1 = 0,
    SEVCON_TPDO2,
    SEVCON_TPDO3,
    SEVCON_TPDO4,
    SEVCON_TPDO5
} SevconTpdoSlot;

typedef struct {
    bool enabled;
    PdoMap map;
} SevconTpdoConfig;

typedef struct {
    SevconTpdoConfig tpdo[SEVCON_TPDO_COUNT];
} SevconPdoLayouts;

/* Each TPDO has its own cache and timestamp. Reception of one slot does not
 * refresh another. Values remain raw; scaling belongs to named accessors.
 */
typedef struct {
    PdoValue values[8];
    size_t value_count;
    bool seen;
    uint32_t updated_ms;
} SevconTpdoSample;

typedef struct {
    SevconTpdoSample tpdo[SEVCON_TPDO_COUNT];
} SevconPdoFeedback;

/* TPDO1: only Target Id's signedness and scale have been supplied so far.
 * target_id_value is in the dictionary's engineering unit (not assumed here).
 * Other fields preserve 16-bit patterns until their metadata is checked.
 */
typedef struct {
    int16_t target_id_raw;
    float target_id_value;
    uint16_t target_iq_bits;
    uint16_t id_bits;
    uint16_t iq_bits;
    uint32_t updated_ms;
} SevconCurrentFeedback;

/* User-reported TPDO1 is enabled at 0x410; TPDO2-5 start disabled/empty.
 * This builds local decoding configuration, not controller settings. For new
 * slots, supply a static field array, actual ID and confirmed map before enabling.
 */
PdoResult SevconPdo_DefaultLayouts(SevconPdoLayouts *out);
PdoResult SevconPdo_ValidateLayouts(const SevconPdoLayouts *layouts);
void SevconPdo_ClearFeedback(SevconPdoFeedback *feedback);

/* Full-ID dispatch and exact-DLC decoding. Only PDO_OK changes feedback and
 * updated_slot. No TX, controller remapping, SYNC or readiness decisions.
 * Keep maps stable; clear feedback after replacing a live configuration.
 */
PdoResult SevconPdo_Receive(const SevconPdoLayouts *layouts,
                           const CanFrame *frame,
                           uint32_t now_ms,
                           SevconPdoFeedback *feedback,
                           SevconTpdoSlot *updated_slot);

/* NULL/false means the requested sample/value is unavailable. Named-current
 * conversion preserves *out on failure. Check freshness separately before use.
 * These APIs require a cache initialized by ClearFeedback and owned by one
 * receiver, or protected by application synchronization if shared concurrently.
 */
const PdoValue *SevconPdo_GetValue(const SevconPdoFeedback *feedback,
                                 SevconTpdoSlot slot,
                                 uint16_t index,
                                 uint8_t subindex);
bool SevconPdo_GetCurrents(const SevconPdoFeedback *feedback,
                          SevconCurrentFeedback *out);
bool SevconPdo_IsFresh(const SevconPdoFeedback *feedback,
                      SevconTpdoSlot slot,
                      uint32_t now_ms,
                      uint32_t max_age_ms);

#ifdef __cplusplus
}
#endif
#endif
