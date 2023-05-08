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

void foo(){
    for(int i = 0; i < 10; ++i) {
        uart_puts("tets\n");
        uart_printf("Thread id: %d %d\n", curr_thread->pid, i);
        for(int j = 0; j < 1000000; ++j);
        schedule();
    }
}

void kernel_main() {
    // ...
    // boot setup
    // ...
    for(int i = 0; i < 3; ++i) { // N should > 2
        thread_create(foo);
        uart_printf("Thread id: %d %d\n", curr_thread->pid, i);
    }
    uart_puts("idle\n");
    idle();
}

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
   
    enable_interrupt();
    enable_mini_uart_interrupt();
    //core_timer_interrupt_disable_alternative();
   
    init_thread_sched();
   
    core_timer_enable();
    cpu_timer_enable();

    
  

    uart_async_printf("Lab5 :\n");
    //fork_test();
    //kernel_main();
    //execfile("syscall.img");
    shell();
}