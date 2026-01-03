#include "../include/common.h"

void *memcpy(void *dest, const void *src, uint16_t len)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    while (len--)
        *d++ = *s++;
    return dest;
}

uint16_t checksum(void* bytes, uint16_t len) {
    uint16_t *pos = (uint16_t*)bytes;
    uint32_t csum = 0;
    for (int i = 0; i < len / 2 + len % 1; i++) {
        csum += *pos++;
    }
    uint16_t result = csum & 0xFFFF | (csum >> 16);
    return ~result;
}