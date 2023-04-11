#include "dtb.h"
#include "uart.h"
#include "irq.h"
#include "list.h"

extern char *dtb_base;


void main(char *arg)
{
    register unsigned long long x21 asm("x21");
    // pass by x21 reg
    dtb_base = x21;
    // init list
    INIT_LIST_HEAD(&timer_event_list);
    INIT_LIST_HEAD(&task_list);

    uart_init();
    cpacr_el1_off();
    enable_interrupt();

    // turn on interrupt
    core_timer_enable();
    enable_mini_uart_interrupt();

    // turn off to enable printf
    // 控制在EL1和EL0時 access advanced SIMD 或 floating point時會不會產生exception


    // get devicetree
    fdt_traverse(initramfs_callback);

    uart_puts("Lab3 :\n");

    shell();
}