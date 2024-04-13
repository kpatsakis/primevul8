static uint32_t dp8393x_crba(dp8393xState *s)
{
    return (s->regs[SONIC_CRBA1] << 16) | s->regs[SONIC_CRBA0];
}