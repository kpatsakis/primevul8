static void dp8393x_put(dp8393xState *s, int width, int offset,
                        uint16_t val)
{
    if (s->big_endian) {
        if (width == 2) {
            s->data[offset * 2] = 0;
            s->data[offset * 2 + 1] = cpu_to_be16(val);
        } else {
            s->data[offset] = cpu_to_be16(val);
        }
    } else {
        if (width == 2) {
            s->data[offset * 2] = cpu_to_le16(val);
            s->data[offset * 2 + 1] = 0;
        } else {
            s->data[offset] = cpu_to_le16(val);
        }
    }
}