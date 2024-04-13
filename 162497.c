static void dp8393x_do_receiver_disable(dp8393xState *s)
{
    s->regs[SONIC_CR] &= ~SONIC_CR_RXEN;
}