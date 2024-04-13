static inline void cirrus_bitblt_bgcol(CirrusVGAState *s)
{
    unsigned int color;
    switch (s->cirrus_blt_pixelwidth) {
    case 1:
        s->cirrus_blt_bgcol = s->cirrus_shadow_gr0;
        break;
    case 2:
        color = s->cirrus_shadow_gr0 | (s->gr[0x10] << 8);
        s->cirrus_blt_bgcol = le16_to_cpu(color);
        break;
    case 3:
        s->cirrus_blt_bgcol = s->cirrus_shadow_gr0 |
            (s->gr[0x10] << 8) | (s->gr[0x12] << 16);
        break;
    default:
    case 4:
        color = s->cirrus_shadow_gr0 | (s->gr[0x10] << 8) |
            (s->gr[0x12] << 16) | (s->gr[0x14] << 24);
        s->cirrus_blt_bgcol = le32_to_cpu(color);
        break;
    }
}