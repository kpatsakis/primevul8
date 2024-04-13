static void cirrus_cursor_draw_line(VGAState *s1, uint8_t *d1, int scr_y)
{
    CirrusVGAState *s = (CirrusVGAState *)s1;
    int w, h, bpp, x1, x2, poffset;
    unsigned int color0, color1;
    const uint8_t *palette, *src;
    uint32_t content;

    if (!(s->sr[0x12] & CIRRUS_CURSOR_SHOW))
        return;
    /* fast test to see if the cursor intersects with the scan line */
    if (s->sr[0x12] & CIRRUS_CURSOR_LARGE) {
        h = 64;
    } else {
        h = 32;
    }
    if (scr_y < s->hw_cursor_y ||
        scr_y >= (s->hw_cursor_y + h))
        return;

    src = s->vram_ptr + s->real_vram_size - 16 * 1024;
    if (s->sr[0x12] & CIRRUS_CURSOR_LARGE) {
        src += (s->sr[0x13] & 0x3c) * 256;
        src += (scr_y - s->hw_cursor_y) * 16;
        poffset = 8;
        content = ((uint32_t *)src)[0] |
            ((uint32_t *)src)[1] |
            ((uint32_t *)src)[2] |
            ((uint32_t *)src)[3];
    } else {
        src += (s->sr[0x13] & 0x3f) * 256;
        src += (scr_y - s->hw_cursor_y) * 4;
        poffset = 128;
        content = ((uint32_t *)src)[0] |
            ((uint32_t *)(src + 128))[0];
    }
    /* if nothing to draw, no need to continue */
    if (!content)
        return;
    w = h;

    x1 = s->hw_cursor_x;
    if (x1 >= s->last_scr_width)
        return;
    x2 = s->hw_cursor_x + w;
    if (x2 > s->last_scr_width)
        x2 = s->last_scr_width;
    w = x2 - x1;
    palette = s->cirrus_hidden_palette;
    color0 = s->rgb_to_pixel(c6_to_8(palette[0x0 * 3]),
                             c6_to_8(palette[0x0 * 3 + 1]),
                             c6_to_8(palette[0x0 * 3 + 2]));
    color1 = s->rgb_to_pixel(c6_to_8(palette[0xf * 3]),
                             c6_to_8(palette[0xf * 3 + 1]),
                             c6_to_8(palette[0xf * 3 + 2]));
    bpp = ((s->ds->depth + 7) >> 3);
    d1 += x1 * bpp;
    switch(s->ds->depth) {
    default:
        break;
    case 8:
        vga_draw_cursor_line_8(d1, src, poffset, w, color0, color1, 0xff);
        break;
    case 15:
        vga_draw_cursor_line_16(d1, src, poffset, w, color0, color1, 0x7fff);
        break;
    case 16:
        vga_draw_cursor_line_16(d1, src, poffset, w, color0, color1, 0xffff);
        break;
    case 32:
        vga_draw_cursor_line_32(d1, src, poffset, w, color0, color1, 0xffffff);
        break;
    }
}