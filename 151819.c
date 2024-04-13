static bool red_stream_write_u8(RedStream *s, uint8_t n)
{
    return red_stream_write_all(s, &n, sizeof(uint8_t));
}