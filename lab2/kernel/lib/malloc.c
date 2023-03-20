extern char __heap_start;

char *__heap_top = &__heap_start;

// return current heap pointer
// move heap pointer
void *simple_malloc(unsigned long size)
{
    char *r = __heap_top;
    __heap_top += size;
    return r;
}