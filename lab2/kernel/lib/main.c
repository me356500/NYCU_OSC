#include "uart.h"
#include "mbox.h"
#include "shell.h"
#include "string.h"

int main() {

    uart_init();
    mbox_info();
    shell();
    
}