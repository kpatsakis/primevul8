static uint32_t dp8393x_ttda(dp8393xState *s)
{
    return (s->regs[SONIC_UTDA] << 16) |
           (s->regs[SONIC_TTDA] & SONIC_DESC_ADDR);
}