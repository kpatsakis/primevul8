static void scsi_disk_reset(DeviceState *dev)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev.qdev, dev);
    uint64_t nb_sectors;

    scsi_device_purge_requests(&s->qdev, SENSE_CODE(RESET));

    blk_get_geometry(s->qdev.conf.blk, &nb_sectors);
    nb_sectors /= s->qdev.blocksize / BDRV_SECTOR_SIZE;
    if (nb_sectors) {
        nb_sectors--;
    }
    s->qdev.max_lba = nb_sectors;
    /* reset tray statuses */
    s->tray_locked = 0;
    s->tray_open = 0;

    s->qdev.scsi_version = s->qdev.default_scsi_version;
}