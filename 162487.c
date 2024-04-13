static void dp8393x_do_receiver_enable(dp8393xState *s)
{
    s->regs[SONIC_CR] &= ~SONIC_CR_RXDIS;
    if (dp8393x_can_receive(s->nic->ncs)) {
        qemu_flush_queued_packets(qemu_get_queue(s->nic));
    }
}