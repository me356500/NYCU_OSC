
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