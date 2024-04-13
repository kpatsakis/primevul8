static uint32_t cirrus_vga_mem_readb(void *opaque, target_phys_addr_t addr)
{
    CirrusVGAState *s = opaque;
    unsigned bank_index;
    unsigned bank_offset;
    uint32_t val;

    if ((s->sr[0x07] & 0x01) == 0) {
	return vga_mem_readb(s, addr);
    }

    addr &= 0x1ffff;

    if (addr < 0x10000) {
	/* XXX handle bitblt */
	/* video memory */
	bank_index = addr >> 15;
	bank_offset = addr & 0x7fff;
	if (bank_offset < s->cirrus_bank_limit[bank_index]) {
	    bank_offset += s->cirrus_bank_base[bank_index];
	    if ((s->gr[0x0B] & 0x14) == 0x14) {
		bank_offset <<= 4;
	    } else if (s->gr[0x0B] & 0x02) {
		bank_offset <<= 3;
	    }
	    bank_offset &= s->cirrus_addr_mask;
	    val = *(s->vram_ptr + bank_offset);
	} else
	    val = 0xff;
    } else if (addr >= 0x18000 && addr < 0x18100) {
	/* memory-mapped I/O */
	val = 0xff;
	if ((s->sr[0x17] & 0x44) == 0x04) {
	    val = cirrus_mmio_blt_read(s, addr & 0xff);
	}
    } else {
	val = 0xff;
#ifdef DEBUG_CIRRUS
	printf("cirrus: mem_readb %06x\n", addr);
#endif
    }
    return val;
}