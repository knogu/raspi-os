#include "../include/printf.h"
#include "../include/peripherals/timer.h"
#include "../include/peripherals/irq.h"
#include "../include/peripherals/aux.h"

const u32 interval_1 = CLOCKHZ;
u32 cur_val_1 = 0;

const u32 interval_3 = CLOCKHZ / 4;
u32 cur_val_3 = 0;

void timer_init() {
    cur_val_1 = REGS_TIMER->counter_lo;
    cur_val_1 += interval_1;
    REGS_TIMER->compare[1] = cur_val_1;

    cur_val_3 = REGS_TIMER->counter_lo;
    cur_val_3 += interval_3;
    REGS_TIMER->compare[3] = cur_val_3;
}

void handle_timer_1() {
    cur_val_1 += interval_1;
    REGS_TIMER->compare[1] = cur_val_1;
    REGS_TIMER->control_status |= SYS_TIMER_IRQ_1;

    printf("Timer 1 received.\n");
}

void handle_timer_3() {
    cur_val_3 += interval_1;
    REGS_TIMER->compare[3] = cur_val_3;
    REGS_TIMER->control_status |= SYS_TIMER_IRQ_3;

    printf("Timer 3 received.\n");
}