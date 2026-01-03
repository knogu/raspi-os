#include "../include/common.h"
#include "../include/printf.h"

void *memcpy(void *dest, const void *src, uint16_t len)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    while (len--)
        *d++ = *s++;
    return dest;
}

uint16_t checksum(void* bytes, uint16_t len) {
    const uint8_t *p = (const uint8_t *)bytes;
    uint32_t sum = 0;
    while (len >= 2) {
        uint16_t word = ((uint16_t)p[0] << 8) | (uint16_t)p[1];
        printf("adding %X from p: %X", word, p);
        sum += word;
        p += 2;
        len -= 2;
    }
    if (len == 1) {
        sum += (uint16_t)p[0] << 8;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return (uint16_t)~sum;
}