static void cirrus_write_bitblt(CirrusVGAState * s, unsigned reg_value)
{
    unsigned old_value;

    old_value = s->gr[0x31];
    s->gr[0x31] = reg_value;

    if (((old_value & CIRRUS_BLT_RESET) != 0) &&
	((reg_value & CIRRUS_BLT_RESET) == 0)) {
	cirrus_bitblt_reset(s);
    } else if (((old_value & CIRRUS_BLT_START) == 0) &&
	       ((reg_value & CIRRUS_BLT_START) != 0)) {
	cirrus_bitblt_start(s);
    }
}