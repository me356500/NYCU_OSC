
#include "gpio.h"
#include "sprintf.h"

#define MAX_BUF_SIZE 0x400
void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned int d);
int uart_printf(char *fmt, ...);