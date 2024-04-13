static void scsi_cd_realize(SCSIDevice *dev, Error **errp)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, dev);
    AioContext *ctx;
    int ret;

    if (!dev->conf.blk) {
        /* Anonymous BlockBackend for an empty drive. As we put it into
         * dev->conf, qdev takes care of detaching on unplug. */
        dev->conf.blk = blk_new(qemu_get_aio_context(), 0, BLK_PERM_ALL);
        ret = blk_attach_dev(dev->conf.blk, &dev->qdev);
        assert(ret == 0);
    }

    ctx = blk_get_aio_context(dev->conf.blk);
    aio_context_acquire(ctx);
    s->qdev.blocksize = 2048;
    s->qdev.type = TYPE_ROM;
    s->features |= 1 << SCSI_DISK_F_REMOVABLE;
    if (!s->product) {
        s->product = g_strdup("QEMU CD-ROM");
    }
    scsi_realize(&s->qdev, errp);
    aio_context_release(ctx);
}