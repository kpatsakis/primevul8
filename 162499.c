static void dp8393x_realize(DeviceState *dev, Error **errp)
{
    dp8393xState *s = DP8393X(dev);
    int i, checksum;
    uint8_t *prom;
    Error *local_err = NULL;

    address_space_init(&s->as, s->dma_mr, "dp8393x");
    memory_region_init_io(&s->mmio, OBJECT(dev), &dp8393x_ops, s,
                          "dp8393x-regs", 0x40 << s->it_shift);

    s->nic = qemu_new_nic(&net_dp83932_info, &s->conf,
                          object_get_typename(OBJECT(dev)), dev->id, s);
    qemu_format_nic_info_str(qemu_get_queue(s->nic), s->conf.macaddr.a);

    s->watchdog = timer_new_ns(QEMU_CLOCK_VIRTUAL, dp8393x_watchdog, s);

    memory_region_init_rom(&s->prom, OBJECT(dev), "dp8393x-prom",
                           SONIC_PROM_SIZE, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }
    prom = memory_region_get_ram_ptr(&s->prom);
    checksum = 0;
    for (i = 0; i < 6; i++) {
        prom[i] = s->conf.macaddr.a[i];
        checksum += prom[i];
        if (checksum > 0xff) {
            checksum = (checksum + 1) & 0xff;
        }
    }
    prom[7] = 0xff - checksum;
}