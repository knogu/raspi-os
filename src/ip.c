#include "enc28j60.h"
#include "../include/common.h"
#include "../include/printf.h"
#include "../include/mem.h"

struct Ether {
    EtherNetII eth;
    u8 payload[];
} __attribute__((packed));

typedef struct Ether Ether;

typedef struct {
    EtherNetII eth;
    u8 verAndHeaderLen;
    u8 tos;
    u16 packetsLen;
    u16 identifier;
    u16 flagAndOffset;
    u8 ttl;
    u8 protocol;
    u16 headerChecksum;
    u8 senderIP[4];
    u8 destinationIP[4];
    u8 type; // icmp. todo: make generic
    u8 code;
    u16 checksum;
    u16 icmpIdentifier;
    u16 sequence;
} IpIcmp;

struct Ip {
    u8 verAndHeaderLen;
    u8 tos;
    u16 packetsLen;
    u16 identifier;
    u16 flagAndOffset;
    u8 ttl;
    u8 protocol;
    u16 headerChecksum;
    u8 senderIP[4];
    u8 destinationIP[4];
    u8 payload[];
} __attribute__((packed));

typedef struct Ip Ip;

struct Icmp {
    u8 type;
    u8 code;
    u8 checksum;
    u16 identifier;
    u16 sequence;
    u8 payload[48]; // fixed for mac os. todo: copy from req
} __attribute__((packed));

typedef struct Icmp Icmp;

extern ENC_HandleTypeDef handle;

void SendPing(uint8_t *senderIP, uint8_t *targetIP, uint8_t *deviceMAC, uint8_t *destMac) {
    // Icmp icmp;
    // icmp.type = 8;
    // icmp.code = 0;
    // icmp.checksum = 0; // todo: fill
    // icmp.identifier = 2; // any value should be fine
    // icmp.sequence = 1;

    IpIcmp ip;
    memcpy(ip.eth.SrcAddrs, deviceMAC, 6);
    memcpy(ip.eth.DestAddrs, destMac, 6);
    ip.eth.type = 0x0008;

    ip.verAndHeaderLen = 4 << 4 | 5; // 4 is from ipv4 and 5 is from that the header len 160 bits divided by 32
    ip.tos = 0;
    ip.packetsLen = (7*4) << 8; // todo: calculate
    ip.ttl = 64;
    ip.protocol = 1; // icmp
    ip.headerChecksum = 0; // todo
    memcpy(ip.senderIP, senderIP, 4);
    memcpy(ip.destinationIP, targetIP, 4);
    ip.type = 8;
    ip.code = 0;
    ip.checksum = 0xfcf7; // todo: calculate
    ip.icmpIdentifier = 2 << 8; // any value should be fine
    ip.sequence = 1 << 8;

    if (ENC_RestoreTXBuffer(&handle, sizeof(IpIcmp)) == 0) {
        printf("Sending ping...\n");

        ENC_WriteBuffer((unsigned char *)&ip, sizeof(IpIcmp));
        handle.transmitLength = sizeof(IpIcmp);

        ENC_Transmit(&handle);
    }
}

void send_ether(uint8_t *deviceMAC, uint8_t *destMac, void *payload, int payload_len, uint16_t type) {
    Ether *eth = get_free_pages(1);
    memcpy(eth->eth.SrcAddrs, deviceMAC, 6);
    memcpy(eth->eth.DestAddrs, destMac, 6);
    eth->eth.type = type;
    memcpy(eth->payload, payload, payload_len);

    if (ENC_RestoreTXBuffer(&handle, sizeof(IpIcmp)) == 0) {
        printf("Sending ether...\n");

        ENC_WriteBuffer((unsigned char *)&eth, sizeof(Ether) + payload_len);
        handle.transmitLength = sizeof(Ether) + payload_len;

        ENC_Transmit(&handle);
    }
}

void send_ip(uint8_t *senderIP, uint8_t *targetIP, void* payload_ptr, int payload_size) {
    Ip *ip = get_free_pages(1);
    ip->verAndHeaderLen = 4 << 4 | 5; // 4 is from ipv4 and 5 is from that the header len 160 bits divided by 32
    ip->tos = 0;
    ip->packetsLen = 0x54 << 8; // todo: calculate
    ip->ttl = 64;
    ip->protocol = 1; // icmp
    ip->headerChecksum = 0; // todo
    memcpy(ip->senderIP, senderIP, 4);
    memcpy(ip->destinationIP, targetIP, 4);
    memcpy(ip->payload, payload_ptr, payload_size);
    send_ether(myMAC, routerMAC, ip, sizeof(Ip) + payload_size, 0x0008);
}

void pong(uint16_t identifier, uint16_t sequence) {
    Icmp *icmp = get_free_pages(1);
    icmp->type = 0;
    icmp->code = 0;
    icmp->identifier = identifier;
    icmp->sequence = sequence;
    for (int i = 47; i >= 0; i--) {
        icmp->payload[i] = 37 - (47 - i);
    }
    icmp->checksum = checksum(icmp, sizeof(Icmp));
    send_ip(deviceIP, routerIP, icmp, sizeof(Icmp));
}

void SendPong(uint8_t *senderIP, uint8_t *targetIP, uint8_t *deviceMAC, uint8_t *destMac, uint16_t identifier, uint16_t sequence) {
    // Icmp icmp;
    // icmp.type = 8;
    // icmp.code = 0;
    // icmp.checksum = 0; // todo: fill
    // icmp.identifier = 2; // any value should be fine
    // icmp.sequence = 1;

    IpIcmp ip;
    memcpy(ip.eth.SrcAddrs, deviceMAC, 6);
    memcpy(ip.eth.DestAddrs, destMac, 6);
    ip.eth.type = 0x0008;

    ip.verAndHeaderLen = 4 << 4 | 5; // 4 is from ipv4 and 5 is from that the header len 160 bits divided by 32
    ip.tos = 0;
    ip.packetsLen = (7*4) << 8; // todo: calculate
    ip.ttl = 64;
    ip.protocol = 1; // icmp
    ip.headerChecksum = 0; // todo
    memcpy(ip.senderIP, senderIP, 4);
    memcpy(ip.destinationIP, targetIP, 4);
    ip.type = 0;
    ip.code = 0;
    ip.checksum = 0xfcff; // todo: calculate
    ip.icmpIdentifier = identifier;
    ip.sequence = sequence;

    if (ENC_RestoreTXBuffer(&handle, sizeof(IpIcmp)) == 0) {
        printf("Sending ...\n");

        ENC_WriteBuffer((unsigned char *)&ip, sizeof(IpIcmp));
        handle.transmitLength = sizeof(IpIcmp);

        ENC_Transmit(&handle);
    }
}

void ping_test() {
    SendPing(deviceIP, routerIP, myMAC, routerMAC);
    printf("Waiting for ping response\n");
    while (1) {
        while (!ENC_GetReceivedFrame(&handle));

        uint16_t len    = handle.RxFrameInfos.length;
        uint8_t *buffer = (uint8_t *)handle.RxFrameInfos.buffer;
        IpIcmp* checkPacket     = (IpIcmp *)buffer;

        if (len > 0) {
            printf("received %d bytes\n", len);
            break;
        }
    }
}