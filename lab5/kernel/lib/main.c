#include "dtb.h"
#include "uart.h"
#include "irq.h"
#include "cpio.h"
#include "list.h"
#include "timer.h"
#include "task.h"
#include "sched.h"
#include "mm.h"
#include "mbox.h"


extern char *dtb_base;


void main(char *arg)
{
    //register unsigned long long x21 asm("x21");
    // pass by x21 reg
    dtb_base = arg;
    // init list
    init_idx();
    uart_init();
    INIT_LIST_HEAD(&task_list);
    fdt_traverse(initramfs_callback);
    memory_init();
    timer_list_init();
    init_thread_sched();
   
    enable_interrupt();
    enable_mini_uart_interrupt();
    //core_timer_interrupt_disable_alternative();

    core_timer_enable();


    
  

    uart_puts("Lab5 :\n");

   

    shell();
}