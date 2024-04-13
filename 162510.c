static void dp8393x_do_start_timer(dp8393xState *s)
{
    s->regs[SONIC_CR] &= ~SONIC_CR_STP;
    dp8393x_set_next_tick(s);
}