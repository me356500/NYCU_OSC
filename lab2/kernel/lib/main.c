#include "dtb.h"
#include "uart.h"
extern char *dtb_base;
void main(char *arg)
{
    dtb_base = arg;
    uart_init();
    uart_puts("test1");
    fdt_traverse(initramfs_callback);
    uart_puts("test2");
   // uart_printf("dtb: %x: \r\n", dtb_base);
    uart_puts("test3");
    /*
    char* str = simple_malloc(8);
    str[0] = 'a';
    str[1] = 'b';
    str[2] = 'c';
    */
    //uart_puts(str);

    shell();
}