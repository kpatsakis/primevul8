static int valid_utf8(const unsigned char* s)
{
    int expected = 0;
    int length;
    if (*s < 0x80)
        /* single-byte code */
        return 1;
    if (*s < 0xc0)
        /* following byte */
        return 0;
    if (*s < 0xE0)
        expected = 1;
    else if (*s < 0xF0)
        expected = 2;
    else if (*s < 0xF8)
        expected = 3;
    else
        return 0;
    length = expected + 1;
    for (; expected; expected--)
        if (s[expected] < 0x80 || s[expected] >= 0xC0)
            return 0;
    return length;
}