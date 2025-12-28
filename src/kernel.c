#include "common.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"

void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }

    uart_send(c);
}

u32 get_el();

void kernel_main() {
    uart_init();
    init_printf(0, putc);
    printf("raspberry PI Bare Metal OS Initializing .. \n");

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();

    printf("\nEL: %d\n", get_el());
    while (1) {
        // uart_send(uart_recv());
    }
}