#include "../include/spi.h"
#include "../include/peripherals/spi.h"
#include "../include/gpio.h"
#include "../include/printf.h"

void spi_init() {
    gpio_pin_set_func(7, GFAlt0); //CS1
    gpio_pin_set_func(8, GFOutput); //CS0 MAYBE SHOULD BE ALT 0
    gpio_pin_set_func(9, GFAlt0); //MISO
    gpio_pin_set_func(10, GFAlt0);//MOSI -> DIN (brown)
    gpio_pin_set_func(11, GFAlt0);//SCLK -> CLK (orange)
    gpio_pin_enable(7);
    // gpio_pin_enable(8);
    gpio_pull(8, 0);
    gpio_pin_enable(9);
    gpio_pin_enable(10);
    gpio_pin_enable(11);
}

void spi_send_recv(unsigned char *sbuffer, unsigned char *rbuffer, unsigned int size) {
    REGS_SPI0->data_length = size;
    REGS_SPI0->cs = REGS_SPI0->cs | CS_CLEAR_RX | CS_CLEAR_TX | CS_TA;

    unsigned int read_count = 0;
    unsigned int write_count = 0;

    while(read_count < size || write_count < size) {
        while(write_count < size && REGS_SPI0->cs & CS_TXD) {
            if (sbuffer) {
                REGS_SPI0->fifo = *sbuffer++;
            } else {
                REGS_SPI0->fifo = 0;
            }

            write_count++;
        }

        while(read_count < size && REGS_SPI0->cs & CS_RXD) {
            unsigned int data = REGS_SPI0->fifo;

            if (rbuffer) {
                *rbuffer++ = data;
            }

            read_count++;
        }
    }

    // while(!(REGS_SPI0->cs & CS_DONE)) {
    //     while(REGS_SPI0->cs & CS_RXD) {
    //         unsigned int r = REGS_SPI0->fifo;
    //         debughex(r);
    //     }
    // }

    REGS_SPI0->cs = (REGS_SPI0->cs & ~CS_TA);
}

void spi_send(unsigned char *data, unsigned int size) {
    spi_send_recv(data, 0, size);
}

void spi_recv(unsigned char *data, unsigned int size) {
    spi_send_recv(0, data, size);
}

void spi_chip_select(unsigned char chip_select) {
    gpio_setPinOutputBool(8, chip_select);
}