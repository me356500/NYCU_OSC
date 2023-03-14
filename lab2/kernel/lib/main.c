

void main(char *arg)
{
   

    uart_init();
    /*
    char* str = simple_malloc(8);
    str[0] = 'a';
    str[1] = 'b';
    str[2] = 'c';
    */
    uart_puts(str);
    shell();
}