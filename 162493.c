static void dp8393x_do_stop_timer(dp8393xState *s)
{
    s->regs[SONIC_CR] &= ~SONIC_CR_ST;
    dp8393x_update_wt_regs(s);
}