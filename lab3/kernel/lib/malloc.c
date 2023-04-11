extern char __heap_start;

char *__heap_top = &__heap_start;

// return current heap pointer
// move heap pointer
void *smalloc(unsigned long size)
{
    char *r = __heap_top;
    __heap_top += size;
    return r;
}

void *memcpy(void *dest, const void *src, int n) {
    char *cdest = (char *) dest;
    const char *csrc = (const char *) src;

    for (int i = 0; i < n; i++) {
        cdest[i] = csrc[i];
    }

    return dest;
}