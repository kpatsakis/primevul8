static void dp8393x_do_command(dp8393xState *s, uint16_t command)
{
    if ((s->regs[SONIC_CR] & SONIC_CR_RST) && !(command & SONIC_CR_RST)) {
        s->regs[SONIC_CR] &= ~SONIC_CR_RST;
        return;
    }

    s->regs[SONIC_CR] |= (command & SONIC_CR_MASK);

    if (command & SONIC_CR_HTX)
        dp8393x_do_halt_transmission(s);
    if (command & SONIC_CR_TXP)
        dp8393x_do_transmit_packets(s);
    if (command & SONIC_CR_RXDIS)
        dp8393x_do_receiver_disable(s);
    if (command & SONIC_CR_RXEN)
        dp8393x_do_receiver_enable(s);
    if (command & SONIC_CR_STP)
        dp8393x_do_stop_timer(s);
    if (command & SONIC_CR_ST)
        dp8393x_do_start_timer(s);
    if (command & SONIC_CR_RST)
        dp8393x_do_software_reset(s);
    if (command & SONIC_CR_RRRA) {
        dp8393x_do_read_rra(s);
        s->regs[SONIC_CR] &= ~SONIC_CR_RRRA;
    }
    if (command & SONIC_CR_LCAM)
        dp8393x_do_load_cam(s);
}