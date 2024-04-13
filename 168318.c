g_Ctoc(const Char *str, char *buf, unsigned int len)
{
    while (len--) {
        if ((*buf++ = *str++) == EOS) {
            return 0;
        }
    }
    return 1;
}