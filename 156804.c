static int get_device_type(SCSIDiskState *s)
{
    uint8_t cmd[16];
    uint8_t buf[36];
    int ret;

    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));
    cmd[0] = INQUIRY;
    cmd[4] = sizeof(buf);

    ret = scsi_SG_IO_FROM_DEV(s->qdev.conf.blk, cmd, sizeof(cmd),
                              buf, sizeof(buf), s->qdev.io_timeout);
    if (ret < 0) {
        return -1;
    }
    s->qdev.type = buf[0];
    if (buf[1] & 0x80) {
        s->features |= 1 << SCSI_DISK_F_REMOVABLE;
    }
    return 0;
}