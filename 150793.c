static void cirrus_pci_mmio_map(PCIDevice *d, int region_num,
				uint32_t addr, uint32_t size, int type)
{
    CirrusVGAState *s = &((PCICirrusVGAState *)d)->cirrus_vga;

    cpu_register_physical_memory(addr, CIRRUS_PNPMMIO_SIZE,
				 s->cirrus_mmio_io_addr);
}