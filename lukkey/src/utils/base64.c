#include "base64.h"
#include <stdlib.h>
#include <string.h>

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

uint8_t *base64_decode_utils(const uint8_t *input, size_t input_len, size_t *out_len)
{
    if (!input) return NULL;
    static int8_t dtable[256];
    static bool inited = false;
    if (!inited) {
        for (int i = 0; i < 256; ++i) dtable[i] = -1;
        for (int i = 'A'; i <= 'Z'; ++i) dtable[i] = i - 'A';
        for (int i = 'a'; i <= 'z'; ++i) dtable[i] = i - 'a' + 26;
        for (int i = '0'; i <= '9'; ++i) dtable[i] = i - '0' + 52;
        dtable[(int)'+'] = 62; dtable[(int)'/'] = 63;
        inited = true;
    }

    size_t alloc = (input_len / 4 + 1) * 3;
    uint8_t *out = (uint8_t*)malloc(alloc);
    if (!out) return NULL;

    size_t o = 0; int val = 0; int valb = -8;
    for (size_t i = 0; i < input_len; i++) {
        unsigned char c = input[i];
        if (c == '\r' || c == '\n' || c == ' ' || c == '\t') continue;
        if (c == '=') break;
        int8_t d = dtable[c];
        if (d < 0) { free(out); return NULL; }
        val = (val << 6) | d; valb += 6;
        if (valb >= 0) {
            if (o >= alloc) { alloc *= 2; uint8_t *tmp = (uint8_t*)realloc(out, alloc); if (!tmp) { free(out); return NULL; } out = tmp; }
            out[o++] = (uint8_t)((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    if (out_len) *out_len = o;
    return out;
}

char *base64_encode_utils(const uint8_t *input, size_t input_len, size_t *out_len)
{
    if (!input) return NULL;
    size_t olen = 4 * ((input_len + 2) / 3);
    char *out = (char*)malloc(olen + 1);
    if (!out) return NULL;

    size_t i = 0, j = 0;
    while (i < input_len) {
        uint32_t octet_a = i < input_len ? input[i++] : 0;
        uint32_t octet_b = i < input_len ? input[i++] : 0;
        uint32_t octet_c = i < input_len ? input[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        out[j++] = b64_table[(triple >> 18) & 0x3F];
        out[j++] = b64_table[(triple >> 12) & 0x3F];
        out[j++] = (i - 1 > input_len) ? '=' : b64_table[(triple >> 6) & 0x3F];
        out[j++] = (i > input_len) ? '=' : b64_table[triple & 0x3F];
    }

    /* Fix padding for partial last triplet */
    size_t mod = input_len % 3;
    if (mod) {
        out[olen - 1] = '=';
        if (mod == 1) out[olen - 2] = '=';
    }

    out[olen] = '\0';
    if (out_len) *out_len = olen;
    return out;
}


