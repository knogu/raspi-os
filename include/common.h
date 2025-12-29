#pragma once

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef volatile u32 reg32;

void *memcpy(void *dest, const void *src, uint16_t len);

typedef struct {
    uint8_t DestAddrs[6];
    uint8_t SrcAddrs[6];
    uint16_t type;
} EtherNetII;