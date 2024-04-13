static uint32_t dp8393x_crda(dp8393xState *s)
{
    return (s->regs[SONIC_URDA] << 16) |
           (s->regs[SONIC_CRDA] & SONIC_DESC_ADDR);
}