static void cirrus_vga_mem_writel(void *opaque, target_phys_addr_t addr, uint32_t val)
{
#ifdef TARGET_WORDS_BIGENDIAN
    cirrus_vga_mem_writeb(opaque, addr, (val >> 24) & 0xff);
    cirrus_vga_mem_writeb(opaque, addr + 1, (val >> 16) & 0xff);
    cirrus_vga_mem_writeb(opaque, addr + 2, (val >> 8) & 0xff);
    cirrus_vga_mem_writeb(opaque, addr + 3, val & 0xff);
#else
    cirrus_vga_mem_writeb(opaque, addr, val & 0xff);
    cirrus_vga_mem_writeb(opaque, addr + 1, (val >> 8) & 0xff);
    cirrus_vga_mem_writeb(opaque, addr + 2, (val >> 16) & 0xff);
    cirrus_vga_mem_writeb(opaque, addr + 3, (val >> 24) & 0xff);
#endif
}