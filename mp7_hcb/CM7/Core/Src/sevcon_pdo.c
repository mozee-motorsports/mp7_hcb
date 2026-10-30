#include "sevcon_pdo.h"
#include <limits.h>
#include <string.h>

/* Match DVT's wire order. The three U16 entries preserve bits only; their
 * signedness/units/scaling have not been established. Target Id is INTEGER16,
 * scale 0.0625, read-only according to the supplied dictionary row.
 */
static const PdoField tpdo1_fields[] = {
    {0x4600, 5, PDO_I16},  /* Bytes 0-1: Target Id */
    {0x4600, 6, PDO_U16},  /* Bytes 2-3: Target Iq, uninterpreted bits */
    {0x4600, 7, PDO_U16},  /* Bytes 4-5: Id, uninterpreted bits */
    {0x4600, 8, PDO_U16}   /* Bytes 6-7: Iq, uninterpreted bits */
};

/* Add static PdoField arrays for TPDO2, TPDO3, TPDO4 and TPDO5 here as their
 * actual mappings become available. Unused slots deliberately have no fields.
 */
PdoResult SevconPdo_DefaultLayouts(SevconPdoLayouts *out)
{
    if (out == NULL) {
        return PDO_INVALID;
    }

    SevconPdoLayouts defaults = {
        .tpdo = {
            [SEVCON_TPDO1] = {
                .enabled = true,
                .map = {
                    .cob_id = 0x410,
                    .fields = tpdo1_fields,
                    .field_count = 4,
                    /* Confirms the user-supplied decoding contract only. */
                    .confirmed = true
                }
            },
            [SEVCON_TPDO2] = {.enabled = false, .map = {0}},
            [SEVCON_TPDO3] = {.enabled = false, .map = {0}},
            [SEVCON_TPDO4] = {.enabled = false, .map = {0}},
            [SEVCON_TPDO5] = {.enabled = false, .map = {0}}
        }
    };

    *out = defaults;
    return PDO_OK;
}

PdoResult SevconPdo_ValidateLayouts(const SevconPdoLayouts *layouts)
{
    if (layouts == NULL) {
        return PDO_INVALID;
    }

    for (size_t i = 0; i < SEVCON_TPDO_COUNT; ++i) {
        const SevconTpdoConfig *slot = &layouts->tpdo[i];
        if (!slot->enabled) {
            continue;
        }

        uint8_t length;
        if (Pdo_Validate(&slot->map, &length) != PDO_OK) {
            return PDO_INVALID;
        }
        for (size_t j = 0; j < i; ++j) {
            if (layouts->tpdo[j].enabled &&
                layouts->tpdo[j].map.cob_id == slot->map.cob_id) {
                return PDO_INVALID;
            }
        }

        /* The named current accessor requires this exact TPDO1 object/type
         * set, but allows a different wire order. Other slots are generic maps.
         */
        if (i == SEVCON_TPDO1) {
            if (slot->map.field_count != 4U) {
                return PDO_SCHEMA_MISMATCH;
            }
            for (size_t f = 0; f < 4U; ++f) {
                bool found = false;
                for (size_t m = 0; m < 4U; ++m) {
                    const PdoField *field = &slot->map.fields[m];
                    if (field->index == tpdo1_fields[f].index &&
                        field->subindex == tpdo1_fields[f].subindex &&
                        field->type == tpdo1_fields[f].type) {
                        found = true;
                    }
                }
                if (!found) {
                    return PDO_SCHEMA_MISMATCH;
                }
            }
        }
    }
    return PDO_OK;
}

void SevconPdo_ClearFeedback(SevconPdoFeedback *feedback)
{
    if (feedback != NULL) {
        memset(feedback, 0, sizeof *feedback);
    }
}

