
static void intel_hda_mmio_write(void *opaque, hwaddr addr, uint64_t val,
                                 unsigned size)
{
    IntelHDAState *d = opaque;
    const IntelHDAReg *reg = intel_hda_reg_find(d, addr);

    intel_hda_reg_write(d, reg, val, MAKE_64BIT_MASK(0, size * 8));