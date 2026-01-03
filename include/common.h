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

// MAC address to be assigned to the ENC28J60

extern uint8_t myMAC[6];

// Router MAC is not known to start with, and requires an ARP reply to find out

extern uint8_t routerMAC[6];

// IP address to be assigned to the ENC28J60

extern uint8_t deviceIP[4];

// IP Address of the router, whose hardware address we will find using the ARP request

extern uint8_t routerIP[4];