

void cmd(char *s1) {
    
    if(!strcmp(s1, "help")) {
        uart_puts("help   \t: print this help menu\n");
        uart_puts("hello  \t: print Hello World!\n");
        uart_puts("mailbox\t: show infos of board revision and ARM memory\n");
        uart_puts("reboot \t: reboot the device\n");
    }
    else if(!strcmp(s1, "hello")) {
        uart_puts("Hello World!\n");
    }
    else if(!strcmp(s1, "mailbox")) {
        mbox_info();
    }
    else if(!strcmp(s1, "reboot")) {
        reset(1);
    }
    else {
        uart_puts("Unknown command: ");
        uart_puts(s1);
        uart_puts("\n");
    }
    return;
}

void shell() {
    while(1) {
        uart_puts("# ");

        int i = 0;
        char str[64] = {};
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