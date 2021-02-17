#include "cobs.h"

/*
 * StuffData byte stuffs "length" bytes of data
 * at the location pointed to by "ptr", writing
 * the output to the location pointed to by "dst".
 *
 * Returns the length of the encoded data.
 * From https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
 */
size_t StuffData(const uint8_t *ptr, size_t length, uint8_t *dst) {
    uint8_t *start = dst;
    uint8_t *code_ptr = dst++;

    *code_ptr = 1;
    while (length--) {
        if (*ptr) {
            *dst++ = *ptr++;
            *code_ptr += 1;
        } else {
            code_ptr = dst++;
            *code_ptr = 1;
            ptr++;
        }

        if (*code_ptr == 0xFF && length > 0) {
            code_ptr = dst++;
            *code_ptr = 1;
        }
    }
    *dst++ = 0;

    return dst - start;
}

/*
 * UnStuffData decodes "length" bytes of data at
 * the location pointed to by "ptr", writing the
 * output to the location pointed to by "dst".
 *
 * Returns the length of the decoded data
 * (which is guaranteed to be <= length).
 * From https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
 */
size_t UnStuffData(const uint8_t *ptr, size_t length, uint8_t *dst) {
    const uint8_t *start = dst, *end = ptr + length;
    uint8_t code = 0xFF, copy = 0;
    bool flag = true;

    for (; ptr < end; copy--) {
        if (copy != 0) {
            flag = false;
            *dst++ = *ptr++;
        } else {
            if (code != 0xFF) {
                flag = true;
                *dst++ = 0;
            }
            copy = code = *ptr++;
            if (code == 0) {
                break;
            }
        }
    }

    if (flag) {
        --dst;
    }

    return dst - start;
}
