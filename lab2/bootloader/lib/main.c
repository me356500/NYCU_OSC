

int relocated = 1;

char *dtb_base;

void main(char *arg)
{
    uart_init();
    
    dtb_base = arg;
    if (relocated)
    {
        relocated = 0;
        relocate(arg);
    }

    
    shell();
}