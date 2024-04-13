static void scsi_disk_unit_attention_reported(SCSIDevice *dev)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, dev);
    if (s->media_changed) {
        s->media_changed = false;
        scsi_device_set_ua(&s->qdev, SENSE_CODE(MEDIUM_CHANGED));
    }
}