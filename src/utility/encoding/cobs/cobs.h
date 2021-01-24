#ifndef TELEMETRYJETCLI_COBS_H
#define TELEMETRYJETCLI_COBS_H

#include <memory>

/*
 * StuffData byte stuffs "length" bytes of data
 * at the location pointed to by "ptr", writing
 * the output to the location pointed to by "dst".
 *
 * Returns the length of the encoded data.
 * From https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
 */
size_t StuffData(const uint8_t *ptr, size_t length, uint8_t *dst);

/*
 * UnStuffData decodes "length" bytes of data at
 * the location pointed to by "ptr", writing the
 * output to the location pointed to by "dst".
 *
 * Returns the length of the decoded data
 * (which is guaranteed to be <= length).
 * From https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
 */
size_t UnStuffData(const uint8_t *ptr, size_t length, uint8_t *dst);

#endif  // TELEMETRYJETCLI_COBS_H
