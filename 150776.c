static void cirrus_linear_writeb(void *opaque, target_phys_addr_t addr,
				 uint32_t val)
{
    CirrusVGAState *s = (CirrusVGAState *) opaque;
    unsigned mode;

    addr &= s->cirrus_addr_mask;

    if (((s->sr[0x17] & 0x44) == 0x44) &&
        ((addr & s->linear_mmio_mask) ==  s->linear_mmio_mask)) {
	/* memory-mapped I/O */
	cirrus_mmio_blt_write(s, addr & 0xff, val);
    } else if (s->cirrus_srcptr != s->cirrus_srcptr_end) {
	/* bitblt */
	*s->cirrus_srcptr++ = (uint8_t) val;
	if (s->cirrus_srcptr >= s->cirrus_srcptr_end) {
	    cirrus_bitblt_cputovideo_next(s);
	}
    } else {
	/* video memory */
	if ((s->gr[0x0B] & 0x14) == 0x14) {
	    addr <<= 4;
	} else if (s->gr[0x0B] & 0x02) {
	    addr <<= 3;
	}
	addr &= s->cirrus_addr_mask;

	mode = s->gr[0x05] & 0x7;
	if (mode < 4 || mode > 5 || ((s->gr[0x0B] & 0x4) == 0)) {
	    *(s->vram_ptr + addr) = (uint8_t) val;
	    cpu_physical_memory_set_dirty(s->vram_offset + addr);
	} else {
	    if ((s->gr[0x0B] & 0x14) != 0x14) {
		cirrus_mem_writeb_mode4and5_8bpp(s, mode, addr, val);
	    } else {
		cirrus_mem_writeb_mode4and5_16bpp(s, mode, addr, val);
	    }
	}
    }
}