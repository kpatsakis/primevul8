static uint32_t dp8393x_cdp(dp8393xState *s)
{
    return (s->regs[SONIC_URRA] << 16) | s->regs[SONIC_CDP];
}