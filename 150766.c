static void cirrus_get_offsets(VGAState *s1,
                               uint32_t *pline_offset,
                               uint32_t *pstart_addr,
                               uint32_t *pline_compare)
{
    CirrusVGAState * s = (CirrusVGAState *)s1;
    uint32_t start_addr, line_offset, line_compare;

    line_offset = s->cr[0x13]
	| ((s->cr[0x1b] & 0x10) << 4);
    line_offset <<= 3;
    *pline_offset = line_offset;

    start_addr = (s->cr[0x0c] << 8)
	| s->cr[0x0d]
	| ((s->cr[0x1b] & 0x01) << 16)
	| ((s->cr[0x1b] & 0x0c) << 15)
	| ((s->cr[0x1d] & 0x80) << 12);
    *pstart_addr = start_addr;

    line_compare = s->cr[0x18] |
        ((s->cr[0x07] & 0x10) << 4) |
        ((s->cr[0x09] & 0x40) << 3);
    *pline_compare = line_compare;
}