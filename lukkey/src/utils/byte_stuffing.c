#include "byte_stuffing.h"

int byte_stuff_encode(const uint8_t *in, size_t in_len,
                      uint8_t *out, size_t out_cap)
{
    if (!in || !out)
        return -1;

    size_t j = 0;
    for (size_t i = 0; i < in_len; i++)
    {
        switch (in[i])
        {
        case 0x7D:
            if (j + 2 > out_cap) return -1;
            out[j++] = 0x7D;
            out[j++] = 0x00;
            break;
        case 0x0D:
            if (j + 2 > out_cap) return -1;
            out[j++] = 0x7D;
            out[j++] = 0x01;
            break;
        case 0x0A:
            if (j + 2 > out_cap) return -1;
            out[j++] = 0x7D;
            out[j++] = 0x02;
            break;
        default:
            if (j + 1 > out_cap) return -1;
            out[j++] = in[i];
            break;
        }
    }
    return (int)j;
}

int byte_stuff_decode(const uint8_t *in, size_t in_len,
                      uint8_t *out, size_t out_cap)
{
    if (!in || !out)
        return -1;

    size_t j = 0;
    for (size_t i = 0; i < in_len; i++)
    {
        if (in[i] == BYTE_STUFF_ESCAPE)
        {
            if (++i >= in_len)
                return -1;
            switch (in[i])
            {
            case 0x00: if (j >= out_cap) return -1; out[j++] = 0x7D; break;
            case 0x01: if (j >= out_cap) return -1; out[j++] = 0x0D; break;
            case 0x02: if (j >= out_cap) return -1; out[j++] = 0x0A; break;
            default:   return -1;
            }
        }
        else
        {
            if (j >= out_cap) return -1;
            out[j++] = in[i];
        }
    }
    return (int)j;
}
