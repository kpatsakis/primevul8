static void padstr8(uint8_t *buf, int buf_size, const char *src)
{
    int i;
    for(i = 0; i < buf_size; i++) {
        if (*src)
            buf[i] = *src++;
        else
            buf[i] = ' ';
    }
}