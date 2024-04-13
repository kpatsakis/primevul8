static uint32_t cirrus_linear_bitblt_readw(void *opaque, target_phys_addr_t addr)
{
    uint32_t v;
#ifdef TARGET_WORDS_BIGENDIAN
    v = cirrus_linear_bitblt_readb(opaque, addr) << 8;
    v |= cirrus_linear_bitblt_readb(opaque, addr + 1);
#else
    v = cirrus_linear_bitblt_readb(opaque, addr);
    v |= cirrus_linear_bitblt_readb(opaque, addr + 1) << 8;
#endif
    return v;
}