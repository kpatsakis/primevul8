static uint16_t dp8393x_get(dp8393xState *s, int width, int offset)
{
    uint16_t val;

    if (s->big_endian) {
        val = be16_to_cpu(s->data[offset * width + width - 1]);
    } else {
        val = le16_to_cpu(s->data[offset * width]);
    }
    return val;
}