static void cirrus_get_resolution(VGAState *s, int *pwidth, int *pheight)
{
    int width, height;

    width = (s->cr[0x01] + 1) * 8;
    height = s->cr[0x12] |
        ((s->cr[0x07] & 0x02) << 7) |
        ((s->cr[0x07] & 0x40) << 3);
    height = (height + 1);
    /* interlace support */
    if (s->cr[0x1a] & 0x01)
        height = height * 2;
    *pwidth = width;
    *pheight = height;
}