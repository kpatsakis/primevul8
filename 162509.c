static bool dp8393x_can_receive(NetClientState *nc)
{
    dp8393xState *s = qemu_get_nic_opaque(nc);

    return !!(s->regs[SONIC_CR] & SONIC_CR_RXEN);
}