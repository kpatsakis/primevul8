static void dp8393x_update_wt_regs(dp8393xState *s)
{
    int64_t elapsed;
    uint32_t val;

    if (s->regs[SONIC_CR] & SONIC_CR_STP) {
        timer_del(s->watchdog);
        return;
    }

    elapsed = s->wt_last_update - qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    val = dp8393x_wt(s);
    val -= elapsed / 5000000;
    s->regs[SONIC_WT1] = (val >> 16) & 0xffff;
    s->regs[SONIC_WT0] = (val >> 0)  & 0xffff;
    dp8393x_set_next_tick(s);

}