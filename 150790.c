static inline void cirrus_cursor_compute_yrange(CirrusVGAState *s)
{
    const uint8_t *src;
    uint32_t content;
    int y, y_min, y_max;

    src = s->vram_ptr + s->real_vram_size - 16 * 1024;
    if (s->sr[0x12] & CIRRUS_CURSOR_LARGE) {
        src += (s->sr[0x13] & 0x3c) * 256;
        y_min = 64;
        y_max = -1;
        for(y = 0; y < 64; y++) {
            content = ((uint32_t *)src)[0] |
                ((uint32_t *)src)[1] |
                ((uint32_t *)src)[2] |
                ((uint32_t *)src)[3];
            if (content) {
                if (y < y_min)
                    y_min = y;
                if (y > y_max)
                    y_max = y;
            }
            src += 16;
        }
    } else {
        src += (s->sr[0x13] & 0x3f) * 256;
        y_min = 32;
        y_max = -1;
        for(y = 0; y < 32; y++) {
            content = ((uint32_t *)src)[0] |
                ((uint32_t *)(src + 128))[0];
            if (content) {
                if (y < y_min)
                    y_min = y;
                if (y > y_max)
                    y_max = y;
            }
            src += 4;
        }
    }
    if (y_min > y_max) {
        s->last_hw_cursor_y_start = 0;
        s->last_hw_cursor_y_end = 0;
    } else {
        s->last_hw_cursor_y_start = y_min;
        s->last_hw_cursor_y_end = y_max + 1;
    }
}