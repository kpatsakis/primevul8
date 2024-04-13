
static uint64_t intel_hda_mmio_read(void *opaque, hwaddr addr, unsigned size)
{
    IntelHDAState *d = opaque;
    const IntelHDAReg *reg = intel_hda_reg_find(d, addr);

    return intel_hda_reg_read(d, reg, MAKE_64BIT_MASK(0, size * 8));