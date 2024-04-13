static void dp8393x_do_software_reset(dp8393xState *s)
{
    timer_del(s->watchdog);

    s->regs[SONIC_CR] &= ~(SONIC_CR_LCAM | SONIC_CR_RRRA | SONIC_CR_TXP | SONIC_CR_HTX);
    s->regs[SONIC_CR] |= SONIC_CR_RST | SONIC_CR_RXDIS;
}