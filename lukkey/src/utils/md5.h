#ifndef __UTILS_MD5_H__
#define __UTILS_MD5_H__

#include <stddef.h>
#include <stdint.h>

#define MD5_DIGEST_LENGTH 16

/* MD5 as 32-char hex; out_hex must hold at least 33 bytes (incl. NUL). */
void md5_compute_hex(const uint8_t *data, size_t len, char *out_hex);

/* Raw 16-byte MD5 digest */
void md5_compute(const uint8_t *data, size_t len, uint8_t digest[MD5_DIGEST_LENGTH]);

#endif // __UTILS_MD5_H__

