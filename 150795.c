static void cirrus_vga_mem_writeb(void *opaque, target_phys_addr_t addr,
                                  uint32_t mem_value)
{
    CirrusVGAState *s = opaque;
    unsigned bank_index;
    unsigned bank_offset;
    unsigned mode;

    if ((s->sr[0x07] & 0x01) == 0) {
	vga_mem_writeb(s, addr, mem_value);
        return;
    }

    addr &= 0x1ffff;

    if (addr < 0x10000) {
	if (s->cirrus_srcptr != s->cirrus_srcptr_end) {
	    /* bitblt */
	    *s->cirrus_srcptr++ = (uint8_t) mem_value;
	    if (s->cirrus_srcptr >= s->cirrus_srcptr_end) {
		cirrus_bitblt_cputovideo_next(s);
	    }
	} else {
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
		mode = s->gr[0x05] & 0x7;
		if (mode < 4 || mode > 5 || ((s->gr[0x0B] & 0x4) == 0)) {
		    *(s->vram_ptr + bank_offset) = mem_value;
		    cpu_physical_memory_set_dirty(s->vram_offset +
						  bank_offset);
		} else {
		    if ((s->gr[0x0B] & 0x14) != 0x14) {
			cirrus_mem_writeb_mode4and5_8bpp(s, mode,
							 bank_offset,
							 mem_value);
		    } else {
			cirrus_mem_writeb_mode4and5_16bpp(s, mode,
							  bank_offset,
							  mem_value);
		    }
		}
	    }
	}
    } else if (addr >= 0x18000 && addr < 0x18100) {
	/* memory-mapped I/O */
	if ((s->sr[0x17] & 0x44) == 0x04) {
	    cirrus_mmio_blt_write(s, addr & 0xff, mem_value);
	}
    } else {
#ifdef DEBUG_CIRRUS
	printf("cirrus: mem_writeb %06x value %02x\n", addr, mem_value);
#endif
    }
}