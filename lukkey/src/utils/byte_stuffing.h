#ifndef __UTILS_BYTE_STUFFING_H__
#define __UTILS_BYTE_STUFFING_H__

#include <stddef.h>
#include <stdint.h>

#define BYTE_STUFF_ESCAPE 0x7D

/**
 * Byte-stuff encode: 0x7D->0x7D 0x00, 0x0D->0x7D 0x01, 0x0A->0x7D 0x02.
 * @return Output length, or -1 if out_cap too small or bad args.
 */
int byte_stuff_encode(const uint8_t *in, size_t in_len,
                      uint8_t *out, size_t out_cap);

/**
 * Inverse of byte_stuff_encode.
 * @return Decoded length, or -1 on bad escape or bad args.
 */
int byte_stuff_decode(const uint8_t *in, size_t in_len,
                      uint8_t *out, size_t out_cap);

#endif /* __UTILS_BYTE_STUFFING_H__ */
