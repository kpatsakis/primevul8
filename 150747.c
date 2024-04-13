static void cirrus_mmio_writew(void *opaque, target_phys_addr_t addr,
			       uint32_t val)
{
#ifdef TARGET_WORDS_BIGENDIAN
    cirrus_mmio_writeb(opaque, addr, (val >> 8) & 0xff);
    cirrus_mmio_writeb(opaque, addr + 1, val & 0xff);
#else
    cirrus_mmio_writeb(opaque, addr, val & 0xff);
    cirrus_mmio_writeb(opaque, addr + 1, (val >> 8) & 0xff);
#endif
}