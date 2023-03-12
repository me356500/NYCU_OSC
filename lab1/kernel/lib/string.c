
int isalpha(char c){
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}