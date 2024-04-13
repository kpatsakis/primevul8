static void scsi_disk_resize_cb(void *opaque)
{
    SCSIDiskState *s = opaque;

    /* SPC lists this sense code as available only for
     * direct-access devices.
     */
    if (s->qdev.type == TYPE_DISK) {
        scsi_device_report_change(&s->qdev, SENSE_CODE(CAPACITY_CHANGED));
    }
}