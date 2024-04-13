
static const IntelHDAReg *intel_hda_reg_find(IntelHDAState *d, hwaddr addr)
{
    const IntelHDAReg *reg;

    if (addr >= ARRAY_SIZE(regtab)) {
        goto noreg;
    }
    reg = regtab+addr;
    if (reg->name == NULL) {
        goto noreg;
    }
    return reg;

noreg:
    dprint(d, 1, "unknown register, addr 0x%x\n", (int) addr);
    return NULL;