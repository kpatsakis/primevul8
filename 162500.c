static void dp8393x_do_load_cam(dp8393xState *s)
{
    int width, size;
    uint16_t index = 0;

    width = (s->regs[SONIC_DCR] & SONIC_DCR_DW) ? 2 : 1;
    size = sizeof(uint16_t) * 4 * width;

    while (s->regs[SONIC_CDC] & 0x1f) {
        /* Fill current entry */
        address_space_read(&s->as, dp8393x_cdp(s),
                           MEMTXATTRS_UNSPECIFIED, s->data, size);
        s->cam[index][0] = dp8393x_get(s, width, 1) & 0xff;
        s->cam[index][1] = dp8393x_get(s, width, 1) >> 8;
        s->cam[index][2] = dp8393x_get(s, width, 2) & 0xff;
        s->cam[index][3] = dp8393x_get(s, width, 2) >> 8;
        s->cam[index][4] = dp8393x_get(s, width, 3) & 0xff;
        s->cam[index][5] = dp8393x_get(s, width, 3) >> 8;
        DPRINTF("load cam[%d] with %02x%02x%02x%02x%02x%02x\n", index,
            s->cam[index][0], s->cam[index][1], s->cam[index][2],
            s->cam[index][3], s->cam[index][4], s->cam[index][5]);
        /* Move to next entry */
        s->regs[SONIC_CDC]--;
        s->regs[SONIC_CDP] += size;
        index++;
    }

    /* Read CAM enable */
    address_space_read(&s->as, dp8393x_cdp(s),
                       MEMTXATTRS_UNSPECIFIED, s->data, size);
    s->regs[SONIC_CE] = dp8393x_get(s, width, 0);
    DPRINTF("load cam done. cam enable mask 0x%04x\n", s->regs[SONIC_CE]);

    /* Done */
    s->regs[SONIC_CR] &= ~SONIC_CR_LCAM;
    s->regs[SONIC_ISR] |= SONIC_ISR_LCD;
    dp8393x_update_irq(s);
}