PdoResult SevconPdo_Receive(const SevconPdoLayouts *layouts,
                           const CanFrame *frame,
                           uint32_t now_ms,
                           SevconPdoFeedback *feedback,
                           SevconTpdoSlot *updated_slot)
{
    if (frame == NULL || feedback == NULL || updated_slot == NULL ||
        frame->id > 0x7FFU || frame->length > 8U) {
        return PDO_INVALID;
    }
    PdoResult result = SevconPdo_ValidateLayouts(layouts);
    if (result != PDO_OK) {
        return result;
    }

    for (size_t i = 0; i < SEVCON_TPDO_COUNT; ++i) {
        const SevconTpdoConfig *slot = &layouts->tpdo[i];
        if (!slot->enabled || slot->map.cob_id != frame->id) {
            continue;
        }

        SevconTpdoSample next = {0};
        result = Pdo_Decode(&slot->map, frame, next.values, 8,
                            &next.value_count);
        if (result != PDO_OK) {
            return result;
        }
        next.seen = true;
        next.updated_ms = now_ms;
        feedback->tpdo[i] = next;
        *updated_slot = (SevconTpdoSlot)i;
        return PDO_OK;
    }
    return PDO_NOT_MINE;
}

const PdoValue *SevconPdo_GetValue(const SevconPdoFeedback *feedback,
                                 SevconTpdoSlot slot,
                                 uint16_t index,
                                 uint8_t subindex)
{
    if (feedback == NULL || (unsigned)slot >= SEVCON_TPDO_COUNT ||
        !feedback->tpdo[slot].seen) {
        return NULL;
    }
    const SevconTpdoSample *sample = &feedback->tpdo[slot];
    return Pdo_Find(sample->values, sample->value_count, index, subindex);
}

bool SevconPdo_GetCurrents(const SevconPdoFeedback *feedback,
                          SevconCurrentFeedback *out)
{
    if (out == NULL) {
        return false;
    }
    const PdoValue *target_id = SevconPdo_GetValue(feedback, SEVCON_TPDO1, 0x4600, 5);
    const PdoValue *target_iq = SevconPdo_GetValue(feedback, SEVCON_TPDO1, 0x4600, 6);
    const PdoValue *actual_id = SevconPdo_GetValue(feedback, SEVCON_TPDO1, 0x4600, 7);
    const PdoValue *actual_iq = SevconPdo_GetValue(feedback, SEVCON_TPDO1, 0x4600, 8);

    if (target_id == NULL || target_iq == NULL || actual_id == NULL || actual_iq == NULL ||
        target_id->raw < INT16_MIN || target_id->raw > INT16_MAX ||
        target_iq->raw < 0 || target_iq->raw > UINT16_MAX ||
        actual_id->raw < 0 || actual_id->raw > UINT16_MAX ||
        actual_iq->raw < 0 || actual_iq->raw > UINT16_MAX) {
        return false;
    }

    SevconCurrentFeedback next = {
        .target_id_raw = (int16_t)target_id->raw,
        .target_id_value = (float)target_id->raw * 0.0625f,
        .target_iq_bits = (uint16_t)target_iq->raw,
        .id_bits = (uint16_t)actual_id->raw,
        .iq_bits = (uint16_t)actual_iq->raw,
        .updated_ms = feedback->tpdo[SEVCON_TPDO1].updated_ms
    };
    *out = next;
    return true;
}

bool SevconPdo_IsFresh(const SevconPdoFeedback *feedback,
                      SevconTpdoSlot slot,
                      uint32_t now_ms,
                      uint32_t max_age_ms)
{
    if (feedback == NULL || (unsigned)slot >= SEVCON_TPDO_COUNT ||
        !feedback->tpdo[slot].seen) {
        return false;
    }
    /* Unsigned subtraction handles normal millisecond-counter wraparound.
     * Caller supplies the freshness limit; no stop policy is chosen here.
     */
    return (uint32_t)(now_ms - feedback->tpdo[slot].updated_ms) <= max_age_ms;
}
