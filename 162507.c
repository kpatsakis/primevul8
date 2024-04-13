static void dp8393x_set_next_tick(dp8393xState *s)
{
    uint32_t ticks;
    int64_t delay;

    if (s->regs[SONIC_CR] & SONIC_CR_STP) {
        timer_del(s->watchdog);
        return;
    }

    ticks = dp8393x_wt(s);
    s->wt_last_update = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    delay = NANOSECONDS_PER_SECOND * ticks / 5000000;
    timer_mod(s->watchdog, s->wt_last_update + delay);
}