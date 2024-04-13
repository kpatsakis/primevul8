static uint32_t cirrus_mmio_readb(void *opaque, target_phys_addr_t addr)
{
    CirrusVGAState *s = (CirrusVGAState *) opaque;

    addr &= CIRRUS_PNPMMIO_SIZE - 1;

    if (addr >= 0x100) {
        return cirrus_mmio_blt_read(s, addr - 0x100);
    } else {
        return vga_ioport_read(s, addr + 0x3c0);
    }
}