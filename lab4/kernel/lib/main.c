#include "dtb.h"
#include "uart.h"
#include "irq.h"
#include "list.h"

extern char *dtb_base;

void init() {
    INIT_LIST_HEAD(&timer_event_list);
    INIT_LIST_HEAD(&task_list);
    core_timer_enable();
    core_timer_interrupt_enable();
    uart_init();
    enable_mini_uart_interrupt();
    enable_interrupt();
}

void main(char *arg)
{
    register unsigned long long x21 asm("x21");
    // pass by x21 reg
    dtb_base = x21;
    // init list
    

  
    init();

    // get devicetree
    fdt_traverse(initramfs_callback);
    memory_init();
    uart_puts("Lab4 :\n");

    shell();
}