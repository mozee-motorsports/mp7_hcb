#include "canopen_pdo.h"
#include <string.h>

/* All sizes here are BYTES. A zero size identifies an unsupported enum value. */
static unsigned width(PdoType type)
{
    switch (type) {
    case PDO_U8: case PDO_I8: return 1U;
    case PDO_U16: case PDO_I16: return 2U;
    case PDO_U32: case PDO_I32: return 4U;
    default: return 0U;
    }
}


static bool is_signed(PdoType type)
{ return type == PDO_I8 || type == PDO_I16 || type == PDO_I32; }

PdoResult Pdo_MappingEntry(const PdoField *field, uint32_t *entry)
{
    if (field == NULL || entry == NULL || field->index == 0U || width(field->type) == 0U)
        return PDO_INVALID;
    /* The descriptor uses bits, although the packet codec works in bytes. */
    *entry = ((uint32_t)field->index << 16) | ((uint32_t)field->subindex << 8) |
             (uint32_t)(8U * width(field->type));
    return PDO_OK;
}
PdoResult Pdo_Validate(const PdoMap *map, uint8_t *payload_length)
{
    /* This codec accepts standard nonzero IDs and at most eight integer fields.
     * Each supported field occupies at least one byte, so eight is also the
     * maximum possible field count in an eight-byte Classical CAN PDO.
     */
    if (map == NULL || payload_length == NULL || map->fields == NULL ||
        map->cob_id == 0U || map->cob_id > 0x7FFU ||
        map->field_count == 0U || map->field_count > 8U) return PDO_INVALID;
    unsigned length = 0U;
    for (size_t i = 0; i < map->field_count; ++i) {
        unsigned bytes = width(map->fields[i].type);
        if (bytes == 0U || map->fields[i].index == 0U) return PDO_INVALID;
        length += bytes;
        if (length > 8U) return PDO_INVALID;
        /* Each object key must identify exactly one field in this codec.
         * Equal indices with DIFFERENT subindices are separate valid keys.
         */
        for (size_t j = 0; j < i; ++j)
            if (map->fields[i].index == map->fields[j].index &&
                map->fields[i].subindex == map->fields[j].subindex) return PDO_INVALID;
    }
    /* Commit only after every check passes; errors preserve the caller's output. */
    *payload_length = (uint8_t)length; return PDO_OK;
}
const PdoValue *Pdo_Find(const PdoValue *values, size_t count,
                        uint16_t index, uint8_t subindex)
{
    if (values == NULL || count > 8U) return NULL;
    for (size_t i = 0; i < count; ++i)
        if (values[i].index == index && values[i].subindex == subindex) return &values[i];
    return NULL;
}
PdoResult Pdo_Encode(const PdoMap *map, const PdoValue *values,
                     size_t value_count, CanFrame *out)
{
    uint8_t length;
    PdoResult result = Pdo_Validate(map, &length);
    if (result != PDO_OK || values == NULL || out == NULL) return PDO_INVALID;
    if (!map->confirmed) return PDO_UNCONFIRMED;
    if (value_count != map->field_count) return PDO_VALUES_MISMATCH;
    /* Equal counts alone would miss duplicates: [A, A] cannot satisfy [A, B]. */
    for (size_t i = 0; i < value_count; ++i)
        for (size_t j = 0; j < i; ++j)
            if (values[i].index == values[j].index && values[i].subindex == values[j].subindex)
                return PDO_VALUES_MISMATCH;
    /* Build a temporary frame so a later failure cannot expose a partial packet. */
    CanFrame encoded = {0}; encoded.id = map->cob_id; encoded.length = length;
    unsigned offset = 0U;
    for (size_t i = 0; i < map->field_count; ++i) {
        const PdoField *field = &map->fields[i];
        /* Follow map order, but fetch values by object key. Callers can keep
         * named values in any order without changing how bytes are arranged.
         */
        const PdoValue *value = Pdo_Find(values, value_count, field->index, field->subindex);
        if (value == NULL) return PDO_VALUES_MISMATCH;
        unsigned bytes = width(field->type), bits = bytes * 8U;
        /* Use 64-bit arithmetic to calculate even a U32 maximum safely.
         * Reject overflow instead of silently truncating the requested value.
         */
        int64_t min = is_signed(field->type) ? -(INT64_C(1) << (bits - 1U)) : 0;
        int64_t max = is_signed(field->type) ? (INT64_C(1) << (bits - 1U)) - 1 :
                                             (INT64_C(1) << bits) - 1;
        if (value->raw < min || value->raw > max) return PDO_VALUE_RANGE;
        /* Converting a negative value to unsigned is defined modulo 2^64.
         * Taking its low 1/2/4 bytes therefore preserves two's-complement bits.
         * Send the least significant byte first: 0x1234 becomes 34 12.
         */
        uint64_t raw = (uint64_t)value->raw;
        for (unsigned b = 0; b < bytes; ++b)
            encoded.data[offset++] = (uint8_t)(raw >> (8U * b));
    }
    *out = encoded; return PDO_OK;
}
PdoResult Pdo_Decode(const PdoMap *map, const CanFrame *frame,
                     PdoValue *out, size_t capacity, size_t *value_count)
{
    uint8_t length;
    if (Pdo_Validate(map, &length) != PDO_OK || frame == NULL || out == NULL ||
        value_count == NULL || frame->id > 0x7FFU || frame->length > 8U) return PDO_INVALID;
    /* An unrelated ID is normal when multiple message handlers share RX. */
    if (frame->id != map->cob_id) return PDO_NOT_MINE;
    if (!map->confirmed) return PDO_UNCONFIRMED;
    /* Exact length prevents accepting a short frame or assuming unknown padding. */
    if (frame->length != length || capacity < map->field_count) return PDO_INVALID;
    /* Decode locally before updating any caller-visible values or count. */
    PdoValue decoded[8] = {{0}};
    unsigned offset = 0U;
    for (size_t i = 0; i < map->field_count; ++i) {
        const PdoField *field = &map->fields[i];
        unsigned bytes = width(field->type), bits = bytes * 8U;
        uint32_t raw = 0U;
        /* Cast before shifting so each byte is assembled as an unsigned value. */
        for (unsigned b = 0; b < bytes; ++b)
            raw |= (uint32_t)frame->data[offset++] << (8U * b);
        decoded[i].index = field->index; decoded[i].subindex = field->subindex;
        decoded[i].raw = (int64_t)raw;
        /* Recover a negative signed value if its top mapped bit is set.
         * Example: I16 bytes 9C FF give 65436; subtract 65536 to obtain -100.
         * Doing this in int64_t avoids out-of-range unsigned-to-signed casts.
         */
        if (is_signed(field->type) && ((uint64_t)raw & (UINT64_C(1) << (bits - 1U))))
            decoded[i].raw -= INT64_C(1) << bits;
    }
    memcpy(out, decoded, map->field_count * sizeof decoded[0]);
    *value_count = map->field_count; return PDO_OK;
}
