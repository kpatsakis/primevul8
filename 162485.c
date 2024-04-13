static uint32_t dp8393x_wt(dp8393xState *s)
{
    return s->regs[SONIC_WT1] << 16 | s->regs[SONIC_WT0];
}