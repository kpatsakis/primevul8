static bool red_stream_write_u32_le(RedStream *s, uint32_t n)
{
    n = GUINT32_TO_LE(n);
    return red_stream_write_all(s, &n, sizeof(uint32_t));
}