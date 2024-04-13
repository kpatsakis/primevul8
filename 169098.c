static unsigned char *decode_chars(const unsigned char *src,
                                   unsigned char *dst, size_t cnt_in)
{
    uint32_t value = 0;
    for (int i = 0; i < cnt_in; i++)
        value |= (uint32_t) ((src[i] - 33u) & 63) << 6 * (3 - i);

    *dst++ = value >> 16;
    if (cnt_in >= 3)
        *dst++ = value >> 8 & 0xff;
    if (cnt_in >= 4)
        *dst++ = value & 0xff;
    return dst;
}