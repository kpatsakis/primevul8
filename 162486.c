static uint32_t dp8393x_tsa(dp8393xState *s)
{
    return (s->regs[SONIC_TSA1] << 16) | s->regs[SONIC_TSA0];
}