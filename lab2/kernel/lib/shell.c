
#include "malloc.h"
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
        uart_puts("cat    \t: cat\n");
        uart_puts("clear  \t: clear all\n");
        uart_puts("help   \t: print this help menu\n");
        uart_puts("hello  \t: print Hello World!\n");
        uart_puts("ls     \t: ls\n");
        uart_puts("mailbox\t: show infos of board revision and ARM memory\n");
        uart_puts("malloc \t: allocate string abc\n");
        uart_puts("reboot \t: reboot the device\n");
    }
    else if(!strcmp(arg[0], "hello") && i == 1) {
        uart_puts("Hello World!\n");
    }
    else if(!strcmp(arg[0], "mailbox") && i == 1) {
        mbox_info();
    }
    else if(!strcmp(arg[0], "reboot") && i == 1) {
        reset(1);
    }
    else if(!strcmp(arg[0], "cat") && i == 2) {
        
        cat(arg[1]);
        //uart_puts(arg[1]);
    }
    else if(!strcmp(arg[0], "clear") && i == 1) {
        uart_puts("\x1b[2J\x1b[H");
    }
    else if(!strcmp(arg[0], "ls") && i == 1) {
        ls(".");
        //uart_puts(arg[1]);
    }
    else if(!strcmp(arg[0], "malloc")) {
        uart_puts("Allocating string abc\n");
        char *st = (char*)simple_malloc(8);
        // uart_puts("test1\n");
        st[0] = 'a';
        st[1] = 'b';
        st[2] = 'c';
        st[3] = '\0';
        uart_send(st[0]);
        uart_send(st[1]);
        uart_send(st[2]);

        uart_puts("\n");
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
        char str[30] = {};
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