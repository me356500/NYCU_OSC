
int relocated = 0;

char *dtb_base;

void main(char *arg)
{
    char *dtb_base;
    dtb_base = arg;
    if (!relocated)
    {
        relocated = 1;
        relocate(arg);
    }

    uart_init();
    shell();
}