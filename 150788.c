static void cirrus_bitblt_reset(CirrusVGAState * s)
{
    s->gr[0x31] &=
	~(CIRRUS_BLT_START | CIRRUS_BLT_BUSY | CIRRUS_BLT_FIFOUSED);
    s->cirrus_srcptr = &s->cirrus_bltbuf[0];
    s->cirrus_srcptr_end = &s->cirrus_bltbuf[0];
    s->cirrus_srccounter = 0;
    cirrus_update_memory_access(s);
}