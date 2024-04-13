g_strchr(const Char *str, int ch)
{
    do {
        if (*str == ch) {
            return (Char *)str;
        }
    } while (*str++);
    return NULL;
}