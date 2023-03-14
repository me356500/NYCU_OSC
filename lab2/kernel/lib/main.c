
#include "dtb.h"
extern char *dtb_base;
void main(char *arg)
{
    dtb_base = arg;
    uart_init();
    
    fdt_traverse(initramfs_callback);
    uart_printf("dtb: %x: \r\n", dtb_base);
    /*
    char* str = simple_malloc(8);
    str[0] = 'a';
    str[1] = 'b';
    str[2] = 'c';
    */
    //uart_puts(str);

    shell();
}