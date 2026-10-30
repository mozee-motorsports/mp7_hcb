#ifndef CANOPEN_PDO_H
#define CANOPEN_PDO_H

/* Generic PDO codec: describe a packet once, then pack/unpack its values.
 * A PDO carries values without their object addresses. The configured map is
 * what tells both ends which object each group of bytes represents.
 * This file has no dependency on STM32 hardware or a particular motor driver.
 */
#include "can_frame.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PDO_OK = 0,           /* Requested local operation completed successfully. */
    PDO_INVALID,          /* Bad arguments, unsupported field type, ID or length. */
    PDO_UNCONFIRMED,      /* Map is a draft; encoding/decoding is not permitted. */
    PDO_NOT_MINE,         /* Valid frame belongs to a different configured ID. */
    PDO_VALUE_RANGE,      /* A raw value cannot fit in its mapped integer type. */
    PDO_VALUES_MISMATCH,  /* Input values have missing, extra or duplicate keys. */
    PDO_SCHEMA_MISMATCH   /* A typed wrapper's required objects/types differ. */
} PdoResult;

/* Supported codec subset: byte-aligned integer objects of 8, 16 or 32 bits.
 * No bit fields, floats, implicit padding or extended identifiers.
 */
typedef enum { PDO_U8, PDO_I8, PDO_U16, PDO_I16, PDO_U32, PDO_I32 } PdoType;

typedef struct {
    uint16_t index;       /* Object dictionary address, e.g. 0x6041 = statusword. */
    uint8_t subindex;     /* Entry within that object, commonly 0 for a scalar. */
    PdoType type;         /* Determines byte width, signedness and valid range. */
} PdoField;

/* Fields appear in WIRE order. Keep the array alive for the map's lifetime.
 * confirmed is the caller's declaration of compatibility with the controller.
 * Validation checks local structure only; it does not read or configure a drive.
 */
typedef struct {
    uint16_t cob_id;          /* Full 11-bit CAN ID; excludes COB-ID control flags. */
    const PdoField *fields;   /* First field is packed first, starting at byte 0. */
    size_t field_count;       /* Number of fields, not their total byte count. */
    bool confirmed;          /* Set only after checking the intended configuration. */
} PdoMap;

/* Values are addressed by object, so their array order does not matter.
 * int64_t can represent the entire supported signed/unsigned 32-bit range.
 */
typedef struct {
    uint16_t index;
    uint8_t subindex;
    int64_t raw;             /* Unscaled dictionary value, not automatically Nm/rpm. */
} PdoValue;

/* Check a map and calculate the exact payload length from its field types.
 * A valid local layout may still describe objects the controller does not support.
 * Works on draft maps; writes payload_length only on PDO_OK.
 */
PdoResult Pdo_Validate(const PdoMap *map, uint8_t *payload_length);

/* Build a mapping descriptor for comparison with DCF mapping records:
 * index (16 bits) | subindex (8 bits) | field length in BITS (8 bits).
 * For example, 6040:00 U16 becomes 0x60400010. No controller write occurs.
 */
PdoResult Pdo_MappingEntry(const PdoField *field, uint32_t *entry);

/* Pure codecs: no transmission, remapping, state transitions or watchdogs.
 * Encode requires exactly one value for every mapped object, no extras/duplicates.
 * Decode requires exact DLC and sufficient output capacity (at most 8 values).
 * Both require a confirmed map. All outputs remain unchanged on an error.
 */
PdoResult Pdo_Encode(const PdoMap *map, const PdoValue *values,
                     size_t value_count, CanFrame *out);

/* Decoded values are returned in map/wire order, each with its object key.
 * capacity is the number of PdoValue elements available in out, not bytes.
 * Only the first *value_count elements are valid after PDO_OK.
 */
PdoResult Pdo_Decode(const PdoMap *map, const CanFrame *frame,
                     PdoValue *out, size_t capacity, size_t *value_count);

/* Look up one value by index + subindex. Returns a pointer into the caller's
 * array, or NULL if absent. Check the result before dereferencing it.
 */
const PdoValue *Pdo_Find(const PdoValue *values, size_t count,
                        uint16_t index, uint8_t subindex);

#ifdef __cplusplus
}
#endif
#endif
