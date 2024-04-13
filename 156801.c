static void scsi_block_realize(SCSIDevice *dev, Error **errp)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, dev);
    AioContext *ctx;
    int sg_version;
    int rc;

    if (!s->qdev.conf.blk) {
        error_setg(errp, "drive property not set");
        return;
    }

    if (s->rotation_rate) {
        error_report_once("rotation_rate is specified for scsi-block but is "
                          "not implemented. This option is deprecated and will "
                          "be removed in a future version");
    }

    ctx = blk_get_aio_context(s->qdev.conf.blk);
    aio_context_acquire(ctx);

    /* check we are using a driver managing SG_IO (version 3 and after) */
    rc = blk_ioctl(s->qdev.conf.blk, SG_GET_VERSION_NUM, &sg_version);
    if (rc < 0) {
        error_setg_errno(errp, -rc, "cannot get SG_IO version number");
        if (rc != -EPERM) {
            error_append_hint(errp, "Is this a SCSI device?\n");
        }
        goto out;
    }
    if (sg_version < 30000) {
        error_setg(errp, "scsi generic interface too old");
        goto out;
    }

    /* get device type from INQUIRY data */
    rc = get_device_type(s);
    if (rc < 0) {
        error_setg(errp, "INQUIRY failed");
        goto out;
    }

    /* Make a guess for the block size, we'll fix it when the guest sends.
     * READ CAPACITY.  If they don't, they likely would assume these sizes
     * anyway. (TODO: check in /sys).
     */
    if (s->qdev.type == TYPE_ROM || s->qdev.type == TYPE_WORM) {
        s->qdev.blocksize = 2048;
    } else {
        s->qdev.blocksize = 512;
    }

    /* Makes the scsi-block device not removable by using HMP and QMP eject
     * command.
     */
    s->features |= (1 << SCSI_DISK_F_NO_REMOVABLE_DEVOPS);

    scsi_realize(&s->qdev, errp);
    scsi_generic_read_device_inquiry(&s->qdev);

out:
    aio_context_release(ctx);
}