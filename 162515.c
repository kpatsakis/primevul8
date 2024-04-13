static uint32_t dp8393x_rbwc(dp8393xState *s)
{
    return (s->regs[SONIC_RBWC1] << 16) | s->regs[SONIC_RBWC0];
}