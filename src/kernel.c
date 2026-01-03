#include "common.h"
#include "enc28j60.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "mem.h"
#include "spi.h"
#include "timer.h"

void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }

    uart_send(c);
}

u32 get_el();

void init_network(void);
void arp_test(void);
void ping_test();

struct align_check1 {
    u8 a;
    u8 b;
    u8 c;
    //u8 padding;
    u32 d;
};

struct align_check2 {
    u8 a;
    u8 b;
    u8 c;
    u32 d;
} __attribute__((packed));

u8 buffer[] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70};

extern ENC_HandleTypeDef handle;
void SendArpResponse(uint8_t *targetIP, uint8_t *deviceMAC, uint8_t *destMac);
void SendPong(uint8_t *senderIP, uint8_t *targetIP, uint8_t *deviceMAC, uint8_t *destMac, uint16_t identifier, uint16_t sequence);


void kernel_main() {
    uart_init();
    init_printf(0, putc);
    printf("raspberry PI Bare Metal OS Initializing .. \n");

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();
    timer_init();

    printf("ALIGN CHECK\n");

    struct align_check1 ac1;
    memcpy(&ac1, buffer, 7);
    printf("UNPACKED: A: %X, B: %X, C: %X, D: %X\n", ac1.a, ac1.b, ac1.c, ac1.d);

    struct align_check2 ac2;
    memcpy(&ac2, buffer, 7);
    printf("PACKED: A: %X, B: %X, C: %X, D: %X\n", ac2.a, ac2.b, ac2.c, ac2.d);

    void *p1 = get_free_pages(10);
    void *p2 = get_free_pages(4);
    void *p3 = allocate_memory(20 * 4096 + 1);
    free_memory(p1);
    free_memory(p2);
    free_memory(p3);

    spi_init();
    init_network();
    arp_test();

    while (1) {
        printf("Waiting for incoming packets...\n");
        while (!ENC_GetReceivedFrame(&handle)) {}
        uint8_t *buf = (uint8_t *)handle.RxFrameInfos.buffer;
        uint16_t len = handle.RxFrameInfos.length;
        printf("Incoming frame: Type: %X %X\n", buf[12], buf[13]);
        if (buf[12] == 0x08 && buf[13] == 0x00) {
            printf("IPv4 frame came\n");
            SendPong(deviceIP, routerIP, myMAC, routerMAC, buf[38] << 8 | buf[39], buf[40] << 8 | buf[41]);
        } else if (buf[12] == 0x08 && buf[13] == 0x06) {
            printf("arp frame came\n");
            SendArpResponse(routerIP, myMAC, routerMAC);
        }
    }

    // void *buf_send = get_free_pages(1);

    while (1) {
        // uart_send(uart_recv());
    }
}