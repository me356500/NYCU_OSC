#include "string.h"
#include "cpio.h"

void cmd(char *s1) {
    
    char *token;
    char arg[20][50];
    int i = 0;
    token = strtok(s1, " ");

    while(token != 0) {
        strcpy(arg[i], token);
        //uart_puts(arg[i]);
        ++i;
        //uart_send('0'+i);
        token = strtok(0, " ");
    }
    
    if(!strcmp(arg[0], "help") && i == 1) {
        uart_puts("help   \t: print this help menu\n");
        uart_puts("hello  \t: print Hello World!\n");
        uart_puts("mailbox\t: show infos of board revision and ARM memory\n");
        uart_puts("reboot \t: reboot the device\n");
    }
    else if(!strcmp(arg[0], "hello") && i == 1) {
        uart_puts("Hello World!\n");
    }
    else if(!strcmp(arg[0], "mailbox") && i == 1) {
        mbox_info();
    }
    else if(!strcmp(arg[0], "reboot") && i == 1) {
        reset(0);
    }
    else if(!strcmp(arg[0], "cat") && i == 2) {
        cat(arg[1]);
        //uart_puts(arg[1]);
    }
    else if(!strcmp(arg[0], "ls") && i == 2) {
        ls(arg[1]);
        //uart_puts(arg[1]);
    }
    else {
        uart_puts("Unknown command: ");
        uart_puts(s1);
        uart_puts("\n");
    }
}

void shell() {
    while(1) {
        uart_puts("# ");

        int i = 0;
        char str[128] = {};
        char c = ' ';
        
        while( c != '\n') {
            c = uart_getc();

             if(c == '\n') {
                uart_puts("\n");
            }
            else {
                uart_send(c);
            }
            if(c == 0x08 || c == 0x7f && i > 0) {
                uart_send('\b');
                uart_send(' ');
                uart_send('\b');
                str[strlen(str) - 1] = '\0';
                --i;
            }
            
            if(c != '\n' && c != 0x08 && c != 0x7f)
                str[i++] = c;

        }
        cmd(str);

    }
}