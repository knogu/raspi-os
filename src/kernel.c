#include "common.h"
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
    ping_test();

    printf("\nEL: %d\n", get_el());

    printf("sleeping 2000 ms...\n");
    timer_sleep(2000);

    printf("sleeping 2000 ms...\n");
    timer_sleep(2000);

    printf("sleeping 5000 ms...\n");
    timer_sleep(5000);

    printf("sleeping 5000 ms...\n");
    timer_sleep(5000);

    printf("done\n");

    while (1) {
        // uart_send(uart_recv());
    }
}