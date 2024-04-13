static void scsi_realize(SCSIDevice *dev, Error **errp)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, dev);
    bool read_only;

    if (!s->qdev.conf.blk) {
        error_setg(errp, "drive property not set");
        return;
    }

    if (!(s->features & (1 << SCSI_DISK_F_REMOVABLE)) &&
        !blk_is_inserted(s->qdev.conf.blk)) {
        error_setg(errp, "Device needs media, but drive is empty");
        return;
    }

    if (!blkconf_blocksizes(&s->qdev.conf, errp)) {
        return;
    }

    if (blk_get_aio_context(s->qdev.conf.blk) != qemu_get_aio_context() &&
        !s->qdev.hba_supports_iothread)
    {
        error_setg(errp, "HBA does not support iothreads");
        return;
    }

    if (dev->type == TYPE_DISK) {
        if (!blkconf_geometry(&dev->conf, NULL, 65535, 255, 255, errp)) {
            return;
        }
    }

    read_only = !blk_supports_write_perm(s->qdev.conf.blk);
    if (dev->type == TYPE_ROM) {
        read_only = true;
    }

    if (!blkconf_apply_backend_options(&dev->conf, read_only,
                                       dev->type == TYPE_DISK, errp)) {
        return;
    }

    if (s->qdev.conf.discard_granularity == -1) {
        s->qdev.conf.discard_granularity =
            MAX(s->qdev.conf.logical_block_size, DEFAULT_DISCARD_GRANULARITY);
    }

    if (!s->version) {
        s->version = g_strdup(qemu_hw_version());
    }
    if (!s->vendor) {
        s->vendor = g_strdup("QEMU");
    }
    if (!s->device_id) {
        if (s->serial) {
            s->device_id = g_strdup_printf("%.20s", s->serial);
        } else {
            const char *str = blk_name(s->qdev.conf.blk);
            if (str && *str) {
                s->device_id = g_strdup(str);
            }
        }
    }

    if (blk_is_sg(s->qdev.conf.blk)) {
        error_setg(errp, "unwanted /dev/sg*");
        return;
    }

    if ((s->features & (1 << SCSI_DISK_F_REMOVABLE)) &&
            !(s->features & (1 << SCSI_DISK_F_NO_REMOVABLE_DEVOPS))) {
        blk_set_dev_ops(s->qdev.conf.blk, &scsi_disk_removable_block_ops, s);
    } else {
        blk_set_dev_ops(s->qdev.conf.blk, &scsi_disk_block_ops, s);
    }
    blk_set_guest_block_size(s->qdev.conf.blk, s->qdev.blocksize);

    blk_iostatus_enable(s->qdev.conf.blk);

    add_boot_device_lchs(&dev->qdev, NULL,
                         dev->conf.lcyls,
                         dev->conf.lheads,
                         dev->conf.lsecs);
}