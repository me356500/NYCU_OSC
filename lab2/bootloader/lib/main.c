#include "uart.h"
#include "mbox.h"
#include "shell.h"
#include "string.h"

int relocated = 0;

void main(char *arg)
{
    if (!relocated)
    {
        relocated = 1;
        relocate(arg);
    }

    uart_init();
    shell();
}