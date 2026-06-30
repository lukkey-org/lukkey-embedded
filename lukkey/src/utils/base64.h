#ifndef __UTILS_BASE64_H__
#define __UTILS_BASE64_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Decode Base64 (whitespace and '=' allowed); caller frees. *out_len = decoded size. */
uint8_t *base64_decode_utils(const uint8_t *input, size_t input_len, size_t *out_len);

/* Encode binary to Base64 (no newlines); caller frees. */
char *base64_encode_utils(const uint8_t *input, size_t input_len, size_t *out_len);

#endif // __UTILS_BASE64_H__


