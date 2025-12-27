#include "common.h"
#include "mini_uart.h"

void kernel_main() {
    uart_init();
    uart_send_string("raspberry PI Bare Metal OS Initializing .. \n");

    while (1) {
        uart_send(uart_recv());
    }
}