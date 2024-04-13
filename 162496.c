static void dp8393x_watchdog(void *opaque)
{
    dp8393xState *s = opaque;

    if (s->regs[SONIC_CR] & SONIC_CR_STP) {
        return;
    }

    s->regs[SONIC_WT1] = 0xffff;
    s->regs[SONIC_WT0] = 0xffff;
    dp8393x_set_next_tick(s);

    /* Signal underflow */
    s->regs[SONIC_ISR] |= SONIC_ISR_TC;
    dp8393x_update_irq(s);
}