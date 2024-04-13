static void cirrus_pci_lfb_map(PCIDevice *d, int region_num,
			       uint32_t addr, uint32_t size, int type)
{
    CirrusVGAState *s = &((PCICirrusVGAState *)d)->cirrus_vga;

    /* XXX: add byte swapping apertures */
    cpu_register_physical_memory(addr, s->vram_size,
				 s->cirrus_linear_io_addr);
    cpu_register_physical_memory(addr + 0x1000000, 0x400000,
				 s->cirrus_linear_bitblt_io_addr);
}