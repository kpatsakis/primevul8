
static uint32_t *intel_hda_reg_addr(IntelHDAState *d, const IntelHDAReg *reg)
{
    uint8_t *addr = (void*)d;

    addr += reg->offset;
    return (uint32_t*)addr;