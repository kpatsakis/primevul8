static int cirrus_get_bpp(VGAState *s1)
{
    CirrusVGAState * s = (CirrusVGAState *)s1;
    uint32_t ret = 8;

    if ((s->sr[0x07] & 0x01) != 0) {
	/* Cirrus SVGA */
	switch (s->sr[0x07] & CIRRUS_SR7_BPP_MASK) {
	case CIRRUS_SR7_BPP_8:
	    ret = 8;
	    break;
	case CIRRUS_SR7_BPP_16_DOUBLEVCLK:
	    ret = cirrus_get_bpp16_depth(s);
	    break;
	case CIRRUS_SR7_BPP_24:
	    ret = 24;
	    break;
	case CIRRUS_SR7_BPP_16:
	    ret = cirrus_get_bpp16_depth(s);
	    break;
	case CIRRUS_SR7_BPP_32:
	    ret = 32;
	    break;
	default:
#ifdef DEBUG_CIRRUS
	    printf("cirrus: unknown bpp - sr7=%x\n", s->sr[0x7]);
#endif
	    ret = 8;
	    break;
	}
    } else {
	/* VGA */
	ret = 0;
    }

    return ret;
}