
static void intel_hda_regs_reset(IntelHDAState *d)
{
    uint32_t *addr;
    int i;

    for (i = 0; i < ARRAY_SIZE(regtab); i++) {
        if (regtab[i].name == NULL) {
            continue;
        }
        if (regtab[i].offset == 0) {
            continue;
        }
        addr = intel_hda_reg_addr(d, regtab + i);
        *addr = regtab[i].reset;
    }