


int relocated = 0;

char *dtb_base;

void main(char *arg)
{
    uart_init();
    
    dtb_base = arg;
    if (!relocated)
    {
        relocated = 1;
        relocate(arg);
    }
    uart_puts("\n");
    
    shell();
}