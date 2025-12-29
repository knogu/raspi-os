#include "enc28j60.h"
#include "../include/common.h"
#include "../include/printf.h"

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
    u8 checksum;
    u16 icmpIdentifier;
    u16 sequence;
} IpIcmp;

typedef struct {
    u8 type;
    u8 code;
    u8 checksum;
    u16 identifier;
    u16 sequence;
} Icmp;



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
    ip.packetsLen = 7 << 8; // todo: calculate
    ip.ttl = 64;
    ip.protocol = 1; // icmp
    ip.headerChecksum = 0; // todo
    memcpy(ip.senderIP, senderIP, 4);
    memcpy(ip.destinationIP, targetIP, 4);
    ip.type = 8;
    ip.code = 0;
    ip.checksum = 0; // todo: fill
    ip.identifier = 2; // any value should be fine
    ip.sequence = 1;

    if (ENC_RestoreTXBuffer(&handle, sizeof(IpIcmp)) == 0) {
        printf("Sending ping...\n");

        ENC_WriteBuffer((unsigned char *)&ip, sizeof(IpIcmp));
        handle.transmitLength = sizeof(IpIcmp);

        ENC_Transmit(&handle);
    }
}

extern uint8_t routerIP[4];
extern uint8_t deviceIP[4];

extern uint8_t myMAC[6];
extern uint8_t routerMAC[6];

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