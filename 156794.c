static void scsi_hd_realize(SCSIDevice *dev, Error **errp)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, dev);
    AioContext *ctx = NULL;
    /* can happen for devices without drive. The error message for missing
     * backend will be issued in scsi_realize
     */
    if (s->qdev.conf.blk) {
        ctx = blk_get_aio_context(s->qdev.conf.blk);
        aio_context_acquire(ctx);
        if (!blkconf_blocksizes(&s->qdev.conf, errp)) {
            goto out;
        }
    }
    s->qdev.blocksize = s->qdev.conf.logical_block_size;
    s->qdev.type = TYPE_DISK;
    if (!s->product) {
        s->product = g_strdup("QEMU HARDDISK");
    }
    scsi_realize(&s->qdev, errp);
out:
    if (ctx) {
        aio_context_release(ctx);
    }
}