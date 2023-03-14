#include "uart.h"
#include "string.h"
extern char *dtb_base;
void load() {

    char *kernel = (char *)(0x80000);
    int kn_ptr = 0;
    int idx = 0;
    char sz[50] = {};
    char c;
    while(1) {
        c = uart_getc();
        //uart_send(c);
        if(c == '\n') {
            sz[idx] = '\0';
            break;
        }
            
        sz[idx++] = c;
    }
    int size = atoi(sz);
    while (size--)
        kernel[kn_ptr++] = uart_getc();

    void (*run)(char *) = (void *)kernel;
    run(dtb_base);

}