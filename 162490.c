static void dp8393x_do_read_rra(dp8393xState *s)
{
    int width, size;

    /* Read memory */
    width = (s->regs[SONIC_DCR] & SONIC_DCR_DW) ? 2 : 1;
    size = sizeof(uint16_t) * 4 * width;
    address_space_read(&s->as, dp8393x_rrp(s),
                       MEMTXATTRS_UNSPECIFIED, s->data, size);

    /* Update SONIC registers */
    s->regs[SONIC_CRBA0] = dp8393x_get(s, width, 0);
    s->regs[SONIC_CRBA1] = dp8393x_get(s, width, 1);
    s->regs[SONIC_RBWC0] = dp8393x_get(s, width, 2);
    s->regs[SONIC_RBWC1] = dp8393x_get(s, width, 3);
    DPRINTF("CRBA0/1: 0x%04x/0x%04x, RBWC0/1: 0x%04x/0x%04x\n",
        s->regs[SONIC_CRBA0], s->regs[SONIC_CRBA1],
        s->regs[SONIC_RBWC0], s->regs[SONIC_RBWC1]);

    /* Go to next entry */
    s->regs[SONIC_RRP] += size;

    /* Handle wrap */
    if (s->regs[SONIC_RRP] == s->regs[SONIC_REA]) {
        s->regs[SONIC_RRP] = s->regs[SONIC_RSA];
    }

    /* Warn the host if CRBA now has the last available resource */
    if (s->regs[SONIC_RRP] == s->regs[SONIC_RWP])
    {
        s->regs[SONIC_ISR] |= SONIC_ISR_RBE;
        dp8393x_update_irq(s);
    }

    /* Allow packet reception */
    s->last_rba_is_full = false;
}