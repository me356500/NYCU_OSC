

int relocated = 1;

char *dtb_base;

void main(char *arg)
{
    
    dtb_base = arg;
    if (relocated)
    {
        relocated = 0;
        relocate(arg);
    }

    uart_init();
    shell();
}